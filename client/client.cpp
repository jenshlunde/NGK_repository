//============================================================================
// Description : file_client in C++, Ansi-style
// Modified by Jens and Khaleed
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "iknlib.h"

using namespace std;

#define PORT 9000
#define BUF_SIZE 1000

//Prototype: Error message function for 'cleaner' code
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

//Prototype: Receive file function 
void receiveFile(char* _filePath, int _sockfd);

int main(int argc, char *argv[])
{
	//Client socket setup
	printf("Starting client...\n");

	int sockfd;
	char filePath[250];
	struct sockaddr_in serv_addr;
	struct hostent *server;

	//Terminal print for wrong arguments
	if (argc < 3)
	    error("ERROR usage: <ip> <file path>");

	//Terminal print for wrong ip
	server = gethostbyname(argv[1]);
	if (server == NULL) 
	    error("ERROR no such host");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	    error("ERROR opening socket");

	printf("Connecting...\n");
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr,(char*)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(PORT);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	    error("ERROR connecting");
	printf("Connection established...\n\n");
	
	snprintf(filePath, sizeof(filePath), "%s", argv[2]);
	// Write the filepath to the server
	write(sockfd, filePath, sizeof(filePath));
	
	//Calls the receive file function to receive the requested file
	receiveFile(filePath, sockfd);

	printf("Closing client...\n\n");
	close(sockfd);
	return 0;
}



/**
 * Modtager filstørrelsen 
 * Hvis størrelsen = 0 udskrives meddelelsen - Filen findes ikke
 * eller modtages og gemmes filen 1000 bit af gangen.
 *
 **/
void receiveFile(char*  _filePath, int _sockfd)
{
	//Opretter buffer
	char buffer[BUF_SIZE];

	
	read(_sockfd, buffer, sizeof(buffer));
	long filesize = atol(buffer);
	printf("Size of file: %ld\n", filesize);

	// If file exists, extract file name and create new file with same name
	if (filesize == 0) {
		error("The requested file does not exist\n");
		close(_sockfd); }

	int bytes;
	const char* fileName = extractFileName(_filePath);
	ofstream file_fw(fileName, ios::trunc | std::ios::binary);

	while (1)
	{
		// Read the 1000 bytes from the server
		// if less bytes are left, read the remaining
		bzero(buffer, sizeof(buffer));
		if (filesize < 1000)
			bytes = read(_sockfd, buffer, (filesize%1000));
		else
			bytes = read(_sockfd, buffer, sizeof(buffer));

		// Write the received data to the file in blocks of 1000 bytes
		file_fw.write(buffer, bytes);

		// Reduce the the filesize by amount read
		filesize -= bytes;

		//Debug message: Amount of bytes read and amount of bytes left to read
		printf("%d bytes read\n", bytes);
		printf("Bytes left to read: %ld\n", filesize);
		//sleep(1);

		// Close if file transer is complete
		if (filesize <= 0)
		{
			printf("File transer complete\n");
			file_fw.close();
			break;
		}
		
		// Close if error occurs
		if (bytes < 0)
		{
			printf("Error occured while receiving data\n");
			printf("TERMINATING\n");
			file_fw.close();
			break;
		}
	}
}