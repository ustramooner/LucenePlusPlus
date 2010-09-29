#pragma once

namespace Lucene
{
	//holder for a thread id
	class ThreadId {
		class Internal;
		Internal* internal;
	public:
		ThreadId();
		ThreadId(const ThreadId& other);
		~ThreadId();

		operator bool() const;
		bool operator! () const;
		ThreadId& operator= (const ThreadId& other);
		bool operator== (const ThreadId& other) const;
		bool operator!= (const ThreadId& other) const;

		static ThreadId currentId();
		static ThreadId nullId();
	};
}


