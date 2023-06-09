// Server side implementation of UDP client-server model
#include <bits/stdc++.h>
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
#include "../utilities/ChunkedFiles.h"

using namespace std;

// Driver code
int main() {
	int sockfd;

	struct sockaddr_in servaddr, cliaddr;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if(bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	socklen_t len = sizeof(cliaddr); //len is value/result
	ChunkedFiles chfls;

	while(true)
	{
		Package pkg;
		int n = recvfrom(sockfd, reinterpret_cast<char*>(&pkg), MAXLINE,
				MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);

		// cout << "recvfrom = " << n << endl;

		int chunkes_received = chfls.write2file(pkg, n);
		if(chunkes_received == -1) // err
		{
			cout << "Error on saving chunk in file" << endl;
			break;
		}

		// form regular package
		Package ack;
		ack.seq_number = pkg.seq_number;
		ack.seq_total = chunkes_received;
		ack.type = 0;
		memcpy(ack.id, pkg.id, 8);

		// add crc on final package
		if( chunkes_received == pkg.seq_total )
		{
			std::string fname = pkg.id;
			uint32_t crc = chfls.calcCrc( fname );
			// send back crc to client
			memcpy( ack.data, &crc, 4 );
			// just for debug save file to fs
			chfls.flushFile(fname);
		}

		sendto(sockfd, (const char *)&ack, MAXLINE - DATA_SIZE + 4,
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
	}

	close(sockfd);
	cout << "Close" << endl;

	return 0;
}
