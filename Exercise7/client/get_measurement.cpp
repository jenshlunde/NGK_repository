//============================================================================
// Description : get_measurement - UDP client, Ansi-style
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

#define BUF_SIZE 1000
#define BUFFER_SIZE 250
#define sendFlag 0b0000000

//Prototype: Error message function for 'cleaner' code
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

//Prototype: Receive file function 
//void receiveFile(char* _filePath, int _sockfd);

int main(int argc, char *argv[])
{
	//Client socket setup
	printf("Starting client...\n");

	int sockfd, error_n;
	unsigned int length;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char buffer[BUFFER_SIZE];

	//Terminal print for wrong arguments
	if (argc < 3)
	    error("ERROR usage: <ip> <command>");

	//Terminal print for wrong ip
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) 
	    error("ERROR opening socket");

	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	if (hp == NULL) 
	    error("ERROR no such host");

	printf("Setting up sending parameters...\n");

	bcopy((char*)hp->h_addr, (char*)&server.sin_addr, hp->h_length);
	server.sin_port = htons(PORT);
	length = sizeof(struct sockaddr_in);

	//printf("Enter message to send: ");

	bzero(buffer, BUFFER_SIZE);
	//fgets(buffer, BUFFER_SIZE-1, stdin);

	error_n = snprintf(buffer, sizeof(buffer), "%s", argv[2]);

	error_n = sendto(sockfd, buffer, strlen(buffer), sendFlag, (const struct sockaddr *)&server, length);
	if (error_n < 0)
		error("ERROR during sendto");
	
	error_n = recvfrom(sockfd, buffer, BUFFER_SIZE, sendFlag, (struct sockaddr *)&from, &length);
	if (error_n < 0)
		error("ERROR during recvfrom");
	
	write(1,"Got an ack: ",12);
	write(1,buffer,error_n);
	printf("\n");

	printf("Closing client...\n\n");
	close(sockfd);
	return 0;
}