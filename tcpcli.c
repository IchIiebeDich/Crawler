/*Written  BY  WangQi and  LiSongTing . All Rights Reserved*/
#include	"unp.h"
void getByteOrder(char* order);
void handle_cli(FILE*fp,int sockfd);
void f1Send(FILE* fp,int sockfd);
void f2Send(FILE *fp,int sockfd);
void f3Send(FILE *fp,int sockfd);
struct timeval timeout;
time_t		ticks;
int main(int argc, char **argv)
{
	int					sockfd;
	struct				sockaddr_in	servaddr;
	char				cliOrder[MAXLINE];
	timeout.tv_sec=10;
	timeout.tv_usec=0;
	if (argc != 2)
		err_quit("usage: a.out <IPaddress>");

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(13);	/* server port*/
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("connect error.Connection refused.");
	getByteOrder(cliOrder);
	printf("This host is :%s\n",cliOrder);
	Writen(sockfd,cliOrder,strlen(cliOrder));

	handle_cli(stdin,sockfd);
		
}
void getByteOrder(char* order){
	union {
	  short  s;
      char   c[sizeof(short)];
    } un;
	un.s = 0x0102;
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2){
			sprintf(order,"%s","big-endian");	
		}
		else if (un.c[0] == 2 && un.c[1] == 1){
			sprintf(order,"%s","little-endian");	
		}
		else
			printf("unknown\n");
	} else
		printf("sizeof(short) = %d\n", (int)sizeof(short));
}

void f2Send(FILE *fp,int sockfd){
	char  snd[MAXLINE];
	char  rcv[MAXLINE];
	int maxfdp1,n;
	fd_set  rset;
	//use IO multiplexing 
	FD_ZERO(&rset);
	printf("-----------------\nPlease input expression with'+ - / *':(tpye'!' to exit this function)\n");
	for(;;){	
		FD_SET(sockfd,&rset);
		FD_SET(fileno(fp),&rset);
		maxfdp1 = max(fileno(fp),sockfd)+1;
		Select(maxfdp1,&rset,NULL,NULL,NULL);

		if(FD_ISSET(sockfd,&rset)){
			if((n=Read(sockfd,rcv,MAXLINE))==0){
				err_quit("Server terminated prematurely\n");
				
			}else if(n>0){
				printf("=%s\n",rcv);
				//Fputs(rcv,stdout);   error,,,I dont know why...
				continue;
			}else{
				printf("n<0\n");
			}
				
		}
		if(FD_ISSET(fileno(fp),&rset)){
			if(Fgets(snd,MAXLINE,fp)==NULL){
				continue;
			}else if(snd[0]=='!'){				
				Writen(sockfd,snd,strlen(snd));				
				break;
			}else{
				Writen(sockfd,snd,strlen(snd));	
			}			
		}
	}	
}
void f3Send(FILE *fp,int sockfd){
	FILE * fp1;
	char   snd[MAXLINE];
	char   fp_name[MAXLINE];
	char	rcv[MAXLINE];
	int maxfdp1,n,m;
	fd_set  rset;
	FD_ZERO(&rset);
	printf("-----------------\nPlease input the Name of file at current path:(tpye'!' to exit this function)\n");
	for(;;){
		FD_SET(sockfd,&rset);
		FD_SET(fileno(fp),&rset);
		maxfdp1 = max(fileno(fp),sockfd)+1;
		Select(maxfdp1,&rset,NULL,NULL,NULL);
	
		if(FD_ISSET(sockfd,&rset)){
			if((n=Read(sockfd,rcv,MAXLINE))==0){
				err_quit("Server terminated prematurely\n");		
			}else if(n>0){
				printf("=%s\n",rcv);
				//Fputs(rcv,stdout);   error,,,I dont know why...
				continue;
			}else{
				printf("n<0\n");
			}
			
		}
		if(FD_ISSET(fileno(fp),&rset)){
			if(Fgets(fp_name,MAXLINE,fp)==NULL){
				continue;
			}else if(fp_name[0]=='!'){				
				Writen(sockfd,fp_name,strlen(fp_name));				
				break;
			}else{
				m = strlen(fp_name);
				fp_name[m-1]='\0';
				fp1 = fopen(fp_name,"r");
				if(fp1==NULL) {
					printf("Warning:File is not exist\n");
					return;

				}else{
					Writen(sockfd,fp_name,strlen(fp_name));
					while(Fgets(snd,MAXLINE,fp1)!=NULL){
						Writen(sockfd,snd,strlen(snd));
					}
					printf("Transmission Success!\n");
					fclose(fp1);
					break;
				}
			}			
		}

	}
}
void f1Send(FILE*fp,int sockfd){
	char   snd[MAXLINE],rcv[MAXLINE];
	int maxfdp1,n,i;
	fd_set  rset;

	FD_ZERO(&rset);
	for(;;){	
		FD_SET(sockfd,&rset);
		FD_SET(fileno(fp),&rset);
		maxfdp1 = max(fileno(fp),sockfd)+1;
		Select(maxfdp1,&rset,NULL,NULL,NULL);

		if(FD_ISSET(sockfd,&rset)){
			if((n=Read(sockfd,rcv,MAXLINE))==0){
				err_quit("Server terminated prematurely\n");
				
			}else if(n>0){
				//printf("=%s\n",rcv);
				Fputs(rcv,stdout);  	
				continue;
			}else{
				printf("n<0\n");
			}
				
		}
		if(FD_ISSET(fileno(fp),&rset)){
			if(Fgets(snd,MAXLINE,fp)==NULL){
				continue;
			}else if(snd[0]=='!'){				
				Writen(sockfd,snd,strlen(snd));				
				break;
			}else{
				for(i=0;i<n;i++) rcv[i]='\0';
				Writen(sockfd,snd,strlen(snd));
			}			
		}
	}
}

void handle_cli(FILE* fp,int sockfd){	
	int maxfdp1,stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;
	int  out;

	stdineof =0;
	FD_ZERO(&rset);
	for(;;){
		if (stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		out = Select(maxfdp1, &rset, NULL, NULL, &timeout);
		if(out==0){
			ticks = time(NULL);
			printf("Disconnect at:%.24s\r\n",ctime(&ticks));
			err_quit("Time Out! 10 seconds passed.\n");
		}
		/* socket is readable */
		if (FD_ISSET(sockfd, &rset)) {	
			if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
				if (stdineof == 1)
					return;		/* normal termination */
				else
					err_quit("Server terminated prematurely");
			}

			Write(fileno(stdout), buf, n);
		}
		 /* std input is readable */
		if (FD_ISSET(fileno(fp), &rset)) { 
			
			if ( (n = Read(fileno(fp), buf, MAXLINE)) == 0) {
				stdineof = 1;
				Shutdown(sockfd, SHUT_WR);	/* send FIN */
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			if(buf[0]=='1'){
				Writen(sockfd,buf,MAXLINE);
				printf("input some strings.input'!'to exit this function\n");
				f1Send(stdin,sockfd);
			}else if(buf[0]=='2'){
				Writen(sockfd,buf,MAXLINE);
				f2Send(stdin,sockfd);
			}else if(buf[0]=='3'){
				Writen(sockfd,buf,MAXLINE);
				f3Send(stdin,sockfd);
			}else if(buf[0]=='4'){
				exit(0);
			}
			else{
				printf("Please select right function\n");
			}
			printf("--------------------\nSelect Functions:1-4:\n");
		}
	}


}
/*Written  BY  WangQi and  LiSongTing . All Rights Reserved*/
