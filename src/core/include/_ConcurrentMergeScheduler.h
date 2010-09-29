#pragma once
#include "ConcurrentMergeScheduler.h"

namespace Lucene {

	class MergeThread : public LuceneThread
	{
	public:
		MergeThread(ConcurrentMergeSchedulerPtr merger, IndexWriterPtr writer, OneMergePtr startMerge);
		virtual ~MergeThread();
		
		LUCENE_CLASS(MergeThread);
		
	protected:
		ConcurrentMergeSchedulerWeakPtr _merger;
		IndexWriterWeakPtr _writer;
		OneMergePtr startMerge;
		OneMergePtr runningMerge;
	
	public:
		void setRunningMerge(OneMergePtr merge);
		OneMergePtr getRunningMerge();
		void setThreadPriority(int32_t pri);
		virtual void run();
	};
}

