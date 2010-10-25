/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Lucene
{
	/// Utility class to support signaling notifications.
	class LPPAPI LuceneSignal
	{
		class Internal;
		Internal* intern;
	public:
		LuceneSignal(SynchronizePtr objectLock = SynchronizePtr());
		virtual ~LuceneSignal();

	    /// create a new LuceneSignal instance atomically.
	    static void createSignal(LuceneSignalPtr& signal, SynchronizePtr objectLock);
	    
		/// Wait for signal using an optional timeout.
		void wait(int32_t timeout = 0);
		
		/// Notify all threads waiting for signal.
		void notifyAll();
	};
}

