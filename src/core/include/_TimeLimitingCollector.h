#pragma once
#include "TimeLimitingCollector.h"

namespace Lucene
{

	class TimerThread : public LuceneThread
	{
	public:
		TimerThread();
		virtual ~TimerThread();
		
		LUCENE_CLASS(TimerThread);
		
	protected:
		int64_t time;
		bool _stopThread;
	
	public:
		virtual void start();
		virtual void run();
		
		/// Get the timer value in milliseconds.
		int64_t getMilliseconds();
		
		/// Stop timer thread.
		void stopThread();
	};
}

