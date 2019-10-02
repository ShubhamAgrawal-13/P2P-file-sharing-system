#include<iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

using namespace std;
#define BUFF_SIZE 1024
#define PORT 1239

void* filehandle(void* args)
{
	int sockfd = *((int*)args);
	int flag;
	recv(sockfd, &flag, sizeof(flag), 0);
	if (flag==1)
	{
		FILE *fp = fopen ( "copyserverfile"  , "wb" );
		char Buffer [ BUFF_SIZE] ; 
		int file_size;
		recv(sockfd, &file_size, sizeof(file_size), 0);

		int n,size=file_size,progress=0;
		cout<<"File is Downloading\n";
		while ( ( n = recv( sockfd , Buffer ,   BUFF_SIZE, 0) ) > 0  &&  file_size > 0)
		{
			cout<<Buffer<<endl;
			fwrite (Buffer , sizeof(char), n, fp);
			memset ( Buffer , '\0', BUFF_SIZE);
			file_size = file_size - n;
			progress+=n;
			double tt = progress*(1.0)/size*100;
			cout<<"Downloading : "<<pthread_self()<<"   "<<tt<<endl;
		} 
		cout<<"File Downloaded\n";
		fclose ( fp );

	}
	if(flag==2)
	{
		char buf[50];
		recv(sockfd,buf, sizeof(buf), 0);

		FILE *fp = fopen ( buf  , "rb" );

		fseek ( fp , 0 , SEEK_END);
	  	int size = ftell ( fp );
	  	rewind ( fp );

	  	send ( sockfd , &size, sizeof(size), 0);

	  	cout<<size<<endl;
		char Buffer [ BUFF_SIZE] ; 
		int n,s=size,progress=0;

		cout<<"File is Uploading\n";
		while ( ( n = fread( Buffer , sizeof(char) , BUFF_SIZE , fp ) ) > 0  && size > 0 )
		{
			//cout<<Buffer<<endl;
			send (sockfd , Buffer, n, 0 );
	   	 	memset ( Buffer , '\0', BUFF_SIZE);
			size = size - n ;
			progress+=n;
			double tt = progress*(1.0)/s*100;
			cout<<"Uploading : "<<tt<<endl;
			cout<<n<<endl;
		}

		cout<<"File Uploaded\n";
		fclose ( fp );
	}
}

int main()
{
	cout<<"Server\n";
	int server_fd = socket (AF_INET, SOCK_STREAM, 0);
	cout<<"iiiiiiiii";
	struct sockaddr_in   addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( PORT );
	addr.sin_addr.s_addr=inet_addr("127.0.0.1");//INADDR_ANY;
	int addrlen = sizeof(sockaddr);

	if(bind (server_fd  , (struct sockaddr *)&addr , sizeof ( addr ) ))
	{
		exit(1);
	}
	listen (server_fd, 3);
	cout<<"kkkkkk";
	while(1)
	{
		int sockfd = accept ( server_fd , (struct sockaddr *)&addr , (socklen_t*)&addrlen);
		cout<<"Connection Established "<<sockfd<<endl;
		pthread_t thread;
	    int err;

	    err = pthread_create(&thread, NULL, filehandle,(void*)&sockfd);

	    if (err)
	    {
	        printf("An error occured: %d", err);
	        exit(1);
	    }
		
	}
	//close( sockfd);
	close( server_fd);
	

	return 0;

}