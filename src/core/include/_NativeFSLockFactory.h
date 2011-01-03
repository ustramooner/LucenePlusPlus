/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "NativeFSLockFactory.h"

namespace Lucene
{
	class NativeFSLock : public Lock
	{
	public:
		NativeFSLock(const String& lockDir, const String& lockFileName);
		virtual ~NativeFSLock();
		
		LUCENE_CLASS(NativeFSLock);
				
	protected:
		String lockDir;
		String path;
		filelockPtr lock;
		
		static SynchronizePtr LOCK_HELD_LOCK();
		static HashSet<String> LOCK_HELD();
	
	public:
		virtual bool obtain();
		virtual void release();
		virtual bool isLocked();
		virtual String toString();
	
	protected:
		bool lockExists();
	};
}

