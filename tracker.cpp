#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <math.h>
#include <map>
#include <vector>
#include <openssl/sha.h>

using namespace std;
#define BUFF_SIZE 1024
#define CHUNK_SIZE 1024*512
#define ll long long

int logout=0;
char ipt[20];
char ipt1[20];
int portt,portt1;

class UseridInfo
{
public:
	char uname[50];
	char pass[50];
	char ip[20];
	int port;
};

class trackerinfo
{
public:
	char ip[20];
	int port;
};

class fileinfo
{
public:
	string sha1;
	int size;
};


class PeerInfo
{
public:
	char ip[20];
	int port;
	vector<char> chunksarray;
};

int users=0;
int groups=0;
int currentuser=-1;
int currentgroup=-1;

map<int,UseridInfo> mapuserinfo;               //map uid to its name and password
map<string,fileinfo> mfile;                       //map file to file information
map<string,vector<PeerInfo>> mpeer;				//map file to no. of peer info
//map<int,PeerInfo> mapclients;				//map uid to client info
map<int,int> mapgroupids;					//map group id to uid (here user id is ownner id)
map<int,string> mapgrptofile;				//map group to file

map<int,vector<int>> mapgrpidtouid;        //map grpid to all clients;

map<int,int> maplogin;                  // map uid to login status



void updateuserfile()
{
	FILE* fp=fopen("users.txt","a+");
	if(fp==NULL)
	{
		cout<<"Error\n";
		return;
	}

	for(auto i=mapuserinfo.begin();i!=mapuserinfo.end();i++)
	{
			int u=i->first;
			UseridInfo &UiI=i->second;
			fprintf(fp,"%d\n",u);
			fprintf(fp,"%s\n",UiI.uname);
			fprintf(fp,"%s\n",UiI.pass);
			fprintf(fp,"%s\n",UiI.ip); 
			fprintf(fp,"%d\n",UiI.port); 
	}
	fclose(fp);
}

void* filehandle(void* args)
{
	int sockfd = *((int*)args);
	int flag;

	//cout<<flag<<"-----------\n";
	// if(flag==3)
	// {
	// 	int logout=1;
	// 	send(sockfd,&logout,sizeof(logout),0);
	// }

	recv(sockfd, &flag, sizeof(flag), 0);

	if(flag==6)		//create group
	{
		groups++;
		mapgroupids.insert(make_pair(groups,currentuser));
		vector<int> v;
		v.push_back(currentuser);
		mapgrpidtouid.insert(make_pair(groups,v));
	}

	if(flag==7)		//join group
	{


	}

	if(flag==8)		//leave group
	{

	}

	if(flag==9)		//list_groups
	{

	}

	if(flag==10)	//list files in group
	{
		//recv(gid);
		int sze=mpeer.size();
		send(sockfd,&sze,sizeof(sze),0);
		for(auto i=mpeer.begin();i!=mpeer.end();i++)
		{
			char name[50];
			strcpy(name,i->first.c_str());
			send(sockfd,name,sizeof(name),0);
		}

	}


	if(flag==11)	//list pending requests
	{

	}

	if(flag==12)		//Accept Group Joining Request
	{

	}

	if(flag==13)		//Show_downloads
	{

	}

	if(flag==3)		//logout
	{
		maplogin[currentuser]=0;
		currentuser=-1;
		send(sockfd,&currentuser,sizeof(currentuser),0);
		return NULL;
	}


	if(flag==4) // create user
	{
		users++;
		char uname[50];
		char pass[50];
		char ip[20];
		int port;
		int ack=0;
		recv(sockfd,&port,sizeof(port),0);
		recv(sockfd,ip,sizeof(ip),0);
		send(sockfd,&ack,sizeof(ack),0);
		recv(sockfd,uname,sizeof(uname),0);
		send(sockfd,&ack,sizeof(ack),0);
		recv(sockfd,pass,sizeof(pass),0);
		send(sockfd,&ack,sizeof(ack),0);
		cout<<ip<<" "<<port<<" "<<uname<<" "<<pass<<endl;
		UseridInfo UiI;
		strcpy(UiI.uname,uname);
		strcpy(UiI.pass,pass);
		strcpy(UiI.ip,ip);
		UiI.port=port;
		mapuserinfo.insert(make_pair(users,UiI));
	}

	if(flag==5) // login user
	{
		char uname[50];
		char pass[50];
		char ip[20];
		int port;
		int ack=0;
		recv(sockfd,&port,sizeof(port),0);
		recv(sockfd,ip,sizeof(ip),0);
		send(sockfd,&ack,sizeof(ack),0);
		recv(sockfd,uname,sizeof(uname),0);
		send(sockfd,&ack,sizeof(ack),0);
		recv(sockfd,pass,sizeof(pass),0);
		
		cout<<ip<<" "<<port<<" "<<uname<<" "<<pass<<endl;

		for(auto i=mapuserinfo.begin();i!=mapuserinfo.end();i++)
		{
			int u=i->first;
			UseridInfo &UiI=i->second;
			UiI.port=port;
			strcpy(UiI.ip,ip);
			if(strcmp(UiI.uname,uname)==0 && strcmp(UiI.pass,pass)==0)
			{
				if(maplogin.find(u)==maplogin.end())
				{
					maplogin.insert(make_pair(i->first,1));
					currentuser=i->first;
					int ack1=currentuser;
					send(sockfd,&ack1,sizeof(ack1),0);
					return NULL;
				}
				else
				{
					if(maplogin[u]==1)
					{
						int ack1=0;
						send(sockfd,&ack1,sizeof(ack1),0);
						return NULL;
					}
					else
					{
						maplogin[u]=1;
						currentuser=i->first;
						int ack1=currentuser;
						send(sockfd,&ack1,sizeof(ack1),0);
						return NULL;
					}
				}
			}
		}
		int ack1=-1;
		send(sockfd,&ack1,sizeof(ack1),0);
		return NULL;
	}

	if (flag==1)  //receive file information
	{
		// int pooort=ntohs(sockfd);
		// cout<<pooort<<endl;
		char ip[20];
		int port;
		int gid;
		char filename[50];
		memset(ip,'\0',sizeof(ip));
		memset(filename,'\0',sizeof(filename));
		int size; 
		int ack=0;
		recv(sockfd,&port,sizeof(port),0);
		recv(sockfd,&gid,sizeof(gid),0);
		send(sockfd,&ack,sizeof(ack),0);
		
		recv(sockfd,ip,sizeof(ip),0);
		send(sockfd,&ack,sizeof(ack),0);
		recv(sockfd,filename,sizeof(filename),0);
		send(sockfd,&ack,sizeof(ack),0);
		recv(sockfd,&size,sizeof(size),0);
		send(sockfd,&ack,sizeof(ack),0);

		int nsize=ceil(size*1.0/(CHUNK_SIZE));
		char a[nsize+1];
		memset(a,'\0',sizeof(a));
		recv(sockfd,a,nsize,0);
		send(sockfd,&ack,sizeof(ack),0);

		cout<<nsize<<endl;
		cout<<a<<endl;
		vector<char> vc;
		for(int i=0;i<nsize;i++)
		{
			vc.push_back(a[i]);
		}
		string s = ""; 
	    for (int i = 0; i < strlen(filename); i++) 
	    { 
	        s = s + filename[i]; 
	    } 
	    cout<<ip<<" "<<port<<" "<<filename<<" "<<size<<" \n";
	    cout<<s<<endl;
	    fileinfo f;
	    f.size=size;
	    f.sha1="aaaa";
		mfile.insert(make_pair(s,f));
		PeerInfo p1;
		strcpy(p1.ip,ip);
		p1.port=port;
		p1.chunksarray=vc;
		map<string,vector<PeerInfo>>::iterator itr=mpeer.find(s);
		if(itr!=mpeer.end())
		{
			itr->second.push_back(p1);
		}
		else
		{
			vector<PeerInfo> vp;
			vp.push_back(p1);
			mpeer.insert(make_pair(s,vp));
		}

		for(auto i=mfile.begin();i!=mfile.end();i++)
		{
			cout<<i->first<<" "<<i->second.size<<"\n";
		}

		for(auto i=mpeer.begin();i!=mpeer.end();i++)
		{
			cout<<i->first<<"\n";
			for(int k=0;k<i->second.size();k++)
			{
				vector<PeerInfo> &v=i->second;
				cout<<v[k].ip<<" "<<v[k].port<<endl;
				for(int i=0;i<v[k].chunksarray.size();i++)
					cout<<v[k].chunksarray[i];
				cout<<endl;
			}

		}

	}
	if(flag==2)                    //send file information
	{	
		char buff[50];
		memset(buff,'\0',50);
		recv(sockfd, buff, sizeof(buff), 0);
		// int pooort=ntohs(sockfd);
		// cout<<pooort<<endl;
		string s = ""; 
	    for (int i = 0; i < strlen(buff); i++) 
	    { 
	        s = s + buff[i]; 
	    } 
	    cout<<s<<endl;
	    fileinfo &mf1=mfile[s];
	    vector<PeerInfo>  &mp1 = mpeer[s];
	    int peers=mp1.size();
	    int size=mf1.size;
	    send(sockfd,&peers,sizeof(peers),0);
	    send(sockfd,&size,sizeof(size),0);

		for(int i=0;i<mp1.size();i++)
		{
			int port=mp1[i].port;
			char ip[20];
			int ack=0;
			strcpy(ip,mp1[i].ip);
			cout<<ip<<" "<<port<<endl;
			send(sockfd,&port,sizeof(port),0);
			recv(sockfd,&ack,sizeof(ack),0);
			send(sockfd,ip,strlen(ip),0);
			recv(sockfd,&ack,sizeof(ack),0);
		}

		cout<<"File info sent \n";

		char ip[20];
		int port;
		int gid;
		char filename[50];
		memset(ip,'\0',sizeof(ip));
		memset(filename,'\0',sizeof(filename));
		size; 
		int ack=0;
		recv(sockfd,&port,sizeof(port),0);
		recv(sockfd,&gid,sizeof(gid),0);
		send(sockfd,&ack,sizeof(ack),0);
		
		recv(sockfd,ip,sizeof(ip),0);
		send(sockfd,&ack,sizeof(ack),0);
		recv(sockfd,filename,sizeof(filename),0);
		send(sockfd,&ack,sizeof(ack),0);
		recv(sockfd,&size,sizeof(size),0);
		send(sockfd,&ack,sizeof(ack),0);

		int nsize=ceil(size*1.0/(CHUNK_SIZE));
		char a[nsize+1];
		memset(a,'\0',sizeof(a));
		recv(sockfd,a,nsize,0);
		send(sockfd,&ack,sizeof(ack),0);

		cout<<nsize<<endl;
		cout<<a<<endl;
		vector<char> vc;
		for(int i=0;i<nsize;i++)
		{
			vc.push_back(a[i]);
		}
		s = ""; 
	    for (int i = 0; i < strlen(filename); i++) 
	    { 
	        s = s + filename[i]; 
	    } 
	    cout<<ip<<" "<<port<<" "<<filename<<" "<<size<<" \n";
	    cout<<s<<endl;
	    fileinfo f;
	    f.size=size;
	    f.sha1="aaaa";
		mfile.insert(make_pair(s,f));
		PeerInfo p1;
		strcpy(p1.ip,ip);
		p1.port=port;
		p1.chunksarray=vc;
		map<string,vector<PeerInfo>>::iterator itr=mpeer.find(s);
		if(itr!=mpeer.end())
		{
			itr->second.push_back(p1);
		}
		else
		{
			vector<PeerInfo> vp;
			vp.push_back(p1);
			mpeer.insert(make_pair(s,vp));
		}
		cout<<"file info updated in tracker\n";
		pthread_exit(NULL);
	}
	close(sockfd);
	cout<<"connection request completed\n";
}

void* requestquit(void* cargs)
{
	string s1;
	cout<<"Request part\n";
	while(1)
	{
		cin>>s1;
		if(s1=="quit")
		{
			cout<<"Tracker Ended\n";
			exit(0);
		}
	}
}


void* listener(void* sargs)
{
	//int port = *((int*)sargs);
	trackerinfo* tti = (trackerinfo*)sargs;
	int port=tti->port;
	char ip[20];
	strcpy(ip,tti->ip);

	cout<<ip<<" "<<port<<endl;

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
		cout<<"port is already in use\n";
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
	    	break;
	}
}

int main(int argc,char** argv)
{
	if(argc<3)
	{
		cout<<"please enter tracker number \n";
		exit(1);
	}

	cout<<argv[1]<<" "<<argv[2]<<endl;

	int tno=atoi(argv[2]);
	FILE* frt = fopen(argv[1],"r");
	if(frt==NULL)
	{
		cout<<"File doesn't exist\n";
		return 1;
	}

	size_t length=0;
	char* bufname=NULL;
	
	int n;
	n = getline(&bufname, &length, frt);
	strcpy(ipt,bufname);
	n = getline(&bufname, &length, frt);
	portt=atoi(bufname);
	ipt[strlen(ipt)-1]='\0';

	n = getline(&bufname, &length, frt);
	strcpy(ipt1,bufname);
	n = getline(&bufname, &length, frt);
	portt1=atoi(bufname);
	ipt1[strlen(ipt1)-1]='\0';

	cout<<"Tracker 1 : "<<ipt<<" "<<portt<<endl;
	cout<<"Tracker 2 : "<<ipt1<<" "<<portt1<<endl;

	trackerinfo ti;

	if(tno==1)
	{
		strcpy(ti.ip,ipt);
		ti.port=portt;
	}
	else if(tno==2)
	{
		strcpy(ti.ip,ipt1);
		ti.port=portt1;
	}
	else
	{
		cout<<"please enter valid tracker number i.e. 1 or 2\n";
		exit(1);
	}

	// cout<<"Enter Port : ";
	// cin>>port;
	// char ip[20]="127.0.0.1";
	// PeerInfo k1,k2,k3;
	// strcpy(k1.ip,ip);
	// strcpy(k2.ip,ip);
	// strcpy(k3.ip,ip);
	// k1.port=9304;
	// k2.port=9305;
	// k3.port=9306;

	// vector<PeerInfo> v1;
	// v1.push_back(k1);
	// v1.push_back(k2);
	// v1.push_back(k3);

	// vector<PeerInfo> v2;
	// v2.push_back(k1);
	// v2.push_back(k2);

	// mpeer.insert({"D.St6",v1});
	// mpeer.insert({"testwhilw.cpp",v2});


	// fileinfo f1,f2;
	// f1.size=137129573;
	// f2.size=143;

	// mfile.insert({"D.St6",f1});
	// mfile.insert({"testwhilw.cpp",f2});

	pthread_t threadid1,threadid2;
	int err1,err2;
	err1 = pthread_create(&threadid1, NULL, requestquit,NULL);

	if (err1)
	{
	    printf("Error occured during client thread creation 1 : %d", err1);
	    exit(1);
	}

	err2 = pthread_create(&threadid2, NULL, listener,(void*)&ti);
	if (err2)
	{
	    printf("Error occured during server thread creation 2 : %d", err2);
	    exit(1);
	}
	pthread_join(threadid2,NULL);
	cout<<"Bye\n";
	return 0;
}