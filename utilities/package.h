#ifndef _PACKAGE_H_GUARD_
#define _PACKAGE_H_GUARD_
// 1472 bytes
// 1455 bytes for data maximum

#include <cstdint>
#include <cstddef>

const int DATA_SIZE = 1455;

struct Package
{
	uint32_t seq_number;	// номер пакета
	uint32_t seq_total;		// количество пакетов с данными
	uint8_t type;			// тип пакета: 0 == ACK, 1 == PUT
	char id[8];				// 8 байт - идентификатор, отличающий один файл от другого
	char data[DATA_SIZE];				// после заголовка и до конца UDP пакета идут данные
} __attribute__ ((__packed__));

const int MAXLINE = sizeof(Package);
const int HEADER_SIZE = MAXLINE - DATA_SIZE;

#endif
	// _PACKAGE_H_GUARD_
