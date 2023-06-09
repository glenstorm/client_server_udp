#ifndef _CHUNKED_FILES_H_GUARD_
#define _CHUNKED_FILES_H_GUARD_

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

#include "PuzzleFile.h"
#include "../utilities/package.h"
#include "../utilities/crc.h"

class ChunkedFiles
{
public:
	// -1 for error
	// n for already received packages
	int write2file(Package& pkg, size_t package_size)
	{
		size_t data_size = package_size - HEADER_SIZE;
		// std::cout << "data_size = " << data_size << std::endl;
		pkg.data[data_size] = 0;
		// std::cout << pkg.data << std::endl;

		std::string fname = pkg.id;

		PuzzleFile &byte_vector = inputFiles_[fname];
		if( byte_vector.isNew() )
			byte_vector.setChunkCount( pkg.seq_total );

		int res = byte_vector.writeData( pkg.data, data_size, pkg.seq_number );

		return res;
	}

	void flushFile( const std::string& fname )
	{
		auto res = inputFiles_.find(fname);
		if( res != inputFiles_.end())
		{
			std::ofstream os( fname );
			os.write(res->second.data(), res->second.size());
			os.close();
		}
	}

	uint32_t calcCrc( const std::string& fname ) const
	{
		auto res = inputFiles_.find(fname);
		if( res != inputFiles_.end())
		{
			return res->second.getCrc();
		}

		return -1;
	}

private:
	std::map<std::string, PuzzleFile> inputFiles_;
};

#endif
	// _CHUNKED_FILE_H_GUARD_
