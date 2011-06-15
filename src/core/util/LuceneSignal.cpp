/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LuceneSignal.h"
#include "Synchronize.h"
#include <boost/thread/condition.hpp>

namespace Lucene
{
    class LuceneSignal::Internal {
    public:
      boost::mutex waitMutex;
      boost::condition signalCondition;
      SynchronizePtr objectLock;
    };

    LuceneSignal::LuceneSignal(SynchronizePtr objectLock)
    {
        this->intern = new Internal();
        this->intern->objectLock = objectLock;
    }
    
    LuceneSignal::~LuceneSignal()
    {
        delete intern;
    }
    
    void LuceneSignal::createSignal(LuceneSignalPtr& signal, SynchronizePtr objectLock)
    {
        static boost::mutex lockMutex;
        boost::mutex::scoped_lock syncLock(lockMutex);
        if (!signal)
            signal = newInstance<LuceneSignal>(objectLock);
    }
    
    void LuceneSignal::wait(int32_t timeout)
    {
        int32_t relockCount = intern->objectLock ? intern->objectLock->unlockAll() : 0;
        boost::mutex::scoped_lock waitLock(intern->waitMutex);
        while (!intern->signalCondition.timed_wait(intern->waitMutex, boost::posix_time::milliseconds(timeout)))
        {
            if (timeout != 0 || intern->signalCondition.timed_wait(intern->waitMutex, boost::posix_time::milliseconds(10)))
                break;
        }
        for (int32_t relock = 0; relock < relockCount; ++relock)
            intern->objectLock->lock();
    }
    
    void LuceneSignal::notifyAll()
    {
        intern->signalCondition.notify_all();
    }
}
