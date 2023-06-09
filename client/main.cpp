#include <bits/stdc++.h>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../utilities/crc.h"
#include "../utilities/package.h"
#include "../utilities/settings.h"

using namespace std;

Package form_package(const string& fname, ifstream &mfile, uint32_t chunk_size, uint32_t num_blocks, uint32_t order, uint32_t& crc)
{
	Package pkg;

	mfile.seekg(order * DATA_SIZE);
	mfile.read(pkg.data, chunk_size);
	crc = crc32c(crc, reinterpret_cast<const unsigned char*>(pkg.data), chunk_size );

	pkg.seq_number = order;
	pkg.seq_total  = num_blocks;
	pkg.type       = 1;
	memcpy(&pkg.id[0], fname.c_str(), sizeof(pkg.id));

	return pkg;
}


void send_package_impl(int sockfd, const Package &pkg, uint32_t chunk_size, const sockaddr_in &servaddr, const uint32_t crc)
{
	socklen_t len;
	size_t real_struct_size = chunk_size + HEADER_SIZE;

	// cout << "real_struct_size = " << real_struct_size << endl;
	// cout << "pkg.data = " << pkg.data << endl;
	// trying resent while is not reseived on server side
	while(sendto(sockfd, (const char *)&pkg, real_struct_size, MSG_CONFIRM, (const struct sockaddr*) &servaddr, sizeof(servaddr)))
	{
 		// receive
		fd_set readfds;            //fd_set is a type
		FD_ZERO(&readfds);         //initialize
		FD_SET(sockfd, &readfds);  //put the socket in the set

		struct timeval timeout;
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;

		int ret = select(sockfd + 1, &readfds, NULL, NULL, &timeout); // Если клиент на какой-то отправленный пакет не получает ACK в течение таймаута, то отправленный пакет переотправляется.
		if(ret > 0)
		{
			// maybe ack maybe checksum
			Package ack_pkg;
			ssize_t package_size = recvfrom(sockfd, reinterpret_cast<char*>(&ack_pkg), MAXLINE - DATA_SIZE + 4, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
			if(!package_size)
				cout << "error reading" << endl;

			if( pkg.seq_total == ack_pkg.seq_total )
			{
				uint32_t server_crc;
				memcpy( &server_crc, ack_pkg.data, 4 );
				if( crc == server_crc )
				{
					cout << "client crc for file " << ack_pkg.id << " = " << crc << " == " << server_crc << endl;
				}
				else
				{
					cout << "client crc for file " << ack_pkg.id << " = " << crc << " != " << server_crc << endl;
				}
			}
		}
		else if (ret == 0)
		{
			cout << "timed out waiting for ack" << endl;
			continue; // resend the same packet again
		}
		else
		{
			cout << "error selecting" << endl;
		}
		break;
	}
}

void send_package(const string& fname, ifstream &mfile, uint32_t chunk_size, int sockfd, const sockaddr_in &servaddr, uint32_t num_blocks, uint32_t order, uint32_t& crc )
{
	Package pkg = form_package(fname, mfile, chunk_size, num_blocks, order, crc );
	send_package_impl(sockfd, pkg, chunk_size, servaddr, crc);
}

int main(int argv, char** argc)
{
	string fname = argc[1];

	int sockfd;
	ifstream mfile(fname, ios::binary|ios::in|ios::ate);

	cout << "open file " << fname << endl;

	if( mfile.is_open())
	{
		size_t file_size = mfile.tellg();
		cout << "file_size = " << file_size << endl;

		size_t num_blocks = file_size/DATA_SIZE + (file_size % DATA_SIZE ?1 :0);
		std::vector<uint32_t> block_order(num_blocks) ; // vector with 100 ints.
		std::iota(std::begin(block_order), std::end(block_order), 0); // Fill with 0, 1, ..., 99.

		std::random_device rd;
		std::mt19937 g(rd());

		std::shuffle(block_order.begin(), block_order.end(), g);
		// std::copy(block_order.begin(), block_order.end(), std::ostream_iterator<int>(std::cout, " "));
		// std::cout << '\n';

		struct sockaddr_in servaddr;
		// Creating socket file descriptor
		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			perror("socket creation failed");
			exit(EXIT_FAILURE);
		}

		memset(&servaddr, 0, sizeof(servaddr));
		// Filling server information
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(PORT);
		servaddr.sin_addr.s_addr = INADDR_ANY;

		// cacl also crc
		uint32_t crc = 0;
		for_each(block_order.begin(), block_order.end(), [&](int order)
		{
			uint32_t chunk_size = DATA_SIZE;
			if(num_blocks == (order + 1))
				chunk_size = file_size % DATA_SIZE;

			send_package(fname, mfile, chunk_size, sockfd, servaddr, num_blocks, order, crc);
		});

		close(sockfd);
	}
	return 0;
}
