/* A simple server in the internet domain using TCP
The port number is passed as an argument 
Based on example from Michael Alrøe

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
using namespace std;

void sendFile(string fileName, long fileSize, int outToClient);

void error(const char *msg)
{
	perror(msg);
	exit(1);
}


int main(int argc, char *argv[])
{
	//Setup 
	printf("Server starting...\n");

	int sockfd, newsockfd, n;
	char filePath[100];
	socklen_t clilen;
	char buffer[BUFSIZE];
	char bufferTx[BUFSIZE];
	struct sockaddr_in serv_addr, cli_addr;
	
	//Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
	listen(sockfd, 5); //5 clients max

	clilen = sizeof(cli_addr);

	while(true)
	{
		printf("Waiting for client...\n");
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
            error("ERROR during accept");
		else
            printf("\nAccepted client\n");
	
		// Read message from client
		bzero(buffer, sizeof(buffer));
		readTextTCP(newsockfd, filePath, sizeof(filePath));

		// Check if requested file exsists
		printf("Received filepath from client: %s\n", filePath);	
		printf("Checking if file exists...\n");
		long fileSize = getFilesize(filePath);		//If fileSize == 0, file doesn't exist

		// Print debug message and send filebyte to client
		if (fileSize == 0)
		{
			printf("File not found\n");
			snprintf(bufferTx, sizeof(bufferTx), "%ld", fileSize);
			n = write(newsockfd, bufferTx, strlen(bufferTx));
			if (n < 0)
				error("ERROR writing to socket");
		} else
		{
			printf("File found - sending file...\n");
			snprintf(bufferTx, sizeof(bufferTx), "%ld", fileSize);
			n = write(newsockfd, bufferTx, strlen(bufferTx));
			if (n < 0)
				error("ERROR writing to socket");
			
			// If file exists, send the file
			sendFile(filePath, fileSize, newsockfd);
		}

		// Close socket connection to client
		close(newsockfd);
	}

	close(sockfd);
	return 0;
}

/**
 * Funktion der sender fil til clienten
**/
void sendFile(string _fileName, long _fileSize, int _outToClient)
{
    printf("\nSending File...\n");
	//Define buffer with size of BUFSIZE (defined in iknlib header) bytes, and the filename
    uint8_t Buffer[BUFSIZE];
	string filePath = _fileName;
	int bytesRead, bytesWritten;
	
    //Deffines input stream
	ifstream file_fr(filePath, std::ios::binary);

    while (_fileSize > 0)	// Loop så længe der er bytes tilbage i filen
	{
		// Read 1000 bytes from the file, load it into the buffer
		// Måske skulle det her ikke hardcodes, men være en DEFINE??

		//Tjekker at den resterende filesize er under 1000 (den sidste chunk)
		if (_fileSize < 1000)
		{
			file_fr.read((char*)Buffer, _fileSize);
			bytesRead = file_fr.gcount();
		} 
		else	//Hvis der er over 1000 tilbage, så sættes streamsize til BUFSIZE frem for filesize	
		{
			file_fr.read((char*)Buffer, BUFSIZE);
			bytesRead = file_fr.gcount();
		}

		//Send data to the client socket
		bytesWritten = write(_outToClient, Buffer, bytesRead);
    	if (bytesWritten < 0)
        	error("Error sending to socket\n");
		
		//Debug message 
		printf("%d bytes sent\n", bytesWritten);

		// Reduce the filesize by amount written
		_fileSize -= bytesWritten;
	}
	
	printf("File transfer complete\n\n");

    // Close the file stream
    file_fr.close();
}


