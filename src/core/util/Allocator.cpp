/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef LPP_USE_NEDMALLOC
extern "C"
{
#include "nedmalloc/nedmalloc.h"
}
#endif

namespace Lucene
{
    void* AllocMemory(size_t size)
    {
        #if defined(LPP_USE_NEDMALLOC)
        return nedalloc::nedmalloc(size);
        #elif (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG) 
        return _malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__);
        #else
        return malloc(size);
        #endif
    }
    
    void* ReallocMemory(void* memory, size_t size)
    {
        if (memory == NULL)
            return AllocMemory(size);
        if (size == 0)
        {
            FreeMemory(memory);
            return NULL;
        }
        #if defined(LPP_USE_NEDMALLOC)
        return nedalloc::nedrealloc(memory, size);
        #elif defined(_WIN32) && defined(_DEBUG)
        return _realloc_dbg(memory, size, _NORMAL_BLOCK, __FILE__, __LINE__);
        #else
        return realloc(memory, size);
        #endif
    }
    
    void FreeMemory(void* memory)
    {
        if (memory == NULL)
            return;
        #if defined(LPP_USE_NEDMALLOC)
        nedalloc::nedfree(memory);
        #elif defined(_WIN32) && defined(_DEBUG)
        _free_dbg(memory, _NORMAL_BLOCK);
        #else
        free(memory);
        #endif
    }
    
    void ReleaseThreadCache()
    {
        #if defined(LPP_USE_NEDMALLOC)
        nedalloc::neddisablethreadcache(0);
        #endif
    }
}
