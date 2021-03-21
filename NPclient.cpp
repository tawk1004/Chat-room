#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>
#include<unistd.h>
using namespace std;

int main(int argc,char** argv){
	if(argc!=3){
		cout<<"Usage: ./client <SERVER IP> <SERVER PORT>"<<endl;
		return 0;
	}
	struct addrinfo hints;
	struct addrinfo *servinfo; // 將指向結果
	memset(&hints, 0, sizeof hints); // 確保 struct 為空
	hints.ai_family = AF_UNSPEC; // 不用管是 IPv4 或 IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	 
	
	int clientfd = socket(AF_INET,SOCK_STREAM,0);
	if(clientfd<0){
		cout<<"ERROR: client socket built error."<<endl;
	}
	struct hostent *he;
	if((he=gethostbyname(argv[1])) == NULL)
	{
		cout << "error" << endl;
		exit(1);
	}
	struct sockaddr_in server;
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr = *((struct in_addr *)he->h_addr);
	server.sin_port = htons(atoi(argv[2]));
	
	/*if(inet_pton(AF_INET,host->h_addr,&server.sin_addr)<0){
		cout<<"ERROR: socket address translated error."<<endl;
		return 0;
	}*/
	/*
	int host;
	if ((host = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
    cout<<"getaddrinfo: "<< gai_strerror(host)<<endl;
    return 1;
	}*/
	if(connect(clientfd,(struct sockaddr *)&server,sizeof(server))<0){
		cout<<"ERROR: socket connection error."<<endl;
		return 0;
	}
	
	fd_set read_set;
	FD_ZERO(&read_set);
	char recv_from_user[100];
	char recv_from_server[100];
	int max = 0;
	while(1){
		FD_SET(STDIN_FILENO,&read_set);
		FD_SET(clientfd,&read_set);
		if(STDIN_FILENO>clientfd) max = STDIN_FILENO;
		else max = clientfd;
		select(max+1,&read_set,NULL,NULL,NULL);
		
		if(FD_ISSET(clientfd,&read_set)){ //from server
			bzero(recv_from_server,sizeof(recv_from_server));
			int if_recv = read(clientfd,recv_from_server,sizeof(recv_from_server));
			if(if_recv<0){
				cout<<"ERROR: read from server error."<<endl;
			}
			if(if_recv==0){
				return 0;
			}
			else{
				cout<<recv_from_server<<endl;
			}
		}
		else if(FD_ISSET(STDIN_FILENO,&read_set)){ //from user
			bzero(recv_from_user,sizeof(recv_from_user));
			read(STDIN_FILENO,recv_from_user,sizeof(recv_from_user));
			if(!strcmp(recv_from_user,"exit")){
				close(clientfd);
				FD_CLR(clientfd,&read_set);
				return 0;
			}
			write(clientfd,recv_from_user,strlen(recv_from_user));
		}
	}
	return 0;
	
	
}