#pragma once
#include "NoLockFactory.h"

namespace Lucene
{

	class NoLock : public Lock
	{
	public:
		virtual ~NoLock();
		
		LUCENE_CLASS(NoLock);
			
	public:
		virtual bool obtain();
		virtual void release();
		virtual bool isLocked();
		virtual String toString();
	};

}

