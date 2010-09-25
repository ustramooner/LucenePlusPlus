/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "test_lucene.h"
#include "Lock.h"

namespace Lucene
{
	class MockLock : public Lock
	{
	public:
		MockLock();
		virtual ~MockLock();
		
		LUCENE_CLASS(MockLock);
	
	public:
		int32_t lockAttempts;
	
	public:
		virtual bool obtain();
		virtual void release();
		virtual bool isLocked();
		virtual String toString();
	};
}
