/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MMAPDIRECTORY_H
#define MMAPDIRECTORY_H

#include "FSDirectory.h"
#include "IndexInput.h"
#include <boost/iostreams/device/mapped_file.hpp>

namespace Lucene
{
	/// File-based {@link Directory} implementation that uses mmap for reading, and {@link SimpleFSIndexOutput} for writing.
	///
	/// NOTE: memory mapping uses up a portion of the virtual memory address space in your process equal to the size of the 
	/// file being mapped.  Before using this class, be sure your have plenty of virtual address space.
	class LPPAPI MMapDirectory : public FSDirectory
	{
	public:
		/// Create a new MMapDirectory for the named location.
		/// @param path the path of the directory.
		/// @param lockFactory the lock factory to use, or null for the default ({@link NativeFSLockFactory})
		MMapDirectory(const String& path, LockFactoryPtr lockFactory = LockFactoryPtr());
		
		virtual ~MMapDirectory();
		
		LUCENE_CLASS(MMapDirectory);
			
	public:
		using FSDirectory::openInput;
		
		/// Creates an IndexInput for the file with the given name.
		virtual IndexInputPtr openInput(const String& name, int32_t bufferSize);
		
		/// Creates an IndexOutput for the file with the given name.
		virtual IndexOutputPtr createOutput(const String& name);
	};
	
}

#endif
