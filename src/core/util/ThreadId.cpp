#include "stdafx.h"
#include "ThreadId.h"
#include <boost/thread/thread.hpp>
namespace Lucene
{
	class ThreadId::Internal {
	public:
		bool initialised;
	#if defined(_WIN32) || defined(_WIN64)
		int64_t id;
	#else
		boost::thread::id id;
	#endif

	};

	ThreadId::ThreadId(){
		internal = new Internal;
		internal->initialised = false;
	}
	ThreadId::ThreadId(const ThreadId& other){
		internal = new Internal;
		internal->id = other.internal->id;
		internal->initialised = other.internal->initialised;
	}

	ThreadId::~ThreadId(){
		delete internal;
	}


	ThreadId::operator bool() const
	{
		return internal->initialised;
	}

	bool ThreadId::operator! () const
	{
		return !internal->initialised;
	}

	ThreadId& ThreadId::operator= (const ThreadId& other)
	{
		internal->id = other.internal->id;
		internal->initialised = other.internal->initialised;
		return *this;
	}
	bool ThreadId::operator== (const ThreadId& other) const
	{
	  return (!internal->initialised && !other.internal->initialised) || internal->id == other.internal->id;
	}

	bool ThreadId::operator!= (const ThreadId& other) const
	{
	  return !((!internal->initialised && !other.internal->initialised) || internal->id == other.internal->id );
	}

	ThreadId ThreadId::currentId(){
		ThreadId ret;
		#ifdef _WIN32
		ret.internal->id = ::GetCurrentThreadId();
		#else
		ret.internal->id = boost::this_thread::get_id();
		#endif
		ret.internal->initialised = true;
		return ret;
	}
  ThreadId ThreadId::nullId()
  {
    ThreadId ret;
		ret.internal->initialised = false;
		return ret;
  }
}

