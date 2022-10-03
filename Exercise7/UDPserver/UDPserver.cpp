/* A simple server in the internet domain using UDP

Modified: Jens Lunde and Kaleed
Extended to support file transfer!
*/

#include "iknlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>

#define STRBUFSIZE 250
#define recvFlag 0b0000000
#define sendFlag 0b0000000
using namespace std;

//void sendFile(string fileName, long fileSize, int outToClient);

void error(const char *msg)
{
	perror(msg);
	exit(1);
}


int main(int argc, char *argv[])
{
	//Setup 
	printf("Server starting...\n");

	int sockfd, newsockfd, error_n;
	unsigned long fromlen;
	char filePath[100];
	socklen_t clilen;
	char buffer[BUFSIZE];
	char bufferTx[BUFSIZE];
	struct sockaddr_in serv_addr, cli_addr;
	
	//Create socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error("ERROR during opening a socket");

	//Bind server adress
	printf("Binds server address\n");

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR during binding");

	printf("Waiting for a client...\n");

	while (true)
	{
  	
	error_n = recvfrom(sockfd, buffer, STRBUFSIZE, recvFlag, (struct sockaddr*)&cli_addr, &clilen);
  	if (error_n < 0) 
		error("recvfrom crashed");
	
	snprintf(bufferTx, sizeof(bufferTx), "Got your message");
	
	error_n = sendto(sockfd,bufferTx,sizeof(bufferTx),sendFlag,(struct sockaddr *) &cli_addr,clilen);
  	if (error_n  < 0)
    	error("sendto crashed");

	//.... switch på indkommende kommando, send det relevante info tilbage
  	}

	return 0;
}