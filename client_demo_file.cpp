#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <string.h>
#include <cstring>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

using namespace std;
#define BUFF_SIZE 1024
#define PORT 1239

int main()
{
	cout<<"Client\n";
	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	cout<<"jjjjjjj";
	struct sockaddr_in  serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons( PORT );
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");


	int clientid = connect ( sockfd  , (struct sockaddr *)&serv_addr  , sizeof(serv_addr) );
	cout<<"-----------";
	if(clientid<0)
	{
		cout<<"Connection failed\n";
		return 1;
	}
	cout<<"Connected\n";
	string s1,s2;
	int flag;
	cout<<"Enter Command : ";
	cin>>s1>>s2;
	if(s1=="upload")
	{
		flag=1;
		send(sockfd,&flag,sizeof(flag),0);
		FILE *fp = fopen ( "/home/shubham/Music/Detective_Conan.mp4"  , "rb" );

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
	close( sockfd);

	return 0;

}