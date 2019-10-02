#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

using namespace std;
#define BUFF_SIZE 1024

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


void* client(void* cargs)
{
	cout<<"Jai Shree Krishna Client \n";
	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	//cout<<"jjjjjjj";
	char ip[20];
	int port;
	cout<<"Enter IP : ";
	cin>>ip;
	cout<<"Enter Port : ";
	cin>>port;
	struct sockaddr_in  serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons( port );
    serv_addr.sin_addr.s_addr=inet_addr(ip);

	int clientid = connect ( sockfd  , (struct sockaddr *)&serv_addr  , sizeof(serv_addr) );
	cout<<"-----------";
	if(clientid<0)
	{
		cout<<"Connection failed\n";
		exit(1);
	}
	cout<<"Connected\n";

	while(1)
	{
		string s1,s2;
		int flag;
		cout<<"Enter Command : ";
		cin>>s1>>s2;
		if(s1=="upload")
		{
			flag=1;
			char buff[50];
			strcpy(buff,s2.c_str());
			send(sockfd,&flag,sizeof(flag),0);
			FILE *fp = fopen ( buff  , "rb" );

			fseek ( fp , 0 , SEEK_END);
		  	int size = ftell ( fp );
		  	rewind ( fp );

		  	send ( sockfd , &size, sizeof(size), 0);

			char Buffer [ BUFF_SIZE] ; 
			int n,s=size,progress=0;

			cout<<"File is Uploading\n";
			while ( ( n = fread( Buffer , sizeof(char) , BUFF_SIZE , fp ) ) > 0  && size > 0 )
			{
				send (sockfd , Buffer, n, 0 );
		   	 	memset ( Buffer , '\0', BUFF_SIZE);
				size = size - n ;
				progress+=n;
				double tt = progress*(1.0)/s*100;
				cout<<"Uploading : "<<tt<<endl;
			}

			cout<<"File Uploaded\n";
			fclose ( fp );
		}


		if(s1=="download")
		{
			flag=2;
			send(sockfd,&flag,sizeof(flag),0);
			char buff[50];
			strcpy(buff,s2.c_str());
			send(sockfd,buff,sizeof(buff),0);

			FILE *fp = fopen ( "copyfileclient"  , "wb" );
			char Buffer [ BUFF_SIZE] ; 
			int file_size;
			recv(sockfd, &file_size, sizeof(file_size), 0);
			cout<<file_size<<endl;
			int n,size=file_size,progress=0;
			cout<<"File is Downloading\n";
			while ( file_size > 0 && ( n = recv( sockfd , Buffer ,   BUFF_SIZE, 0) ) > 0)
			{
				//cout<<Buffer<<endl;
				fwrite (Buffer , sizeof(char), n, fp);
				memset ( Buffer , '\0', BUFF_SIZE);
				file_size = file_size - n;
				progress+=n;
				double tt = progress*(1.0)/size*100;
				cout<<"Downloading : "<<tt<<endl;
				cout<<n<<endl;
			} 
			cout<<"File Downloaded\n";
			fclose ( fp );

		}

		if(s1=="stop")
		{
			break;
		}

	}

	
	close( sockfd);
	//sleep(3);
}


void* server(void* sargs)
{
	int port = *((int*)sargs);
	cout<<"Jai Shree Krishna Server "<<port<< "\n";
	int server_fd = socket (AF_INET, SOCK_STREAM, 0);
	//cout<<"iiiiiiiii";
	struct sockaddr_in  addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr=INADDR_ANY;
	int addrlen = sizeof(sockaddr);

	bind (server_fd  , (struct sockaddr *)&addr , sizeof ( addr ) );
	cout<<"Listening......request";
	listen (server_fd, 3);

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

}


int main(int argc,char* argv[])
{
	// if(argc<=1)
	// {
	// 	cout<<"Give port number through command line \n";
	// 	return 0;
	// }

	// int port = argv[1];
	int port;
	cout<<"Enter Port : ";
	cin>>port;
	pthread_t threadid1,threadid2;
	int err1,err2;

	err1 = pthread_create(&threadid1, NULL, client,NULL);

	if (err1)
	{
	    printf("Error occured during client thread creation 1 : %d", err1);
	    exit(1);
	}

	err2 = pthread_create(&threadid2, NULL, server,(void*)&port);

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