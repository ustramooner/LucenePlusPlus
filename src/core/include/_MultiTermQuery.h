#pragma once
#include "MultiTermQuery.h"

namespace Lucene
{

	class ConstantScoreFilterRewrite : public RewriteMethod
	{
	public:
		virtual ~ConstantScoreFilterRewrite();
		LUCENE_CLASS(ConstantScoreFilterRewrite);
	
	public:
		virtual QueryPtr rewrite(IndexReaderPtr reader, MultiTermQueryPtr query);
	};
	
	class ScoringBooleanQueryRewrite : public RewriteMethod
	{
	public:
		virtual ~ScoringBooleanQueryRewrite();
		LUCENE_CLASS(ScoringBooleanQueryRewrite);
	
	public:
		virtual QueryPtr rewrite(IndexReaderPtr reader, MultiTermQueryPtr query);
	};
	
	class ConstantScoreBooleanQueryRewrite : public ScoringBooleanQueryRewrite
	{
	public:
		virtual ~ConstantScoreBooleanQueryRewrite();
		LUCENE_CLASS(ConstantScoreBooleanQueryRewrite);
	
	public:
		virtual QueryPtr rewrite(IndexReaderPtr reader, MultiTermQueryPtr query);
	};
	
	class ConstantScoreAutoRewriteDefault : public ConstantScoreAutoRewrite
	{
	public:
		virtual ~ConstantScoreAutoRewriteDefault();
		LUCENE_CLASS(ConstantScoreAutoRewriteDefault);
	
	public:
		virtual void setTermCountCutoff(int32_t count);
		virtual void setDocCountPercent(double percent);
	};
}

