#pragma once
#include "SimpleFSLockFactory.h"

namespace Lucene
{

	class SimpleFSLock : public Lock
	{
	public:
		SimpleFSLock(const String& lockDir, const String& lockFileName);
		virtual ~SimpleFSLock();
		
		LUCENE_CLASS(SimpleFSLock);
			
	public:
		String lockDir;
		String lockFile;
	
	public:
		/// Attempts to obtain exclusive access and immediately return upon success or failure.
		/// @return true if exclusive access is obtained.
		virtual bool obtain();
		
		/// Releases exclusive access.
		virtual void release();
		
		/// Returns true if the resource is currently locked. Note that one must still call {@link #obtain()} 
		/// before using the resource.
		virtual bool isLocked();
		
		/// Returns derived object name.
		virtual String toString();
	};
}

