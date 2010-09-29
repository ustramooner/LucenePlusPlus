/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include "Synchronize.h"
#include "LuceneThread.h"

namespace Lucene
{
    class Synchronize::RecursiveMutexContainer {
    public:
        boost::recursive_timed_mutex mutexSynchronize;
        ThreadId lockThread;
    };

    Synchronize::Synchronize()
    {
        recursiveMutexContainer = new RecursiveMutexContainer;
        recursiveMutexContainer->lockThread = LuceneThread::nullId();
        recursionCount = 0;
    }
    
    Synchronize::~Synchronize()
    {
        delete recursiveMutexContainer;
    }
    
    void Synchronize::lock(int32_t timeout)
    {
        if (timeout > 0)
            recursiveMutexContainer->mutexSynchronize.timed_lock(boost::posix_time::milliseconds(timeout));
        else
            recursiveMutexContainer->mutexSynchronize.lock();
        recursiveMutexContainer->lockThread = LuceneThread::currentId();
        ++recursionCount;
    }
    
    void Synchronize::unlock()
    {
        if (--recursionCount == 0)
            recursiveMutexContainer->lockThread = LuceneThread::nullId();
        recursiveMutexContainer->mutexSynchronize.unlock();        
    }
    
    int32_t Synchronize::unlockAll()
    {
        int32_t count = recursionCount;
        for (int32_t unlock = 0; unlock < count; ++unlock)
            this->unlock();
        return count;
    }
    
    bool Synchronize::holdsLock()
    {
        return (recursiveMutexContainer->lockThread == LuceneThread::currentId() && recursionCount > 0);
    }
    
    SyncLock::SyncLock(SynchronizePtr sync, int32_t timeout)
    {
        this->sync = sync;
        lock(timeout);
    }

    SyncLock::~SyncLock()
    {
        if (this->sync)
            this->sync->unlock();
    }
    
    void SyncLock::lock(int32_t timeout)
    {
        if (this->sync)
            this->sync->lock(timeout);
    }
}
