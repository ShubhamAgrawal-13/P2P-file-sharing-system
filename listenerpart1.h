#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <math.h>
#include <arpa/inet.h>
#include <openssl/sha.h>


using namespace std;

#define BUFF_SIZE 1024
#define CHUNK_SIZE 1024*512

int logout=0;

// void calucalteSHA1(char buff[],char ssha1[])
// {
// 	unsigned char digest[SHA_DIGEST_LENGTH];
// 	SHA1((unsigned char*)&buff, strlen(buff), (unsigned char*)&digest);    

//     //char ssha1[SHA_DIGEST_LENGTH*2+1];

//     for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
//          sprintf(&ssha1[i*2], "%02x", (unsigned int)digest[i]);
// }


void* filehandle(void* args)
{
	int sockfd = *((int*)args);
	int flag;

	recv(sockfd, &flag, sizeof(flag), 0);
	cout<<flag<<"-----------\n";
	if(flag==3)
	{
		cout<<"Akai\n";
		recv(sockfd,&logout,sizeof(logout),0);
		cout<<logout<<endl;
	}
	if (flag==1)
	{
		char buff[50],fileserver[50];
		memset(buff,'\0',50);
		recv(sockfd, buff, sizeof(buff), 0);
		sprintf(fileserver,"copy-%s",buff);
		FILE *fp = fopen ( fileserver  , "wb" );
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
			//cout<<"Downloading : "<<pthread_self()<<"   "<<tt<<endl;

		} 

		cout<<"File Downloaded\n";
		fclose ( fp );
		pthread_exit(NULL); 

	}
	if(flag==2)
	{
		char buf[50];
		memset(buf,'\0',sizeof(buf));
		int peers;
	  	recv(sockfd,&peers,sizeof(peers),0);

	  	int index;
	  	recv(sockfd,&index,sizeof(index),0);
	  	sleep(0.3);

		recv(sockfd,buf, sizeof(buf), 0);
		
		cout<<buf<<endl;
		cout<<peers<<"----"<<index<<"\n";

		FILE *fp = fopen ( buf  , "rb" );
		if(fp==NULL)
		{
			cout<<"File doesn't exist\n";
			return NULL;
		}


		fseek ( fp , 0 , SEEK_END);
	  	int size = ftell ( fp );
	  	rewind ( fp );

	  	//send ( sockfd , &size, sizeof(size), 0);
	  	

	  	cout<<size<<endl;
		char Buffer [ BUFF_SIZE];
		memset( Buffer , '\0', BUFF_SIZE); 
		int n,s=size,progress=0;

		int fsize=size/(CHUNK_SIZE);
		cout<<fsize<<endl;
		int psize=fsize;
		if(size%(CHUNK_SIZE)!=0)
			psize=psize+1;

		s=ceil(s/peers);
		cout<<"File is Uploading\n";
		for(int i=index;i<psize*CHUNK_SIZE;i+=peers*CHUNK_SIZE)
		{
			for(int j=i;j<i+CHUNK_SIZE && j<size;j+=BUFF_SIZE)
			{
				fseek ( fp , j ,SEEK_SET );
				n = fread( Buffer , sizeof(char) , BUFF_SIZE , fp );
				//cout<<ftell(fp)<<"---\n"; 
				send(sockfd , Buffer, n, 0 );
				//cout<<n<<" "<<Buffer<<endl;
				s-=n;
				int ack=0;
				recv(sockfd,&ack,sizeof(ack),0);
				memset ( Buffer , '\0', BUFF_SIZE);
			}
			cout<<i/(CHUNK_SIZE)<<"*******"<<endl;
		}

		cout<<"File Uploaded\n";
		fclose ( fp );
	}
	close(sockfd);
	cout<<"connection request completed\n";
}

void* listener(void* sargs)
{
	int port = *((int*)sargs);

	cout<<"Jai Shree Krishna Listner Part "<<port<< "\n";

	int server_fd = socket (AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in  addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr=INADDR_ANY;
	int addrlen = sizeof(sockaddr);

	int x=bind (server_fd  , (struct sockaddr *)&addr , sizeof ( addr ) );
	if(x<0)
	{
		cout<<"Error in binding : port is already in use \n";
		exit(1);
	}
	cout<<"Listening request\n";
	listen (server_fd, 50);

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

	    if(logout==1)
	    {
	    	cout<<"Listener part ended\n";
	    	break;
	    }
	}
	
}