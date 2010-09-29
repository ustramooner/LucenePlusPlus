/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
#include "SegmentInfos.h"
#include "_SegmentInfos.h"
#include "StringUtils.h"
#include "SegmentInfo.h"
#include "IndexFileNames.h"
#include "Directory.h"
#include "ChecksumIndexInput.h"
#include "ChecksumIndexOutput.h"
#include "IndexCommit.h"
#include "InfoStream.h"
#include "TestPoint.h"
#include "MiscUtils.h"

namespace Lucene
{
    /// The file format version, a negative number.  Works since counter, the old 1st entry, is always >= 0
    const int32_t SegmentInfos::FORMAT = -1;
    
    /// This format adds details used for lockless commits.  It differs slightly from the previous format in that file names
	/// are never re-used (write once).  Instead, each file is written to the next generation.  For example, segments_1, 
	/// segments_2, etc.  This allows us to not use a commit lock.  
    const int32_t SegmentInfos::FORMAT_LOCKLESS = -2;
    
    /// This format adds a "hasSingleNormFile" flag into each segment info.
    const int32_t SegmentInfos::FORMAT_SINGLE_NORM_FILE = -3;
    
    /// This format allows multiple segments to share a single vectors and stored fields file.
    const int32_t SegmentInfos::FORMAT_SHARED_DOC_STORE = -4;
    
    /// This format adds a checksum at the end of the file to ensure all bytes were successfully written.
    const int32_t SegmentInfos::FORMAT_CHECKSUM = -5;
    
    /// This format adds the deletion count for each segment. This way IndexWriter can efficiently report numDocs().
    const int32_t SegmentInfos::FORMAT_DEL_COUNT = -6;
    
    /// This format adds the boolean hasProx to record if any fields in the segment store prox information (ie, have
	/// omitTermFreqAndPositions == false)
    const int32_t SegmentInfos::FORMAT_HAS_PROX = -7;
    
    /// This format adds optional commit userData storage.
    const int32_t SegmentInfos::FORMAT_USER_DATA = -8;
    
    /// This format adds optional per-segment string diagnostics storage, and switches userData to Map
    const int32_t SegmentInfos::FORMAT_DIAGNOSTICS = -9;
    
    /// This must always point to the most recent file format.
    const int32_t SegmentInfos::CURRENT_FORMAT = SegmentInfos::FORMAT_DIAGNOSTICS;
    
    /// Advanced configuration of retry logic in loading segments_N file.
    int32_t SegmentInfos::defaultGenFileRetryCount = 10;
    int32_t SegmentInfos::defaultGenFileRetryPauseMsec = 50;
    int32_t SegmentInfos::defaultGenLookaheadCount = 10;
    
    MapStringString SegmentInfos::singletonUserData;
    InfoStreamPtr SegmentInfos::infoStream;
    
    SegmentInfos::SegmentInfos()
    {
        userData = MapStringString::newInstance();
        lastGeneration = 0;
        generation = 0;
        counter = 0;
        version = MiscUtils::currentTimeMillis();
    }
    
    SegmentInfos::~SegmentInfos()
    {
    }
    
    SegmentInfoPtr SegmentInfos::info(int32_t i)
    {
        return segmentInfos[i];
    }
    
    int64_t SegmentInfos::getCurrentSegmentGeneration(HashSet<String> files)
    {
        if (!files)
            return -1;
        int64_t max = -1;
        for (HashSet<String>::iterator file = files.begin(); file != files.end(); ++file)
        {
            if (boost::starts_with(*file, IndexFileNames::SEGMENTS()) && *file != IndexFileNames::SEGMENTS_GEN())
                max = std::max(generationFromSegmentsFileName(*file), max);
        }
        return max;
    }
    
    int64_t SegmentInfos::getCurrentSegmentGeneration(DirectoryPtr directory)
    {
        try
        {
            return getCurrentSegmentGeneration(directory->listAll());
        }
        catch (LuceneException&)
        {
            return -1;
        }
    }
    
    String SegmentInfos::getCurrentSegmentFileName(HashSet<String> files)
    {
        return IndexFileNames::fileNameFromGeneration(IndexFileNames::SEGMENTS(), L"", getCurrentSegmentGeneration(files));
    }
    
    String SegmentInfos::getCurrentSegmentFileName(DirectoryPtr directory)
    {
        return IndexFileNames::fileNameFromGeneration(IndexFileNames::SEGMENTS(), L"", getCurrentSegmentGeneration(directory));
    }
    
    String SegmentInfos::getCurrentSegmentFileName()
    {
        return IndexFileNames::fileNameFromGeneration(IndexFileNames::SEGMENTS(), L"", lastGeneration);
    }
    
    int64_t SegmentInfos::generationFromSegmentsFileName(const String& fileName)
    {
        if (fileName == IndexFileNames::SEGMENTS())
            return 0;
        else if (boost::starts_with(fileName, IndexFileNames::SEGMENTS()))
            return StringUtils::toLong(fileName.substr(wcslen(IndexFileNames::SEGMENTS().c_str()) + 1), StringUtils::CHARACTER_MAX_RADIX);
        else
            boost::throw_exception(IllegalArgumentException(L"FileName '" + fileName + L"' is not a segments file"));
        return 0;
    }
    
    String SegmentInfos::getNextSegmentFileName()
    {
        return IndexFileNames::fileNameFromGeneration(IndexFileNames::SEGMENTS(), L"", generation == -1 ? 1 : generation + 1);
    }
    
    void SegmentInfos::read(DirectoryPtr directory, const String& segmentFileName)
    {
        bool success = false;

        // clear any previous segments
        segmentInfos.clear();
        
        ChecksumIndexInputPtr input(newLucene<ChecksumIndexInput>(directory->openInput(segmentFileName)));
        
        generation = generationFromSegmentsFileName(segmentFileName);
        lastGeneration = generation;
        LuceneException finally;
        try
        {
            int32_t format = input->readInt();
            
            if (format < 0) // file contains explicit format info
            {
                if (format < CURRENT_FORMAT)
                    boost::throw_exception(CorruptIndexException(L"Unknown format version: " + StringUtils::toString(format)));
                version = input->readLong(); // read version
                counter = input->readInt(); // read counter
            }
            else
                counter = format;
            
            for (int32_t i = input->readInt(); i > 0; --i) // read segmentInfos
                segmentInfos.add(newLucene<SegmentInfo>(directory, format, input));
            
            // in old format the version number may be at the end of the file
            if (format >= 0)
            {
                if (input->getFilePointer() >= input->length())
                    version = MiscUtils::currentTimeMillis(); // old file format without version number
                else
                    input->readLong(); // read version
            }
            
            if (format <= FORMAT_USER_DATA)
            {
                if (format <= FORMAT_DIAGNOSTICS)
                    userData = input->readStringStringMap();
                else if (input->readByte() != 0)
                {
                    if (!singletonUserData)
                        singletonUserData = MapStringString::newInstance();
                    singletonUserData[String(L"userData")] = input->readString();
                    userData = singletonUserData;
                }
                else
                    userData.clear();
            }
            else
                userData.clear();
            
            if (format <= FORMAT_CHECKSUM)
            {
                int64_t checksumNow = input->getChecksum();
                int64_t checksumThen = input->readLong();
                if (checksumNow != checksumThen)
                    boost::throw_exception(CorruptIndexException(L"Checksum mismatch in segments file"));
            }
            
            success = true;
        }
        catch (LuceneException& e)
        {
            finally = e;
        }
        
        input->close();
            
        // clear any segment infos we had loaded so we have a clean slate on retry
        if (!success)
            segmentInfos.clear();
        
        finally.throwException();
    }
    
    void SegmentInfos::read(DirectoryPtr directory)
    {
        lastGeneration = -1;
        generation = lastGeneration;
        newLucene<FindSegmentsRead>(shared_from_this(), directory)->run();
    }
    
    void SegmentInfos::write(DirectoryPtr directory)
    {
        String segmentFileName(getNextSegmentFileName());
        
        // always advance the generation on write
        if (generation == -1)
            generation = 1;
        else
            ++generation;
        
        ChecksumIndexOutputPtr segnOutput(newLucene<ChecksumIndexOutput>(directory->createOutput(segmentFileName)));
        
        bool success = false;
        LuceneException finally;
        try
        {
            segnOutput->writeInt(CURRENT_FORMAT); // write FORMAT
            segnOutput->writeLong(++version); // every write changes the index
            segnOutput->writeInt(counter); // write counter
            segnOutput->writeInt(segmentInfos.size()); // write infos
            for (Collection<SegmentInfoPtr>::iterator seginfo = segmentInfos.begin(); seginfo != segmentInfos.end(); ++seginfo)
                (*seginfo)->write(segnOutput);
            segnOutput->writeStringStringMap(userData);
            segnOutput->prepareCommit();
            success = true;
            pendingSegnOutput = segnOutput;
        }
        catch (LuceneException& e)
        {
            finally = e;
        }
        
        if (!success)
        {
            // We hit an exception above; try to close the file but suppress any exception
            try
            {
                segnOutput->close();
            }
            catch (...)
            {
                // Suppress so we keep throwing the original exception
            }
            
            try
            {
                // try not to leave a truncated segments_n file in the index
                directory->deleteFile(segmentFileName);
            }
            catch (...)
            {
                // Suppress so we keep throwing the original exception
            }
        }
        
        finally.throwException();
    }
    
    LuceneObjectPtr SegmentInfos::clone(LuceneObjectPtr other)
    {
        LuceneObjectPtr clone = SegmentInfoCollection::clone(other ? other : newLucene<SegmentInfos>());
        SegmentInfosPtr cloneInfos(boost::dynamic_pointer_cast<SegmentInfos>(clone));
        cloneInfos->counter = counter;
        cloneInfos->generation = generation;
        cloneInfos->lastGeneration = lastGeneration;
        cloneInfos->version = version;
        cloneInfos->pendingSegnOutput = pendingSegnOutput;
        for (int32_t i = 0; i < cloneInfos->size(); ++i)
            cloneInfos->segmentInfos[i] = boost::dynamic_pointer_cast<SegmentInfo>(cloneInfos->info(i)->clone());
        cloneInfos->userData = MapStringString::newInstance();
        cloneInfos->userData.putAll(userData.begin(), userData.end());
        return cloneInfos;
    }
    
    int64_t SegmentInfos::getVersion()
    {
        return version;
    }
    
    int64_t SegmentInfos::getGeneration()
    {
        return generation;
    }
    
    int64_t SegmentInfos::getLastGeneration()
    {
        return lastGeneration;
    }
    
    int64_t SegmentInfos::readCurrentVersion(DirectoryPtr directory)
    {
        // Fully read the segments file: this ensures that it's completely written so that if IndexWriter.prepareCommit has been called 
        // (but not yet commit), then the reader will still see itself as current.
        SegmentInfosPtr sis(newLucene<SegmentInfos>());
        sis->read(directory);
        return sis->getVersion();
    }
    
    MapStringString SegmentInfos::readCurrentUserData(DirectoryPtr directory)
    {
        SegmentInfosPtr sis(newLucene<SegmentInfos>());
        sis->read(directory);
        return sis->getUserData();
    }
    
    void SegmentInfos::setInfoStream(InfoStreamPtr infoStream)
    {
        SegmentInfos::infoStream = infoStream;
    }
    
    void SegmentInfos::setDefaultGenFileRetryCount(int32_t count)
    {
        defaultGenFileRetryCount = count;
    }
    
    int32_t SegmentInfos::getDefaultGenFileRetryCount()
    {
        return defaultGenFileRetryCount;
    }
    
    void SegmentInfos::setDefaultGenFileRetryPauseMsec(int32_t msec)
    {
        defaultGenFileRetryPauseMsec = msec;
    }
    
    int32_t SegmentInfos::getDefaultGenFileRetryPauseMsec()
    {
        return defaultGenFileRetryPauseMsec;
    }
    
    void SegmentInfos::setDefaultGenLookaheadCount(int32_t count)
    {
        defaultGenLookaheadCount = count;
    }
    
    int32_t SegmentInfos::getDefaultGenLookahedCount()
    {
        return defaultGenLookaheadCount;
    }
    
    InfoStreamPtr SegmentInfos::getInfoStream()
    {
        return infoStream;
    }
    
    void SegmentInfos::message(const String& message)
    {
        if (infoStream)
            *infoStream << L"SIS [" << message << L"]\n";
    }
    
    FindSegmentsFile::FindSegmentsFile(SegmentInfosPtr infos, DirectoryPtr directory)
    {
        this->_segmentInfos = infos;
        this->directory = directory;
    }
    
    FindSegmentsFile::~FindSegmentsFile()
    {
    }
    
    void FindSegmentsFile::doRun(IndexCommitPtr commit)
    {
        if (commit)
        {
            if (directory != commit->getDirectory())
                boost::throw_exception(IOException(L"The specified commit does not match the specified Directory"));
            runBody(commit->getSegmentsFileName());
            return;
        }
        
        String segmentFileName;
        int64_t lastGen = -1;
        int64_t gen = 0;
        int32_t genLookaheadCount = 0;
        bool retry = false;
        LuceneException exc;
        SegmentInfosPtr segmentInfos(_segmentInfos);
        
        int32_t method = 0;
        
        // Loop until we succeed in calling runBody() without hitting an IOException.  An IOException most likely
        // means a commit was in process and has finished, in the time it took us to load the now-old infos files
        // (and segments files).  It's also possible it's a true error (corrupt index).  To distinguish these,
        // on each retry we must see "forward progress" on which generation we are trying to load.  If we don't, 
        // then the original error is real and we throw it.

        // We have three methods for determining the current generation.  We try the first two in parallel, and
        // fall back to the third when necessary.
        
        while (true)
        {
            if (method == 0)
            {
                // Method 1: list the directory and use the highest segments_N file.  This method works well as long
                // as there is no stale caching on the directory contents (NOTE: NFS clients often have such stale caching)
                HashSet<String> files(directory->listAll());
                int64_t genA = segmentInfos->getCurrentSegmentGeneration(files);
                
                segmentInfos->message(L"directory listing genA=" + genA);
                
                // Method 2: open segments.gen and read its contents.  Then we take the larger of the two gen's.  This way, 
                // if either approach is hitting a stale cache (NFS) we have a better chance of getting the right generation.
                int64_t genB = -1;
                for (int32_t i = 0; i < SegmentInfos::defaultGenFileRetryCount; ++i)
                {
                    IndexInputPtr genInput;
                    try
                    {
                        genInput = directory->openInput(IndexFileNames::SEGMENTS_GEN());
                    }
                    catch (FileNotFoundException& e)
                    {
                        segmentInfos->message(L"Segments.gen open: FileNotFoundException " + e.getError());
                        break;
                    }
                    catch (IOException& e)
                    {
                        segmentInfos->message(L"Segments.gen open: IOException " + e.getError());
                    }
                    
                    if (genInput)
                    {
                        LuceneException finally;
                        bool fileConsistent = false;
                        try
                        {
                            int32_t version = genInput->readInt();
                            if (version == SegmentInfos::FORMAT_LOCKLESS)
                            {
                                int64_t gen0 = genInput->readLong();
                                int64_t gen1 = genInput->readLong();
                                segmentInfos->message(L"fallback check: " + StringUtils::toString(gen0) + L"; " + StringUtils::toString(gen1));
                                if (gen0 == gen1)
                                {
                                    // the file is consistent
                                    genB = gen0;
                                    fileConsistent = true;
                                }
                            }
                        }
                        catch (IOException&)
                        {
                            // will retry
                        }
                        catch (LuceneException& e)
                        {
                            finally = e;
                        }
                        genInput->close();
                        finally.throwException();
                        if (fileConsistent)
                            break;
                    }
                    
                    boost::this_thread::sleep(boost::posix_time::milliseconds(SegmentInfos::defaultGenFileRetryPauseMsec));
                }
                
                segmentInfos->message(String(IndexFileNames::SEGMENTS_GEN()) + L" check: genB=" + StringUtils::toString(genB));

                // pick the larger of the two gen's
                gen = std::max(genA, genB);
                
                // neither approach found a generation
                if (gen == -1)
                    boost::throw_exception(FileNotFoundException(L"No segments* file found in directory"));
            }
            
            // Third method (fallback if first & second methods are not reliable): since both directory cache and
            // file contents cache seem to be stale, just advance the generation.
            if (method == 1 || (method == 0 && lastGen == gen && retry))
            {
                method = 1;
                
                if (genLookaheadCount < SegmentInfos::defaultGenLookaheadCount)
                {
                    ++gen;
                    ++genLookaheadCount;
                    segmentInfos->message(L"look ahead increment gen to " + StringUtils::toString(gen));
                }
            }
            
            if (lastGen == gen)
            {
                // This means we're about to try the same segments_N last tried.  This is allowed, exactly once, because 
                // writer could have been in the process of writing segments_N last time.
                
                if (retry)
                {
                    // OK, we've tried the same segments_N file twice in a row, so this must be a real error.
                    exc.throwException();
                }
                else
                    retry = true;
            }
            else if (method == 0)
            {
                // Segment file has advanced since our last loop, so reset retry
                retry = false;
            }
            
            lastGen = gen;
            
            segmentFileName = IndexFileNames::fileNameFromGeneration(IndexFileNames::SEGMENTS(), L"", gen);
            
            try
            {
                runBody(segmentFileName);
                if (!exc.isNull())
                    segmentInfos->message(L"success on " + segmentFileName);
                return;
            }
            catch (LuceneException& err)
            {
                // Save the original root cause
                if (exc.isNull())
                    exc = err;
                
                segmentInfos->message(L"primary Exception on '" + segmentFileName + L"': " + err.getError() + L"'; will retry: retry=" + StringUtils::toString(retry) + L"; gen = " + StringUtils::toString(gen));
                
                if (!retry && gen > 1)
                {
                    // This is our first time trying this segments file (because retry is false), and, there is possibly a 
                    // segments_(N-1) (because gen > 1). So, check if the segments_(N-1) exists and try it if so.
                    String prevSegmentFileName(IndexFileNames::fileNameFromGeneration(IndexFileNames::SEGMENTS(), L"", gen - 1));
                    
                    if (directory->fileExists(prevSegmentFileName))
                    {
                        segmentInfos->message(L"fallback to prior segment file '" + prevSegmentFileName + L"'");
                        
                        try
                        {
                            runBody(prevSegmentFileName);
                            if (!exc.isNull())
                                segmentInfos->message(L"success on fallback " + prevSegmentFileName);
                            return;
                        }
                        catch (LuceneException& err2)
                        {
                            segmentInfos->message(L"secondary Exception on '" + prevSegmentFileName + L"': " + err2.getError() + L"'; will retry");
                        }
                    }
                }
            }
        }
    }
    
    FindSegmentsRead::FindSegmentsRead(SegmentInfosPtr infos, DirectoryPtr directory) : FindSegmentsFileT<int64_t>(infos, directory)
    {
        result = 0;
    }
    
    FindSegmentsRead::~FindSegmentsRead()
    {
    }
    
    int64_t FindSegmentsRead::doBody(const String& segmentFileName)
    {
        SegmentInfosPtr(_segmentInfos)->read(directory, segmentFileName);
        return 0;
    }
    
    SegmentInfosPtr SegmentInfos::range(int32_t first, int32_t last)
    {
        SegmentInfosPtr infos(newLucene<SegmentInfos>());
        infos->segmentInfos.addAll(segmentInfos.begin() + first, segmentInfos.begin() + last);
        return infos;
    }
    
    void SegmentInfos::updateGeneration(SegmentInfosPtr other)
    {
        lastGeneration = other->lastGeneration;
        generation = other->generation;
        version = other->version;
    }
    
    void SegmentInfos::rollbackCommit(DirectoryPtr dir)
    {
        if (pendingSegnOutput)
        {
            try 
            {
                pendingSegnOutput->close();
            }
            catch (...)
            {			
            }
            
            // must carefully compute filename from "generation" since lastgeneration isn't incremented
            try
            {
                String segmentFileName(IndexFileNames::fileNameFromGeneration(IndexFileNames::SEGMENTS(), L"", generation));
                dir->deleteFile(segmentFileName);
            }
            catch (...)
            {
            }			
            pendingSegnOutput.reset();
        }
    }
    
    void SegmentInfos::prepareCommit(DirectoryPtr dir)
    {
        TestScope testScope(L"SegmentInfos", L"prepareCommit");
        if (pendingSegnOutput)
            boost::throw_exception(IllegalStateException(L"prepareCommit was already called"));
        write(dir);
    }
    
    HashSet<String> SegmentInfos::files(DirectoryPtr dir, bool includeSegmentsFile)
    {
        HashSet<String> files(HashSet<String>::newInstance());
        if (includeSegmentsFile)
            files.add(getCurrentSegmentFileName());
        for (Collection<SegmentInfoPtr>::iterator seginfo = segmentInfos.begin(); seginfo != segmentInfos.end(); ++seginfo)
        {
            if ((*seginfo)->dir == dir)
            {
                HashSet<String> segFiles((*seginfo)->files());
                files.addAll(segFiles.begin(), segFiles.end());
            }
        }
        return files;
    }
    
    void SegmentInfos::finishCommit(DirectoryPtr dir)
    {
        if (!pendingSegnOutput)
            boost::throw_exception(IllegalStateException(L"prepareCommit was not called"));
        
        bool success = false;
        LuceneException finally;
        try
        {
            pendingSegnOutput->finishCommit();
            pendingSegnOutput->close();
            pendingSegnOutput.reset();
            success = true;
        }
        catch (LuceneException& e)
        {
            finally = e;
        }
        
        if (!success)
            rollbackCommit(dir);
        finally.throwException();
        
        // NOTE: if we crash here, we have left a segments_N file in the directory in a possibly corrupt state (if
        // some bytes made it to stable storage and others didn't).  But, the segments_N file includes checksum
        // at the end, which should catch this case.  So when a reader tries to read it, it will throw a 
        // CorruptIndexException, which should cause the retry logic in SegmentInfos to kick in and load the last
        // good (previous) segments_N-1 file.
        
        String fileName(IndexFileNames::fileNameFromGeneration(IndexFileNames::SEGMENTS(), L"", generation));
        
        success = false;
        try
        {
            dir->sync(fileName);
            success = true;
        }
        catch (...)
        {
        }
        
        if (!success)
            dir->deleteFile(fileName);
        
        lastGeneration = generation;
        IndexOutputPtr genOutput;
        try
        {
            genOutput = dir->createOutput(IndexFileNames::SEGMENTS_GEN());
            
            try
            {
                genOutput->writeInt(FORMAT_LOCKLESS);
                genOutput->writeLong(generation);
                genOutput->writeLong(generation);
            }
            catch (LuceneException& e)
            {
                finally = e;
            }
            
            genOutput->close();
            finally.throwException();
        }
        catch (...)
        {
        }		
    }
    
    void SegmentInfos::commit(DirectoryPtr dir)
    {
        prepareCommit(dir);
        finishCommit(dir);
    }
    
    String SegmentInfos::segString(DirectoryPtr directory)
    {
        SyncLock syncLock(this);
        String buffer;
        for (Collection<SegmentInfoPtr>::iterator seginfo = segmentInfos.begin(); seginfo != segmentInfos.end(); ++seginfo)
        {
            if (seginfo != segmentInfos.begin())
                buffer += L' ';
            buffer += (*seginfo)->segString(directory);
            if ((*seginfo)->dir != directory)
                buffer += L"**";
        }
        return buffer;
    }
    
    MapStringString SegmentInfos::getUserData()
    {
        return userData;
    }
    
    void SegmentInfos::setUserData(MapStringString data)
    {
        if (!data)
            userData = MapStringString::newInstance();
        else
            userData = data;
    }
    
    void SegmentInfos::replace(SegmentInfosPtr other)
    {
        segmentInfos.clear();
        segmentInfos.addAll(other->segmentInfos.begin(), other->segmentInfos.end());
        lastGeneration = other->lastGeneration;
    }
    
    bool SegmentInfos::hasExternalSegments(DirectoryPtr dir)
    {
        for (Collection<SegmentInfoPtr>::iterator seginfo = segmentInfos.begin(); seginfo != segmentInfos.end(); ++seginfo)
        {
            if ((*seginfo)->dir != dir)
                return true;
        }
        return false;
    }
}