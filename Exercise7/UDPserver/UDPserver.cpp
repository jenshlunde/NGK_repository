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

void error(const char *msg);


int main(int argc, char *argv[])
{
	//Setup 
	printf("Server starting...\n");

	int sockfd, serv_length, error_n, sendSize;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char buffer[BUFSIZE];
	char bufferTx[BUFSIZE];
	char bufferTerm[BUFSIZE];
	
	//Create socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error("ERROR during opening a socket");

	//Bind server adress
	printf("Binds server address\n");

	serv_length = sizeof(serv_addr);
	bzero(&serv_addr, serv_length);
	bzero(&bufferTx, sizeof(bufferTx));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);		//Could parse from argument, but is hardcoded to avoid "fejl-40"
	if (bind(sockfd, (struct sockaddr *) &serv_addr, serv_length) < 0) 
		error("ERROR during binding");

	clilen = sizeof(struct sockaddr_in);

	printf("Waiting for a client...\n");

	while (true)
	{
		error_n = recvfrom(sockfd, buffer, STRBUFSIZE, recvFlag, (struct sockaddr*)&cli_addr, &clilen);
		if (error_n < 0) 
			error("ERROR during recvfrom");

        if ('U' == buffer[0] || 'u' == buffer[0])
        {
			printf("Received U/u - Sending Uptime\n");

            //Input stream to uptime file
            ifstream file_read("/proc/uptime", ios::binary);
            //Reading file
            file_read.read(buffer, BUFSIZE);
            //Closing input stream
            file_read.close();

            //Looking for the first newline to determine the length of the uptime file
            //as not to send the whole 1024 buffer with extra garbage values.
            for (size_t i = 0; i < BUFSIZE; i++)
			{
                if (buffer[i] == '\n'){
                    sendSize = i;
                    break;  
                } 
            }

			error_n = sendto(sockfd, buffer, sizeof(sendSize+1), sendFlag, (struct sockaddr *) &cli_addr, clilen);
		}
		else if ('L' == buffer[0] || 'l' == buffer[0])
		{
			printf("Received L/l\ - Sending LoadAvg\n");

            //Input stream to uptime file
            ifstream file_read("/proc/loadavg", ios::binary);
            //Reading file
            file_read.read(buffer, BUFSIZE);
            //Closing input stream
            file_read.close();

            //Looking for the first newline to determine the length of the uptime file
            //as not to send the whole 1024 buffer with extra garbage values.
            for (size_t i = 0; i < BUFSIZE; i++)
			{
                if (buffer[i] == '\n'){
                    sendSize = i;
                    break;  
                } 
            }

			snprintf(bufferTx, sizeof(bufferTx), "SERVER: not a valid parameter \n");		
			
			error_n = sendto(sockfd, buffer, sizeof(sendSize+1), sendFlag, (struct sockaddr *) &cli_addr, clilen);
		}
		else
		{
			snprintf(bufferTx, sizeof(bufferTx), "SERVER: not a valid parameter \n");		
			error_n = sendto(sockfd,bufferTx,sizeof(bufferTx),sendFlag,(struct sockaddr *) &cli_addr,clilen);
			if (error_n  < 0)
				error("ERROR during sendto");
		}
		printf("Message handled - waiting for a client...\n");
	}
	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}
