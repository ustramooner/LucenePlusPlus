/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDCACHEIMPL_H
#define FIELDCACHEIMPL_H

#include "FieldCache.h"

namespace Lucene
{
  typedef HashMap< EntryPtr, boost::any, luceneHash<EntryPtr>, luceneEquals<EntryPtr> > MapEntryAny;
  typedef WeakHashMap< LuceneObjectWeakPtr, MapEntryAny, luceneWeakHash<LuceneObjectWeakPtr>, luceneWeakEquals<LuceneObjectWeakPtr> > WeakMapLuceneObjectMapEntryAny;


	/// The default cache implementation, storing all values in memory.  A WeakHashMap is used for storage.
	class FieldCacheImpl : public FieldCache, public LuceneObject
	{
	public:
		FieldCacheImpl();
		virtual ~FieldCacheImpl();
	
		LUCENE_CLASS(FieldCacheImpl);
	
	protected:
		MapStringCache caches;
		InfoStreamPtr infoStream;
	
	public:
		virtual void initialize();
		virtual void purgeAllCaches();
		virtual void purge(IndexReaderPtr r);
		virtual Collection<FieldCacheEntryPtr> getCacheEntries();
		
		virtual Collection<uint8_t> getBytes(IndexReaderPtr reader, const String& field);
		virtual Collection<uint8_t> getBytes(IndexReaderPtr reader, const String& field, ByteParserPtr parser);
		
		virtual Collection<int32_t> getInts(IndexReaderPtr reader, const String& field);
		virtual Collection<int32_t> getInts(IndexReaderPtr reader, const String& field, IntParserPtr parser);
		
		virtual Collection<int64_t> getLongs(IndexReaderPtr reader, const String& field);
		virtual Collection<int64_t> getLongs(IndexReaderPtr reader, const String& field, LongParserPtr parser);
		
		virtual Collection<double> getDoubles(IndexReaderPtr reader, const String& field);
		virtual Collection<double> getDoubles(IndexReaderPtr reader, const String& field, DoubleParserPtr parser);
		
		virtual Collection<String> getStrings(IndexReaderPtr reader, const String& field);
		virtual StringIndexPtr getStringIndex(IndexReaderPtr reader, const String& field);
		
		virtual void setInfoStream(InfoStreamPtr stream);
		virtual InfoStreamPtr getInfoStream();
	};
	
	class Entry : public LuceneObject
	{
	public:
		/// Creates one of these objects for a custom comparator/parser.
		Entry(const String& field, boost::any custom);
		virtual ~Entry();
	
		LUCENE_CLASS(Entry);
	
	public:
		String field; // which Fieldable
		boost::any custom; // which custom comparator or parser
	
	public:
		/// Two of these are equal if they reference the same field and type.
		virtual bool equals(LuceneObjectPtr other);
		
		/// Composes a hashcode based on the field and type.
		virtual int32_t hashCode();
	};
	
	/// Internal cache.
	class Cache : public LuceneObject
	{
	public:
		Cache(FieldCachePtr wrapper = FieldCachePtr());
		virtual ~Cache();
		
		LUCENE_CLASS(Cache);
	
	public:
		FieldCacheWeakPtr _wrapper;
		WeakMapLuceneObjectMapEntryAny readerCache;
	
	protected:
		virtual boost::any createValue(IndexReaderPtr reader, EntryPtr key) = 0;
	
	public:
	    /// Remove this reader from the cache, if present.
	    virtual void purge(IndexReaderPtr r);
	    
      virtual boost::any get(IndexReaderPtr reader, EntryPtr key);
      virtual void printNewInsanity(InfoStreamPtr infoStream, boost::any value);
	};
	
	class ByteCache : public Cache
	{
	public:
		ByteCache(FieldCachePtr wrapper = FieldCachePtr());
		virtual ~ByteCache();
		
		LUCENE_CLASS(ByteCache);
	
	protected:
		virtual boost::any createValue(IndexReaderPtr reader, EntryPtr key);
	};
	
	class IntCache : public Cache
	{
	public:
		IntCache(FieldCachePtr wrapper = FieldCachePtr());
		virtual ~IntCache();
		
		LUCENE_CLASS(IntCache);
	
	protected:
		virtual boost::any createValue(IndexReaderPtr reader, EntryPtr key);
	};
	
	class LongCache : public Cache
	{
	public:
		LongCache(FieldCachePtr wrapper = FieldCachePtr());
		virtual ~LongCache();
		
		LUCENE_CLASS(LongCache);
	
	protected:
		virtual boost::any createValue(IndexReaderPtr reader, EntryPtr key);
	};
	
	class DoubleCache : public Cache
	{
	public:
		DoubleCache(FieldCachePtr wrapper = FieldCachePtr());
		virtual ~DoubleCache();
		
		LUCENE_CLASS(DoubleCache);
	
	protected:
		virtual boost::any createValue(IndexReaderPtr reader, EntryPtr key);
	};
	
	class StringCache : public Cache
	{
	public:
		StringCache(FieldCachePtr wrapper = FieldCachePtr());
		virtual ~StringCache();
		
		LUCENE_CLASS(StringCache);
	
	protected:
		virtual boost::any createValue(IndexReaderPtr reader, EntryPtr key);
	};
	
	class StringIndexCache : public Cache
	{
	public:
		StringIndexCache(FieldCachePtr wrapper = FieldCachePtr());
		virtual ~StringIndexCache();
		
		LUCENE_CLASS(StringIndexCache);
	
	protected:
		virtual boost::any createValue(IndexReaderPtr reader, EntryPtr key);
	};
	
	class FieldCacheEntryImpl : public FieldCacheEntry
	{
	public:
        FieldCacheEntryImpl(LuceneObjectPtr readerKey, const String& fieldName, int32_t cacheType, boost::any custom, boost::any value);
		virtual ~FieldCacheEntryImpl();
		
		LUCENE_CLASS(FieldCacheEntryImpl);
	
	protected:
		LuceneObjectPtr readerKey;
		String fieldName;
        int32_t cacheType;
		boost::any custom;
		boost::any value;
	
	public:
		virtual LuceneObjectPtr getReaderKey();
		virtual String getFieldName();
        virtual int32_t getCacheType();
		virtual boost::any getCustom();
		virtual boost::any getValue();
	};
}

#endif
