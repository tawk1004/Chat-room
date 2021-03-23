#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<ctype.h>
#include<iostream>
#include<sys/select.h>
#include<unistd.h>
#include<netdb.h>
#include<errno.h>
#include<arpa/inet.h>
using namespace std;

struct user{
	char name[100];
	char ip[100];
	int port;
	int fd;
};

int main(int argc,char** argv){
	//creat socket
	if(argc!=2){
        printf("Usage: ./server <SERVER PORT>\n");
        return 0;		
	}
	
	int listenfd;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd<0) cout<<"Fail to creat socket"<<endl;
	
	struct sockaddr_in severInfo,clientInfo;
	bzero(&severInfo,sizeof(severInfo));
	//bzero(&clientInfo,sizeof(clientInfo));
	severInfo.sin_family = AF_INET;
	severInfo.sin_addr.s_addr = INADDR_ANY;
	severInfo.sin_port = htons(atoi(argv[1]));
	
	bind(listenfd,(struct sockaddr *)&severInfo,sizeof(severInfo));
	
	if(listen(listenfd,100)<0) cout<<"listen socket error"<<endl;
	
	int forClient = 0,ready_num = 0,i;
	fd_set all_set,read_set;
	FD_ZERO(&all_set);
	FD_SET(listenfd,&all_set);
	int maxfd = listenfd;
	char buffer[100];
	int maxi = -1;
	int j;
	
	struct user client[101];
	for(i=0;i<100;i++){
		client[i].fd = -1;
	}
	while(1){
		read_set = all_set;
		ready_num = select(maxfd+1,&read_set,NULL,NULL,NULL);
		
		if(FD_ISSET(listenfd,&read_set)){
			bzero(&clientInfo,sizeof(clientInfo));
			socklen_t clientlen = sizeof(clientInfo);
			int connectfd = accept(listenfd,(struct sockaddr *)&clientInfo,&clientlen);
			for(i=0;i<100;i++){
				if(client[i].fd==-1){
					client[i].fd = connectfd;
					strcpy(client[i].name,"anonymous");
					strcpy(client[i].ip,inet_ntop(AF_INET,&clientInfo.sin_addr,client[i].ip,sizeof(client[i].ip)));
					client[i].port = ntohs(clientInfo.sin_port);
					break;
				}
			}
			
			if(i==100) {
				cout<<"too many clients"<<endl;
				return 0;
			}
			FD_SET(client[i].fd,&all_set);

			if(connectfd>maxfd) maxfd = connectfd;
			if(i>maxi) maxi = i;
			
			//hello 
			j=0;
			for(j=0;j<=maxi && maxi != -1;j++){
				if(client[j].fd == -1) continue;
				if(FD_ISSET(client[j].fd,&all_set)){
					if(client[j].fd != client[i].fd){
						bzero(buffer,sizeof(buffer));
						snprintf(buffer,sizeof(buffer),"[Server] Someone is coming!");
					}
						
					else{
						bzero(buffer,sizeof(buffer));
						snprintf(buffer,sizeof(buffer),"[Server] Hello, anonymous! From: %s/%d\n",client[j].ip,client[j].port);
					}
					write(client[j].fd,buffer,strlen(buffer));
				}
			}

		}
		//new over
		
		int sockfd;
		char recvwords[100];
		char recvwords2[100];
		for(i=0;i<=maxi;i++){
			if((sockfd = client[i].fd)<0) {
				continue;
			}
			if(FD_ISSET(sockfd,&read_set)){
				bzero(recvwords,sizeof(recvwords));
				bzero(recvwords2,sizeof(recvwords2));
				int recvlen = read(sockfd,recvwords,sizeof(recvwords));
				if(recvlen<0) continue;

				else if(recvlen==0){
					for(j=0;j<=maxi;j++){
						if(FD_ISSET(client[j].fd,&all_set)){
							if(client[j].fd != client[i].fd){
								bzero(buffer,sizeof(buffer));
								snprintf(buffer,sizeof(buffer),"[Server] %s is offline.\n",client[i].name);
								write(client[j].fd,buffer,strlen(buffer));
							}
						}
					}
					FD_CLR(sockfd,&all_set);
					close(sockfd);
					client[i].fd = -1;	
					ready_num--;
				}
				else{
					strcpy(recvwords2,recvwords);
					char *command = strtok(recvwords," \n");
					char *name;
					char *temp;
					
					
					if(!strcmp(command,"who")){ //who
						temp = strtok(NULL," \n");
						if(!temp){
							
							for(j=0;j<=maxi && maxi!=-1;j++){
								if(client[j].fd == -1) continue;
								if(FD_ISSET(client[j].fd,&all_set)){
									bzero(buffer,sizeof(buffer));
									if(client[j].fd != client[i].fd){
										snprintf(buffer,sizeof(buffer),"[Server] %s %s/%d\n",client[j].name,client[j].ip,client[j].port);
									}
									else{
										snprintf(buffer,sizeof(buffer),"[Server] %s %s/%d ->me\n",client[j].name,client[j].ip,client[j].port);
									}
									write(client[i].fd,buffer,strlen(buffer));
								}
							}
						}
						else{
							bzero(buffer,sizeof(buffer));
							snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n");
							write(client[i].fd,buffer,strlen(buffer));
						}
					}
					
					else if(!strcmp(command,"name")){ //name
						temp = strtok(NULL," \n");
						int change_name = 1;
						
						if(strlen(temp)!=0){
							bzero(buffer,sizeof(buffer));
							if(strlen(temp)<2 || strlen(temp)>12){
								snprintf(buffer,sizeof(buffer),"[Server] ERROR: Username can only consists of 2~12 English letters.\n");
								write(client[i].fd,buffer,strlen(buffer));	
								change_name = 0;
							}
							else{
								if(!strcmp(temp,"anonymous")){
									snprintf(buffer,sizeof(buffer),"[Server] ERROR: Username cannot be anonymous.\n",temp);
									write(client[i].fd,buffer,strlen(buffer));
									change_name = 0;
								}
								j=0;
								for(j=0;j<=maxi && maxi!=-1;j++){
										if(client[j].fd == -1) continue;
										if(FD_ISSET(client[j].fd,&all_set)){
											if(!strcmp(client[j].name,temp)){
												snprintf(buffer,sizeof(buffer),"[Server] ERROR: %s has been used by others.\n",temp);
												write(client[i].fd,buffer,strlen(buffer));
												change_name = 0;
											}
										}
								}
								j=0;
								for(j=0;j<strlen(temp);j++){
									if(temp[j]>='a' && temp[j]<='z');
									else if(temp[j]>='A' && temp[j]<='Z');
									else{
										snprintf(buffer,sizeof(buffer),"[Server] ERROR: Username can only consists of 2~12 English letters.\n");
										write(client[i].fd,buffer,strlen(buffer));
										change_name = 0;
										
									}
								}
								
								if(change_name==1){
									char old_name[100];
									bzero(old_name,sizeof(old_name));
									strcpy(old_name,client[i].name);
									bzero(client[i].name,sizeof(client[i].name));
									strcpy(client[i].name,temp);
								
									for(j=0;j<=maxi;j++){
										if(client[j].fd == client[i].fd){
											snprintf(buffer,sizeof(buffer),"[Server] You're now known as %s.\n",temp);
											write(client[j].fd,buffer,strlen(buffer));
										}
										else{
											snprintf(buffer,sizeof(buffer),"[Server] %s is now known as %s.\n",old_name,temp);
											write(client[j].fd,buffer,strlen(buffer));
										}
									}
									
									
								}
							}
							
						}
						
						else{
							bzero(buffer,sizeof(buffer));
							snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n");
							write(client[i].fd,buffer,strlen(buffer));
						}
					}
					
					else if(!strcmp(command,"tell")){
						temp = strtok(NULL," \n");
						if(strlen(temp)!=0){
							bzero(buffer,sizeof(buffer));
							if(!strcmp(client[i].name,"anonymous")){
								snprintf(buffer,sizeof(buffer),"[Server] ERROR: You are anonymous.\n");
								write(client[i].fd,buffer,strlen(buffer));
								continue;
							}
							else if(!strcmp(temp,"anonymous")){
								snprintf(buffer,sizeof(buffer),"[Server] ERROR: The client to which you sent is anonymous.\n");
								write(client[i].fd,buffer,strlen(buffer));
								continue;
							}
							j=0;
							for(j=0;j<=maxi &&maxi!=-1;j++){
								if(client[j].fd == -1) continue;
								if(FD_ISSET(client[j].fd,&all_set)){
									if(!strcmp(client[j].name,temp)) break;
								}
							}
							if(j>maxi){
								snprintf(buffer,sizeof(buffer),"[Server] ERROR: The receiver doesn't exist.\n");
								write(client[i].fd,buffer,strlen(buffer));
								continue;
							}
							int space = 0,counter = 0,msg = 0;
							char message[100];
							bzero(message,sizeof(message));
							for(counter=0;counter<recvlen-1;counter++){
								if(space>=2){
									message[msg] = recvwords2[counter];
									msg++;
								}
								else if(recvwords2[counter]==' ' && recvwords2[counter+1]==' ') ;
								else if(recvwords2[counter]==' ') space++;
							}
							
							snprintf(buffer,sizeof(buffer),"[Server] %s tell you %s",client[i].name,message);
							write(client[j].fd,buffer,strlen(buffer));
							bzero(buffer,sizeof(buffer));
							snprintf(buffer,sizeof(buffer),"[Server] SUCCESS: Your message has been sent.\n");
							write(client[i].fd,buffer,strlen(buffer));
						}
						else{
							bzero(buffer,sizeof(buffer));
							snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n");
							write(client[i].fd,buffer,strlen(buffer));
						}
					}
					
					else if(!strcmp(command,"yell")){
						
						char message[100];
						temp = strtok(NULL," \n");
						if(strlen(temp)!=0){
							bzero(message,sizeof(message));
							int counter=0,space=0,msg=0;
							for(counter=0;counter<recvlen-1 ;counter++){
								if(space>=1){
									message[msg] = recvwords2[counter];
									msg++;
								}
								
								else if(recvwords2[counter]==' ' && recvwords2[counter+1]==' ') ;
								else if(recvwords2[counter]==' ') space++;
							}
							
							bzero(buffer,sizeof(buffer));
							snprintf(buffer,sizeof(buffer),"[Server] %s yell %s\n",client[i].name,message);
							j=0;
							for(j=0;j<=maxi &&maxi!=-1;j++){
								if(client[j].fd == -1) continue;
								if(FD_ISSET(client[j].fd,&all_set)){
									write(client[j].fd,buffer,strlen(buffer));
								}
							}
						}
						else{
							bzero(buffer,sizeof(buffer));
							snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n");
							write(client[i].fd,buffer,strlen(buffer));
						}
					}
					
					else if(!strcmp(command,"exit")){
						
						temp = strtok(NULL," \n");
						if(!temp){
							j=0;
							for(j=0;j<=maxi && maxi!= -1;j++){
								if(client[j].fd == -1) continue;
								if(FD_ISSET(client[j].fd,&all_set)){
									if(client[j].fd != client[i].fd){
										bzero(buffer,sizeof(buffer));
										snprintf(buffer,sizeof(buffer),"[Server] %s is offline.\n",client[i].name);
										write(client[j].fd,buffer,strlen(buffer));
									}
								}
							}

							FD_CLR(sockfd,&all_set);
							close(sockfd);
							client[i].fd = -1;
							ready_num--;
						}
						else{
							bzero(buffer,sizeof(buffer));
							snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n");
							write(client[i].fd,buffer,strlen(buffer));
						}
					}
					else{
						bzero(buffer,sizeof(buffer));
						snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n");
						write(client[i].fd,buffer,strlen(buffer));
					}
				}

			}
		}
		
		
	}
	
}

