/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DocHelper.h"
#include "Field.h"
#include "UnicodeUtils.h"
#include "SegmentInfo.h"
#include "WhitespaceAnalyzer.h"
#include "Similarity.h"
#include "Document.h"
#include "IndexWriter.h"
#include "StringUtils.h"
#include "MiscUtils.h"

namespace Lucene
{
    const wchar_t* DocHelper::FIELD_1_TEXT = L"field one text";
    const wchar_t* DocHelper::TEXT_FIELD_1_KEY = L"textField1";

    const wchar_t* DocHelper::FIELD_2_TEXT = L"field field field two text";
    // Fields will be lexicographically sorted.  So, the order is: field, text, two
    const int32_t DocHelper::FIELD_2_FREQS[] = {3, 1, 1};
    const wchar_t* DocHelper::TEXT_FIELD_2_KEY = L"textField2";

    const wchar_t* DocHelper::FIELD_3_TEXT = L"aaaNoNorms aaaNoNorms bbbNoNorms";
    const wchar_t* DocHelper::TEXT_FIELD_3_KEY = L"textField3";
    
    const wchar_t* DocHelper::KEYWORD_TEXT = L"Keyword";
    const wchar_t* DocHelper::KEYWORD_FIELD_KEY = L"keyField";
    
    const wchar_t* DocHelper::NO_NORMS_TEXT = L"omitNormsText";
    const wchar_t* DocHelper::NO_NORMS_KEY = L"omitNorms";
    
    const wchar_t* DocHelper::NO_TF_TEXT = L"analyzed with no tf and positions";
    const wchar_t* DocHelper::NO_TF_KEY = L"omitTermFreqAndPositions";
    
    const wchar_t* DocHelper::UNINDEXED_FIELD_TEXT = L"unindexed field text";
    const wchar_t* DocHelper::UNINDEXED_FIELD_KEY = L"unIndField";
    
    const wchar_t* DocHelper::UNSTORED_1_FIELD_TEXT = L"unstored field text";
    const wchar_t* DocHelper::UNSTORED_FIELD_1_KEY = L"unStoredField1";
    
    const wchar_t* DocHelper::UNSTORED_2_FIELD_TEXT = L"unstored field text";
    const wchar_t* DocHelper::UNSTORED_FIELD_2_KEY = L"unStoredField2";
    
    const wchar_t* DocHelper::LAZY_FIELD_BINARY_KEY = L"lazyFieldBinary";
    
    const wchar_t* DocHelper::LAZY_FIELD_KEY = L"lazyField";
    const wchar_t* DocHelper::LAZY_FIELD_TEXT = L"These are some field bytes";
    
    const wchar_t* DocHelper::LARGE_LAZY_FIELD_KEY = L"largeLazyField";
    String DocHelper::LARGE_LAZY_FIELD_TEXT;
    
    const wchar_t* DocHelper::FIELD_UTF1_TEXT = L"field one \u4e00text";
    const wchar_t* DocHelper::TEXT_FIELD_UTF1_KEY = L"textField1Utf8";
    
    const wchar_t* DocHelper::FIELD_UTF2_TEXT = L"field field field \u4e00two text";
    // Fields will be lexicographically sorted.  So, the order is: field, text, two
    const int32_t DocHelper::FIELD_UTF2_FREQS[] = {3, 1, 1};
    const wchar_t* DocHelper::TEXT_FIELD_UTF2_KEY = L"textField2Utf8";

    DocHelper::DocHelper()
    {
        textField1 = newLucene<Field>(TEXT_FIELD_1_KEY, FIELD_1_TEXT, Field::STORE_YES, Field::INDEX_ANALYZED, Field::TERM_VECTOR_NO);
        textField2 = newLucene<Field>(TEXT_FIELD_2_KEY, FIELD_2_TEXT, Field::STORE_YES, Field::INDEX_ANALYZED, Field::TERM_VECTOR_WITH_POSITIONS_OFFSETS);
        textField3 = newLucene<Field>(TEXT_FIELD_3_KEY, FIELD_3_TEXT, Field::STORE_YES, Field::INDEX_ANALYZED);
        textField3->setOmitNorms(true);
        keyField = newLucene<Field>(KEYWORD_FIELD_KEY, KEYWORD_TEXT, Field::STORE_YES, Field::INDEX_NOT_ANALYZED);
        noNormsField = newLucene<Field>(NO_NORMS_KEY, NO_NORMS_TEXT, Field::STORE_YES, Field::INDEX_NOT_ANALYZED_NO_NORMS);
        noTFField = newLucene<Field>(NO_TF_KEY, NO_TF_TEXT, Field::STORE_YES, Field::INDEX_ANALYZED);
        noTFField->setOmitTermFreqAndPositions(true);
        unIndField = newLucene<Field>(UNINDEXED_FIELD_KEY, UNINDEXED_FIELD_TEXT, Field::STORE_YES, Field::INDEX_NO);
        unStoredField1 = newLucene<Field>(UNSTORED_FIELD_1_KEY, UNSTORED_1_FIELD_TEXT, Field::STORE_NO, Field::INDEX_ANALYZED, Field::TERM_VECTOR_NO);
        unStoredField2 = newLucene<Field>(UNSTORED_FIELD_2_KEY, UNSTORED_2_FIELD_TEXT, Field::STORE_NO, Field::INDEX_ANALYZED, Field::TERM_VECTOR_YES);
        
        String binary(L"These are some binary field bytes");
        UTF8ResultPtr utf8 = newInstance<UTF8Result>();
        StringUtils::toUTF8((uint8_t*)binary.c_str(), binary.length(), utf8);
        LAZY_FIELD_BINARY_BYTES = ByteArray::newInstance(utf8->length);
        MiscUtils::arrayCopy(utf8->result.get(), 0, LAZY_FIELD_BINARY_BYTES.get(), 0, utf8->length);
        
        lazyFieldBinary = newLucene<Field>(LAZY_FIELD_BINARY_KEY, LAZY_FIELD_BINARY_BYTES, Field::STORE_YES);
        lazyField = newLucene<Field>(LAZY_FIELD_KEY, LAZY_FIELD_TEXT, Field::STORE_YES, Field::INDEX_ANALYZED);
        
        if (LARGE_LAZY_FIELD_TEXT.empty())
        {
            LARGE_LAZY_FIELD_TEXT.reserve(550000);
            for (int32_t i = 0; i < 10000; ++i)
                LARGE_LAZY_FIELD_TEXT += L"Lazily loading lengths of language in lieu of laughing ";
        }
        
        largeLazyField = newLucene<Field>(LARGE_LAZY_FIELD_KEY, LARGE_LAZY_FIELD_TEXT, Field::STORE_YES, Field::INDEX_ANALYZED);
        textUtfField1 = newLucene<Field>(TEXT_FIELD_UTF1_KEY, FIELD_UTF1_TEXT, Field::STORE_YES, Field::INDEX_ANALYZED, Field::TERM_VECTOR_NO);
        textUtfField2 = newLucene<Field>(TEXT_FIELD_UTF2_KEY, FIELD_UTF2_TEXT, Field::STORE_YES, Field::INDEX_ANALYZED, Field::TERM_VECTOR_WITH_POSITIONS_OFFSETS);
        
        nameValues = MapStringString::newInstance();
        nameValues.put(TEXT_FIELD_1_KEY, FIELD_1_TEXT);
        nameValues.put(TEXT_FIELD_2_KEY, FIELD_2_TEXT);
        nameValues.put(TEXT_FIELD_3_KEY, FIELD_3_TEXT);
        nameValues.put(KEYWORD_FIELD_KEY, KEYWORD_TEXT);
        nameValues.put(NO_NORMS_KEY, NO_NORMS_TEXT);
        nameValues.put(NO_TF_KEY, NO_TF_TEXT);
        nameValues.put(UNINDEXED_FIELD_KEY, UNINDEXED_FIELD_TEXT);
        nameValues.put(UNSTORED_FIELD_1_KEY, UNSTORED_1_FIELD_TEXT);
        nameValues.put(UNSTORED_FIELD_2_KEY, UNSTORED_2_FIELD_TEXT);
        nameValues.put(LAZY_FIELD_KEY, LAZY_FIELD_TEXT);
        nameValues.put(LAZY_FIELD_BINARY_KEY, L"");
        nameValues.put(LARGE_LAZY_FIELD_KEY, LARGE_LAZY_FIELD_TEXT);
        nameValues.put(TEXT_FIELD_UTF1_KEY, FIELD_UTF1_TEXT);
        nameValues.put(TEXT_FIELD_UTF2_KEY, FIELD_UTF2_TEXT);
        
        fields = Collection<FieldPtr>::newInstance();
        fields.add(textField1);
        fields.add(textField2);
        fields.add(textField3);
        fields.add(keyField);
        fields.add(noNormsField);
        fields.add(noTFField);
        fields.add(unIndField);
        fields.add(unStoredField1);
        fields.add(unStoredField2);
        fields.add(textUtfField1);
        fields.add(textUtfField2);
        fields.add(lazyField);
        fields.add(lazyFieldBinary);
        fields.add(largeLazyField);
        
        all = MapStringField::newInstance();
        indexed = MapStringField::newInstance();
        stored = MapStringField::newInstance();
        unstored = MapStringField::newInstance();
        unindexed = MapStringField::newInstance();
        termvector = MapStringField::newInstance();
        notermvector = MapStringField::newInstance();
        lazy = MapStringField::newInstance();
        noNorms = MapStringField::newInstance();
        noTf = MapStringField::newInstance();
        
        for (Collection<FieldPtr>::iterator field = fields.begin(); field != fields.end(); ++field)
        {
            all.put((*field)->name(), *field);
            if ((*field)->isIndexed())
                indexed.put((*field)->name(), *field);
            else
                unindexed.put((*field)->name(), *field);
            if ((*field)->isStored())
                stored.put((*field)->name(), *field);
            else
                unstored.put((*field)->name(), *field);
            if ((*field)->isTermVectorStored())
                termvector.put((*field)->name(), *field);
            if ((*field)->isIndexed() && !(*field)->isTermVectorStored())
                notermvector.put((*field)->name(), *field);
            if ((*field)->isLazy())
                lazy.put((*field)->name(), *field);
            if ((*field)->getOmitNorms())
                noNorms.put((*field)->name(), *field);
            if ((*field)->getOmitTermFreqAndPositions())
                noTf.put((*field)->name(), *field);
        }
    }
    
    DocHelper::~DocHelper()
    {
    }
    
    void DocHelper::setupDoc(DocumentPtr doc)
    {
        for (Collection<FieldPtr>::iterator field = fields.begin(); field != fields.end(); ++field)
            doc->add(*field);
    }
    
    SegmentInfoPtr DocHelper::writeDoc(DirectoryPtr dir, DocumentPtr doc)
    {
        return writeDoc(dir, newLucene<WhitespaceAnalyzer>(), Similarity::getDefault(), doc);
    }
    
    SegmentInfoPtr DocHelper::writeDoc(DirectoryPtr dir, AnalyzerPtr analyzer, SimilarityPtr similarity, DocumentPtr doc)
    {
        IndexWriterPtr writer = newLucene<IndexWriter>(dir, analyzer, IndexWriter::MaxFieldLengthLIMITED);
        writer->setSimilarity(similarity);
        writer->addDocument(doc);
        writer->commit();
        SegmentInfoPtr info = writer->newestSegment();
        writer->close();
        return info;
    }
    
    int32_t DocHelper::numFields(DocumentPtr doc)
    {
        return doc->getFields().size();
    }
}
