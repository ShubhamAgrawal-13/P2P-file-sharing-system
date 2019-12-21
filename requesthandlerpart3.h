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

char ip[20][20];
int port[20];

int currentuser=-1;

typedef struct ip_port
{
	char ip[20];
	char track[50];
	int port;
}clientinfo;


int abitv[20][5000];


struct chunkdata
{
	int port;
	int sockfd;
	int size;
	int peers;
	char ip[20];
	char str1read[50];
	char str2write[50];
	int start;
};


void piece_algorithm(int nobv,int length)
{
	int temp[5000];
	int irecent=0;
	for(int j=0;j<length;j++)
	{
		for(int i=0;i<nobv;i++)
		{

		}
		temp[j]=irecent;
	}

}

// void calucalteSHA1(char buff[],char ssha1[])
// {
// 	unsigned char digest[SHA_DIGEST_LENGTH];
// 	SHA1((unsigned char*)&buff, strlen(buff), (unsigned char*)&digest);    

//     //char ssha1[SHA_DIGEST_LENGTH*2+1];

//     for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
//          sprintf(&ssha1[i*2], "%02x", (unsigned int)digest[i]);
// }

void* copyfile(void* args)
{
	struct chunkdata* cd = (struct chunkdata*)args;
	char* ip=cd->ip;
	int size=cd->size;
	int port=cd->port;
	int start=cd->start;
	//int sockfd=cd->sockfd;
	//char ip[20]="127.0.0.1";
	int peers=cd->peers;
	char* filenames=cd->str1read;
	char* filenamed=cd->str2write;
	cout<<endl;

	//cout<<ip<<" ------- "<<port<<" -------- "<<filenames<<" ------- "<<filenamed<<" ----- "<<peers<<" ------ "<<start<<endl;

	struct sockaddr_in  serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons( port );
	serv_addr.sin_addr.s_addr=inet_addr(ip);
	//cout<<port<<" ----------- \n";
	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	int clientid = connect ( sockfd  , (struct sockaddr *)&serv_addr  , sizeof(serv_addr) );
	
	if(clientid<0)
	{
		cout<<"Connection failed\n";
		exit(1);
	}

	int flag=2;
	send(sockfd,&flag,sizeof(flag),0);
	sleep(0.3);
	send(sockfd,&peers,sizeof(peers),0);
	sleep(0.3);
	send(sockfd,&start,sizeof(start),0);
	sleep(0.3);
	send(sockfd,filenames,strlen(filenames),0);
	sleep(0.3);
	
	//FILE* fps = fopen(filenames,"rb");
	FILE* fpd = fopen(filenamed,"rb+");

	int fsize=size/(CHUNK_SIZE);
	//cout<<fsize<<endl;
	int psize=fsize;
	if(size%(CHUNK_SIZE)!=0)
		psize=psize+1;
	cout<<psize<<endl;
	//cout<<"hhh";
	char mainBuffer[CHUNK_SIZE];
	memset(mainBuffer,'\0',CHUNK_SIZE);
	int s=ceil(size/peers);
	for(int i=start;i<psize*CHUNK_SIZE;i+=peers*CHUNK_SIZE)
	{
		
		int k=0;
		for(int j=i;j<i+CHUNK_SIZE && j<size;j+=BUFF_SIZE)
		{
			//fseek ( fpd , j ,SEEK_SET );
			//cout<<ftell(fpd)<<"---\n"; 
			char Buffer[BUFF_SIZE]; 
			memset(Buffer,'\0',BUFF_SIZE);

			int n;
			n = recv( sockfd , Buffer ,   BUFF_SIZE, 0);
			//cout<<n<<"---"<<Buffer<<endl;
			for(int p=0;p<n;p++)
			{
				mainBuffer[k++]=Buffer[p];
			}
			//cout<<n<<endl;
			s=s-n;
			int ack=0;
			send(sockfd,&ack,sizeof(ack),0);
			
			//memset ( Buffer , '\0', BUFF_SIZE);
		}
		//cout<<mainBuffer<<" "<<endl;
		fseek ( fpd , i ,SEEK_SET );
		fwrite(mainBuffer,sizeof(char),k,fpd);
			
		//cout<<pthread_self()<<" "<<i/(CHUNK_SIZE)<<" "<<ftell(fpd)<<" "<<k<<endl;	
	}
	
	//cout<<"written\n";
	// fclose(fps);
	fclose(fpd);
	cout<<"copy thread completed"<<endl;
}



void* request(void* cargs)
{
	cout<<"Jai Shree Krishna Client \n";
	clientinfo* cl=(clientinfo*)cargs;
	int globalport = cl->port;
	char* globalip=cl->ip;
	char* trackfile=cl->track;

	FILE* frt = fopen(trackfile,"r");

	if(frt==NULL)
	{
		cout<<"File doesn't exist\n";
		return NULL;
	}

	size_t length=0;
	char* bufname=NULL;
	char ipt[20];
	char ipt1[20];

	int n;

	n = getline(&bufname, &length, frt);
	strcpy(ipt,bufname);
	n = getline(&bufname, &length, frt);
	int portt=atoi(bufname);
	ipt[strlen(ipt)-1]='\0';

	n = getline(&bufname, &length, frt);
	strcpy(ipt1,bufname);
	n = getline(&bufname, &length, frt);
	int portt1=atoi(bufname);
	ipt1[strlen(ipt1)-1]='\0';
	

	cout<<"Tracker 1 : "<<ipt<<" "<<portt<<endl;
	cout<<"Tracker 2 : "<<ipt1<<" "<<portt1<<endl;

	cout<<"Connected\n";
	while(1)
	{
		cout<<"Client : \n";
		int peers,size;
		// char ipt[20];
		// int portt;

		// cout<<"Enter Tracker ip address : ";
		// cin>>ipt;
		// cout<<"Enter Port : ";
		// cin>>portt;

		char s1[50],s2[50],s3[50],s4[50];
		char cmd[100];
		char pack[4][50];
		cout<<"Enter Command : \n";
		//getline(cin,cmd);
		scanf("%[^\n]%*c",cmd);	
		int pc=0;
		char* token = strtok(cmd, " ");
		// strcpy(pack[pc],token);
		// pc++;
		while (token != NULL) 
		{ 
	        // printf("%s\n", token);
	        strcpy(pack[pc],token);
			pc++;
	        token = strtok(NULL, " "); 
    	} 

    	// for(int i=0;i<pc;i++)
    	// {
    	// 	cout<<pack[i]<<" "<<i<<endl;
    	// }

    	//cout<<endl;

    	strcpy(s1,pack[0]);
    	int kflagt=0;
		
		struct sockaddr_in  serv_addr;
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons( portt );
		serv_addr.sin_addr.s_addr=inet_addr(ipt);

		//cout<<port<<" ----------- \n";
		int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
		int clientid = connect ( sockfd  , (struct sockaddr *)&serv_addr  , sizeof(serv_addr) );
		if(clientid<0)
		{
			cout<<"connection failed from Tracker 1 : \n";
			cout<<"connecting to Tracker 2 : \n";
			kflagt=1;
		}
		if(kflagt)
		{
			serv_addr.sin_port = htons( portt1 );
			serv_addr.sin_addr.s_addr=inet_addr(ipt1);
			int clientid2 = connect ( sockfd  , (struct sockaddr *)&serv_addr  , sizeof(serv_addr) );
			if(clientid2<0)
			{
				cout<<"connection failed\n"<<endl;
			}
		}

		if(strcmp(s1,"logout")==0)
		{
			int flag=3;
			int logout=1;
			int c=0;
			send(sockfd,&flag,sizeof(flag),0);
			recv(sockfd,&c,sizeof(c),0);
			currentuser=c;
			cout<<"logged out successfully\n"<<endl;
		}


		if(strcmp(s1,"list_files")==0)
		{
			if(currentuser==-1)
			{
				cout<<"first login to upload file \n";
				continue;
			}

			int gid=1;
			int flag=10;
			int sze=0;
			send(sockfd,&flag,sizeof(flag),0);
			recv(sockfd,&sze,sizeof(sze),0);

			for(int i=0;i<sze;i++)
			{
				char name[50];
				memset(name,'\0',sizeof(name));
				recv(sockfd,name,sizeof(name),0);
				cout<<name<<endl;
			}


		}

		strcpy(s2,pack[1]);
		if(strcmp(s1,"create_user")==0)
		{
			if(pc<3)
			{
				cout<<"please enter full command\n";
				continue;
			}

			strcpy(s3,pack[2]);
			int flag=4;
			int ack=0;
			send(sockfd,&flag,sizeof(flag),0);
			send(sockfd,&globalport,sizeof(globalport),0);
			send(sockfd,globalip,strlen(globalip),0);
			recv(sockfd,&ack,sizeof(ack),0);
			send(sockfd,s2,strlen(s2),0);
			recv(sockfd,&ack,sizeof(ack),0);
			send(sockfd,s3,strlen(s3),0);
			recv(sockfd,&ack,sizeof(ack),0);
			//cout<<"Good\n";

		}

		if(strcmp(s1,"login")==0)
		{
			if(pc<3)
			{
				cout<<"please enter full command\n";
				continue;
			}
			strcpy(s3,pack[2]);
			int flag=5;
			int ack=0;
			send(sockfd,&flag,sizeof(flag),0);
			send(sockfd,&globalport,sizeof(globalport),0);
			send(sockfd,globalip,strlen(globalip),0);
			recv(sockfd,&ack,sizeof(ack),0);
			send(sockfd,s2,strlen(s2),0);
			recv(sockfd,&ack,sizeof(ack),0);
			send(sockfd,s3,strlen(s3),0);
			recv(sockfd,&ack,sizeof(ack),0);
			currentuser=ack;
			if(ack==-1)
			{
				cout<<"login unsuccessful - user is not present\n";
			}
			else if(ack==0)
			{
				currentuser=-1;
				cout<<"login unsuccessful - user is already logged in some other system\n";
			}
			else
			{
				cout<<"login successfully\n";
			}

			//cout<<"Good login\n";
		}

		if(strcmp(s1,"upload_file")==0)
		{
			if(currentuser==-1)
			{
				cout<<"first login to upload file \n";
				continue;
			}


			int flag=1;
			int ack=0;
			char buff[50];
			char *ip=globalip;
			strcpy(buff,s2);

			int gid=atoi(pack[2]);

			//cout<<"hello 1\n";
			FILE *fp = fopen ( buff  , "rb" );

			if(fp==NULL)
			{
				cout<<"File doesn't exist\n";
				continue;
			}

			fseek ( fp , 0 , SEEK_END);
		  	int size = ftell ( fp );
		  	rewind ( fp );
		  	int nsize=ceil(size*1.0/(CHUNK_SIZE));
		  //	cout<<nsize<<endl;
		  	char a[nsize+1];
		  	memset(a,'1',nsize);
		  	a[nsize]='\0';
		  	//cout<<"hello 2\n";
			send(sockfd,&flag,sizeof(flag),0);
			send(sockfd,&globalport,sizeof(globalport),0);
			send(sockfd,&gid,sizeof(gid),0);
			recv(sockfd,&ack,sizeof(ack),0);
			send(sockfd,ip,strlen(ip),0);
			
			recv(sockfd,&ack,sizeof(ack),0);
		  	send(sockfd,buff,strlen(buff),0);
		  	recv(sockfd,&ack,sizeof(ack),0);
		  	send ( sockfd , &size, sizeof(size), 0);
		  	recv(sockfd,&ack,sizeof(ack),0);

		  	cout<<a<<endl;
		  	send(sockfd,a,nsize,0);
			recv(sockfd,&ack,sizeof(ack),0);
			cout<<"file uploaded successfully\n";
			fclose ( fp );
		}

		if(strcmp(s1,"download_file")==0)
		{
			if(currentuser==-1)
			{
				cout<<"first login to download file \n";
				continue;
			}

			int gid=atoi(pack[1]);
			strcpy(s3,pack[2]);
			strcpy(s4,pack[3]);

			int flag=2;
			send(sockfd,&flag,sizeof(flag),0);
			send(sockfd,s3,strlen(s3),0);
			
			recv( sockfd , &peers ,   sizeof(peers), 0);
			recv( sockfd , &size ,   sizeof(size), 0);

			cout << size <<"  "<< peers <<endl;
			int n;

			for(int i=1;i<=peers;i++)
			{
				int port1;
				char ip1[20];
				int ack=0;
				memset(ip1,'\0',20);
				recv( sockfd , &port1 ,  sizeof(port1), 0);
				send(sockfd,&ack,sizeof(ack),0);
				n=recv( sockfd , ip1 ,  20, 0);
				//sleep(0.3);
				send(sockfd,&ack,sizeof(ack),0);
				port[i-1]=port1;
				strcpy(ip[i-1],ip1);
				cout<<n<<" "<<port[i-1]<<" "<<ip[i-1]<<endl;
			}

			//close(sockfd);

			if(size==0 || port==0)
			{
				cout<<"file not present\n";
				continue;
			}


			char buff[50];
			strcpy(buff,s3);
			cout<<s3<<"  "<<buff<<endl;
			char fileclient[50];
			sprintf(fileclient,"%s/%s",s4,buff);

			FILE *fpd = fopen ( fileclient  , "wb" );
			//FILE *fp = fopen ( "copyfileclient"  , "wb" );
			char Buffer [ BUFF_SIZE] ; 
			//cout<<size<<endl;
			//cout<<"------\n";
			memset(Buffer,'\0',BUFF_SIZE);

			int fsize=size/(BUFF_SIZE);
			cout<<fsize<<endl;
			int psize=fsize;

			while(fsize>0)
			{
				fwrite(Buffer,1,BUFF_SIZE,fpd);
				fsize--;
			}

			fsize=size%(BUFF_SIZE);
			cout<<fsize<<endl;
			char ch='\0';

			while(fsize>0)
			{
				fwrite(&ch,1,1,fpd);
				fsize--;
			}

			fclose(fpd);

			memset(buff,'\0',50);
			strcpy(buff,s3);
		  	int nsize=ceil(size*1.0/(CHUNK_SIZE));
		  	cout<<nsize<<endl;
		  	char a[nsize+1];
		  	memset(a,'0',nsize);
		  	a[nsize]='\0';
		  	cout<<"hello 2\n";
			int ack=0;
			send(sockfd,&globalport,sizeof(globalport),0);
			send(sockfd,&gid,sizeof(gid),0);
			recv(sockfd,&ack,sizeof(ack),0);
			send(sockfd,globalip,strlen(globalip),0);
			
			recv(sockfd,&ack,sizeof(ack),0);
			
		  	send(sockfd,buff,strlen(buff),0);
		  	recv(sockfd,&ack,sizeof(ack),0);
		  	send ( sockfd , &size, sizeof(size), 0);
		  	recv(sockfd,&ack,sizeof(ack),0);

		  	cout<<a<<endl;
		  	send(sockfd,a,nsize,0);
			recv(sockfd,&ack,sizeof(ack),0);


			pthread_t threadid[peers];
			int err;
			//cout<<peers<<"--------\n";
			int i;
			//struct chunkdata* cd=(struct chunkdata*)malloc(sizeof(struct chunkdata));
			struct chunkdata cd[peers];
			for(i=0;i<peers;i++)
			{
				cd[i].port=port[i];
				strcpy(cd[i].ip,ip[i]);
				cd[i].size=size; 
				cd[i].start=i*CHUNK_SIZE;
				cd[i].peers=peers;
				//cd[i].sockfd=sockfd;
				strcpy(cd[i].str1read,buff);
				strcpy(cd[i].str2write,fileclient);
				// cout<<"hello\n";
				// cd->port=port[i];
				// cout<<"hello1\n";
				// strcpy(cd->ip,ip[i]);
				// cd->size=size;
				// cd->start=i*CHUNK_SIZE;
				// cout<<"hello\n";
				// cd->peers=peers;
				// //cd[i].sockfd=sockfd;
				// strcpy(cd->str1read,buff);
				// strcpy(cd->str2write,fileclient);
				cout<<"hello\n";
				err = pthread_create(&threadid[i], NULL, copyfile,(void*)&cd[i]);
				if (err)
				{
					printf("Error occured during chunk thread creation : %d", err);
					exit(1);
				}
				//pthread_join(threadid[i],NULL);
			}

			for(int i=0;i<peers;i++)
			{
					pthread_join(threadid[i],NULL);
					//pthread_detach(threadid[i]);
			}
		
			cout<<"Bye\n";

			cout<<"File Downloaded\n";
		}
		close( sockfd);
		cout<<"ended"<<endl;
	}
	
	//sleep(3);
}
