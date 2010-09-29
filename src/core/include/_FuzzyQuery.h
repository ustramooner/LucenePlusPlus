#pragma once
#include "FuzzyQuery.h"

namespace Lucene
{

	class ScoreTerm : public LuceneObject
	{
	public:
		virtual ~ScoreTerm();
		LUCENE_CLASS(ScoreTerm);
	
	public:
		TermPtr term;
		double score;
	
	public:
		int32_t compareTo(ScoreTermPtr other);
	};
	
	class ScoreTermQueue : public PriorityQueue<ScoreTermPtr>
	{
	public:
		ScoreTermQueue(int32_t size);
		virtual ~ScoreTermQueue();
		
		LUCENE_CLASS(ScoreTermQueue);
	
	protected:
		virtual bool lessThan(const ScoreTermPtr& first, const ScoreTermPtr& second);
	};
}

