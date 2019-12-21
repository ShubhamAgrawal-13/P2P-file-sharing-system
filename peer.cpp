#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

#include "listenerpart1.h" //server
//#include "requesthandlerpart.h" //client
#include "requesthandlerpart3.h"

using namespace std;


int main(int argc,char* argv[])
{
	if(argc<3)
	{
		cout<<"Give port number through command line \n";
		return 0;
	}
	// char* ip;
	// ip=argv[1];
	
	char* token = strtok(argv[1], ":");
	int port = atoi(strtok(NULL, ":"));
	cout<<argv[1]<<" "<<token<<" "<<port<<endl;

	clientinfo cli;
	strcpy(cli.ip,token);
	strcpy(cli.track,argv[2]);
	cli.port=port;
	// int port;
	// cout<<"Enter Port : ";
	// cin>>port;
	pthread_t threadid1,threadid2;
	int err1,err2;

	err1 = pthread_create(&threadid1, NULL, listener,(void*)&port);

	if (err1)
	{
	    printf("Error occured during client thread creation 1 : %d", err1);
	    exit(1);
	}

	err2 = pthread_create(&threadid2, NULL, request,(void*)&cli);

	if (err2)
	{
	    printf("Error occured during server thread creation 2 : %d", err2);
	    exit(1);
	}

	pthread_join(threadid1,NULL);
	pthread_join(threadid2,NULL);

	cout<<"Bye\n";
	return 0;
}
