#pragma once
#ifndef LUCENE_PCH_H
#define LUCENE_PCH_H

#include "Lucene.h"
#include "LuceneHeaders.h"
#include <limits.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/crc.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/function.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/any.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>

#endif

