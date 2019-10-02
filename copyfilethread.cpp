#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
using namespace std;

#define BUFF_SIZE 512

int main()
{
	FILE *fps = fopen ( "/home/shubham/Music/Detective_Conan.mp4"  , "rb" );
	string s;
	char filename[50];
	cout<<"Enter file name : ";
	cin>>s;
	strcpy(filename,s.c_str());

	FILE* fpd = fopen(filename,"wb");

	fseek ( fps , 0 , SEEK_END);
	int size = ftell ( fps );
	rewind ( fps );


	pthread_t threadid1,threadid2;
	int err1,err2;
	struct filerange
	{
		int size;
		int start;
		int end;
	}f1,f2;
	f1.size=size;
	f2.size=size;
	fseek ( fps , 0 , SEEK_END);
	int size = ftell ( fps );
	rewind ( fps );
	f1.start=

	
	err1 = pthread_create(&threadid1, NULL, filecopy,NULL);

	if (err1)
	{
	    printf("Error occured during client thread creation 1 : %d", err1);
	    exit(1);
	}

	err2 = pthread_create(&threadid2, NULL, filecopy,(void*)&port);

	if (err2)
	{
	    printf("Error occured during server thread creation 2 : %d", err2);
	    exit(1);
	}

	pthread_join(threadid1,NULL);
	pthread_join(threadid2,NULL);

	cout<<"Bye\n";

	char Buffer [BUFF_SIZE]; 
	memset(Buffer,'\0',512);

	int fsize=size/512;
	cout<<fsize<<endl;
	while(fsize>0)
	{
		fwrite(Buffer,1,512,fpd);
		fsize--;
	}
	fsize=size%512;
	cout<<fsize<<endl;
	char ch='\0';
	while(fsize>0)
	{
		fwrite(&ch,1,1,fpd);
		fsize--;
	}


	fsize=size;
	int n;
	rewind ( fpd );
	while ( ( n = fread( Buffer , sizeof(char) , BUFF_SIZE , fps ) ) > 0  && fsize > 0 )
	{
		fwrite(Buffer,1,n,fpd);
		fsize--;
	}
	fclose ( fps );
	fclose (fpd);
	return 0;
}