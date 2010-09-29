#pragma once
#include "SnapshotDeletionPolicy.h"

namespace Lucene {

	class MyCommitPoint : public IndexCommit
	{
	public:
		MyCommitPoint(SnapshotDeletionPolicyPtr deletionPolicy, IndexCommitPtr cp);
		virtual ~MyCommitPoint();
		
		LUCENE_CLASS(MyCommitPoint);
			
	protected:
		SnapshotDeletionPolicyWeakPtr _deletionPolicy;
	
	public:
		IndexCommitPtr cp;
	
	public:
		/// Get the segments file (segments_N) associated with this commit point.
		virtual String getSegmentsFileName();
		
		/// Returns all index files referenced by this commit point.
		virtual HashSet<String> getFileNames();
		
		/// Returns the {@link Directory} for the index.
		virtual DirectoryPtr getDirectory();
		
		/// Delete this commit point.
		virtual void deleteCommit();
		
		virtual bool isDeleted();
		
		/// Returns the version for this IndexCommit.
		virtual int64_t getVersion();
		
		/// Returns the generation (the _N in segments_N) for this IndexCommit.
		virtual int64_t getGeneration();
		
		/// Returns userData, previously passed to {@link IndexWriter#commit(Map)} for this commit.
		virtual MapStringString getUserData();
		
		virtual bool isOptimized();
	};
}

