/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LuceneSync.h"

#ifdef USE_TR1
	#include <tr1/unordered_map>
#else
	#include <boost/unordered_map.hpp>
#endif

namespace Lucene
{
    /// Utility template class to handle hash maps that can be safely copied and shared
	template < class KEY, class VALUE, class HASH = USE_TR1_PREFIX::hash<KEY>, class EQUAL = std::equal_to<KEY> >
	class HashMap : public LuceneSync
	{
	public:
		typedef HashMap<KEY, VALUE, HASH, EQUAL> this_type;
		typedef std::pair<KEY, VALUE> key_value;
		typedef USE_TR1_PREFIX::unordered_map< KEY, VALUE, HASH, EQUAL, Allocator<key_value> > map_type;
		typedef typename map_type::iterator iterator;
		typedef typename map_type::const_iterator const_iterator;
	
		virtual ~HashMap()
		{
		}
	
	protected:
		boost::shared_ptr<map_type> mapContainer;
		
	public:
		static this_type newInstance()
		{
			this_type instance;
			instance.mapContainer = Lucene::newInstance<map_type>();
			return instance;
		}
		
		void reset()
		{
			mapContainer.reset();
		}
		
		int32_t size() const
		{
			return mapContainer->size();
		}
		
		bool empty() const
		{
			return mapContainer->empty();
		}
		
		void clear()
		{
			mapContainer->clear();
		}
		
		iterator begin()
		{
			return mapContainer->begin();
		}
		
		iterator end()
		{
			return mapContainer->end();
		}
		
		const_iterator begin() const
		{
			return mapContainer->begin();
		}
		
		const_iterator end() const
		{
			return mapContainer->end();
		}
		
		operator bool() const
		{
			return mapContainer;
		}
		
		bool operator! () const
		{
			return !mapContainer;
		}
		
		map_type& operator= (const map_type& other)
		{
			mapContainer = other.mapContainer;
			return *this;
		}
		
		void put(const KEY& key, const VALUE& value)
		{
			(*mapContainer)[key] = value;
		}
		
		template <class ITER>
		void putAll(ITER first, ITER last)
		{
			for (iterator current = first; current != last; ++current)
				(*mapContainer)[current->first] = current->second;
		}
		
		template <class ITER>
		void remove(ITER pos)
		{
			mapContainer->erase(pos);
		}
		
		template <class ITER>
		ITER remove(ITER first, ITER last)
		{
			return mapContainer->erase(first, last);
		}
		
		bool remove(const KEY& key)
		{
			return (mapContainer->erase(key) > 0);
		}
		
		iterator find(const KEY& key)
		{
			return mapContainer->find(key);
		}
		
		VALUE get(const KEY& key) const
		{
			iterator findValue = mapContainer->find(key);
			return findValue == mapContainer->end() ? VALUE() : findValue->second;
		}
		
		bool contains(const KEY& key) const
		{
			return (mapContainer->find(key) != mapContainer->end());
		}
		
		VALUE& operator[] (const KEY& key)
		{
			return (*mapContainer)[key];
		}
    };
    
    /// Utility template class to handle weak keyed maps
    template < class KEY, class VALUE, class HASH = USE_TR1_PREFIX::hash<KEY>, class EQUAL = std::equal_to<KEY> >
	class WeakHashMap : public HashMap<KEY, VALUE, HASH, EQUAL>
	{
	public:
		typedef WeakHashMap<KEY, VALUE, HASH, EQUAL> this_type;
		typedef std::pair<KEY, VALUE> key_value;
		typedef typename USE_TR1_PREFIX::unordered_map< KEY, VALUE, HASH, EQUAL, Allocator<key_value> > map_type;
		typedef typename map_type::iterator iterator;
		
		static this_type newInstance()
		{
			this_type instance;
			instance.mapContainer = Lucene::newInstance<map_type>();
			return instance;
		}
		
		void removeWeak()
		{
			if (!this->mapContainer || this->mapContainer->empty())
				return;
			map_type clearCopy;
			for (iterator key = this->mapContainer->begin(); key != this->mapContainer->end(); ++key)
			{
				if (!key->first.expired())
					clearCopy.insert(*key);
			}
			this->mapContainer->swap(clearCopy);
		}
		
		VALUE get(const KEY& key)
		{
			iterator findValue = this->mapContainer->find(key);
			if (findValue != this->mapContainer->end())
				return findValue->second;
			removeWeak();
			return VALUE();
		}
    };
}
