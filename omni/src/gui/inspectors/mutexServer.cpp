#include "mutexServer.h"
#include <QCoreApplication>

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


MutexServer::MutexServer(QString host, int port)
{
	mTerminate = false;
	mHost = host;
	mPort = port;
}

void MutexServer::safeTerminate ()
{
	mTerminate = true;
}

void MutexServer::run ()
{
     	int sockfd, newsockfd, portno;
     	char buffer[256];
     	struct sockaddr_in serv_addr, cli_addr;
     	int n;
	socklen_t clilen;
     	sockfd = socket(AF_INET, SOCK_STREAM, 0);
     	if (sockfd < 0) 
        		printf("ERROR opening socket");
     	bzero((char *) &serv_addr, sizeof(serv_addr));
     	portno = mPort;
     	serv_addr.sin_family = AF_INET;
     	serv_addr.sin_addr.s_addr = INADDR_ANY;
     	serv_addr.sin_port = htons(portno);
     	if (bind(sockfd, (struct sockaddr *) &serv_addr,
              		sizeof(serv_addr)) < 0) 
              		printf("ERROR on binding");
     	listen(sockfd,5);
     	clilen = sizeof(cli_addr);

	while (1) {
     		newsockfd = accept(sockfd, 
                 		(struct sockaddr *) &cli_addr, 
                 		&clilen);
     		if (newsockfd < 0) 
          		printf("ERROR on accept");
     		bzero(buffer,256);
     		n = write(newsockfd,"l",1);
     		if (n < 0)
			printf("ERROR writing to socket\n");

     		n = read(newsockfd,buffer,255);	// Locked
     		//if (n < 0) printf("ERROR reading from socket");
     		//printf("Here is the message: %s\n",buffer);
		close (newsockfd);		// Closing means they are done with lock.
	}
}

