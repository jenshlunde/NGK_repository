//============================================================================
// Description : get_measurement in C++
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
#define sendFlag 0

//Prototype: Error message function for 'cleaner' code
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	//Client socket setup
	printf("Starting client...\n");

	int sockfd, n_error;
	unsigned int length;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char buffer[BUFSIZE];
   	char toSend[2] = {'0','\n'};

	//Terminal print for wrong arguments
	if (argc < 3)
	    error("ERROR usage: <ip> <command ID>");

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) 
	    error("ERROR: opening socket");

	//Terminal print for wrong ip
	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	if (hp == 0) 
	    error("ERROR no such host");

	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	
	server.sin_port = htons(atoi(argv[2]));
   	
	length = sizeof(struct sockaddr_in);

   	toSend[0] = *(argv[2]);

   	n_error = sendto(sockfd,toSend, 2, 0,(const struct sockaddr *)&server,length);
	if(n_error < 0)
		error("ERROR: sendto");

	n_error = recvfrom(sockfd,toSend,256,sendFlag,(struct sockaddr*)&from, &length);
	if(n_error < 0)
		error("ERROR: recvfrom");

	printf("\nGot an ack: ");
	write(1,buffer,n_error);

	printf("Closing client...\n\n");
	close(sockfd);
	return 0;
}