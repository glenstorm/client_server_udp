#ifndef _PUZZLE_FILE_H_GUARD_
#define _PUZZLE_FILE_H_GUARD_

#include "crc.h"

class PuzzleFile
{
public:
	PuzzleFile()
		:chunkCount_(0)
		,fileSizeInChunks_(0)
		,crc_(0){}

	void setChunkCount( size_t fileSizeInChunks )
	{
		fileSizeInChunks_ = fileSizeInChunks;
		bits_.reserve(fileSizeInChunks_ * DATA_SIZE);
		bits_.resize((fileSizeInChunks_ - 1) * DATA_SIZE);
	}

	int writeData(char *data, size_t data_size, size_t start_pos)
	{
		if( data_size != DATA_SIZE )
			std::copy( data, data + data_size, std::back_inserter(bits_) );
		else
			std::copy( data, data + data_size, &bits_[start_pos * DATA_SIZE] );

		crc_ = crc32c(crc_, reinterpret_cast<const unsigned char*>(data), data_size);

		++chunkCount_;
		return chunkCount_;
	}

	bool isNew() const
	{
		return bits_.capacity() == 0;
	}

	const char* data() const
	{
		return bits_.data();
	}

	size_t size() const
	{
		return bits_.size();
	}

	uint32_t getCrc() const
	{
		return crc_;
	}

private:
	std::vector<char> bits_;

	uint32_t crc_;

	size_t fileSizeInChunks_;   // max size in chunks
	size_t chunkCount_;         // current size in chunks
};


#endif
