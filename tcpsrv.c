/*Written  BY  WangQi and  LiSongTing . All Rights Reserved*/
#include	"unp.h"
#include	<time.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>

#define MAXSIZE 100

double calculate(char str[]);
int level(char op);
double calvalue(double od1, double od2, char tempop);
void getByteorder(char* srvOrder);
void Compare(char*cli,char*srv,char*output);
void handle_srv(int sockfd);
void function1(int sockfd);
void function2(int sockfd);
void function3(int sockfd);
int	CLI_NUM=0;
int main(int argc, char **argv)
{
	int			 listenfd, connfd,childpid;
	socklen_t		 clilen;
	struct sockaddr_in	 servaddr, cliaddr;
	char			 buff[MAXLINE],srvOrder[MAXLINE],receive[MAXLINE],outputOrder[MAXLINE];
	time_t			 ticks;           	 
	char*			 note;
	ssize_t			n;
	struct hostent 		* hptr;
	
	note="\n------------------------------\nPlease select functions:\n1.PrintBack Function\n2.Calculate Function\n3.File Transmission\n4.exit\n------------------------------\n";
		
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(13);	

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	
	Listen(listenfd, LISTENQ);
	getByteorder(srvOrder);
	
	printf("Server  Started...\n");
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		
		if(CLI_NUM<2){
			CLI_NUM++;	
		}else{
			if(connfd>=0){
				Write(connfd,"Client number is limited,Please try again later\n",strlen("Client number is limited,Please try again later\n"));
				Close(connfd);
			}
			continue;
		}
		if((childpid=Fork())==0){
			Close(listenfd);	
			while((n=Read(connfd,buff,MAXLINE))>0){
				hptr = gethostbyaddr(&cliaddr.sin_addr,4,AF_INET);
				printf("Host Name of Client:%s\n\n",hptr->h_name);
				printf("Client IP Address:%s, port:%d\n\n",Inet_ntop(AF_INET,&cliaddr.sin_addr,receive,sizeof(receive)),ntohs(cliaddr.sin_port));
				Compare(buff,srvOrder,outputOrder);
				Writen(connfd,outputOrder,strlen(outputOrder));//output byteorder
				ticks = time(NULL);
				snprintf(buff, sizeof(buff), "\n%.24s\r\nconnection success!\n", ctime(&ticks));
				printf("A client connected at:%.24s\r\n\n",ctime(&ticks));
				Write(connfd,buff,strlen(buff));	
				Write(connfd,note,strlen(note));		
				handle_srv(connfd);			
			}
			if(n==0){
				Close(connfd);
				err_quit("Client disconnected\n");
			}
		}
	}
}
void getByteorder(char* srvOrder)
{
	union {
	  short  s;
      char   c[sizeof(short)];
    } un;
	un.s = 0x0102;
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2){
			sprintf(srvOrder,"%s","big-endian");	
		}
		else if (un.c[0] == 2 && un.c[1] == 1){
			sprintf(srvOrder,"%s","little-endian");
		}
		else
			printf("unknown\n");
	} else
		printf("sizeof(short) = %d\n", (int)sizeof(short));
}
void Compare(char*cli,char*srv,char*output){
	
	if(0==strcmp(cli,srv)){
		sprintf(output,"%s%s%s","The server is ",srv,",byteorder is same.\n");
	}else{	
		sprintf(output,"%s%s%s","The server is ",srv,",byteorder is different.\n");
	}
}
void handle_srv(int sockfd){
	ssize_t  n;
	char     rvbuf[MAXLINE];
	int i;
again:
	while((n=read(sockfd,rvbuf,MAXLINE))>0){
		if(rvbuf[0]=='1'){
			printf("Client selected function:%c\n",rvbuf[0]);
			function1(sockfd);
		}else if(rvbuf[0]=='2'){
			printf("Client selected function:%c\n",rvbuf[0]);			
			function2(sockfd);
		}else if(rvbuf[0]=='3'){
			printf("Client selected function:%c\n",rvbuf[0]);
			function3(sockfd);
		}else if(rvbuf[0]=='4'){
			printf("Client selected function:%c\n",rvbuf[0]);
			if(CLI_NUM>0){
				CLI_NUM--;
				Close(sockfd);
				err_quit("Client disconnected\n");
			}				
			break;
		}else{
			printf("error function\n");
		}
		for(i=0;i<MAXLINE;i++) rvbuf[i]='\0';		
	}
	if(n==0){
		CLI_NUM--;
		Close(sockfd);
		err_quit("Client disconnected\n");	
	}
	if(n<0&&errno==EINTR){
		goto again;	
	}else if(n<0){
		err_sys("Read error.no Infomation \n");
	}
}
void function2(int sockfd){
	char  rcv[MAXLINE];
	char  snd[MAXLINE];
	int n;
	double result;
	int length;
	int maxfdp1,i;
	fd_set rset;
	
	FD_ZERO(&rset);
	for(;;){
		FD_SET(sockfd,&rset);
		
		maxfdp1 = sockfd +1;
		Select(maxfdp1,&rset,NULL,NULL,NULL);
		if(FD_ISSET(sockfd,&rset)){
			if((n=read(sockfd,rcv,MAXLINE))>0){
				if(rcv[0]=='!'){
					for(i=0;i<MAXLINE;i++) rcv[i]='\0';	
					printf("Client exit current function,enter Menu\n");
					break;
				}else{
					length = strlen(rcv);
					rcv[length-1] ='\0';
					printf("received from client:%s  length:%d\n",rcv,(int)strlen(rcv));
					result = calculate(rcv);
					printf("result:%.3f\n",result);
					sprintf(snd,"%f",result);
					Writen(sockfd,snd,strlen(snd));
					for(i=0;i<n;i++) rcv[i]='\0';
				}			
			}else if(n==0){
				CLI_NUM--;
				printf("Current CLI_NUM:%d\n",CLI_NUM);
				Close(sockfd);
				err_quit("Client disconnected\n");
			}else if(n<0&&errno==EINTR){
				printf("Terminated\n");
				return;	
			}	
		}

	}
}
void function3(int sockfd){
	ssize_t  n;
	char rcv[MAXLINE];
	FILE * fprcv=NULL;
	char *postfix=".txt";
	char *fname=NULL;
	int  flag=0,i;
again:
	for(i=0;i<MAXLINE;i++) rcv[i]='\0';
	while((n=read(sockfd,rcv,MAXLINE))>0){
		if(rcv[0]=='!') {
			for(i=0;i<MAXLINE;i++) rcv[i]='\0';	
			printf("Client exit current function,enter Menu\n");
			break;
		}
		if(((flag =strcspn(rcv,postfix))>0)&&(fname==NULL)){
			printf("flag:%d\n",flag);
			printf("fname:%s  length:%d\n",rcv,strlen(rcv));
			fname = &rcv;//important			
			fprcv = fopen(fname,"w");
		}else if(fname!=NULL){
			printf("write to file:%s\n",rcv);
			Fputs(rcv,fprcv);
			fflush(fprcv);
			break;
		}	
		for(i=0;i<MAXLINE;i++) rcv[i]='\0';
		//n=0;			
	}if(n==0){
		CLI_NUM--;
		printf("Current CLI_NUM:%d\n",CLI_NUM);
		Close(sockfd);
		err_quit("Client disconnected\n");	
	}else if(n<0&&errno==EINTR){
		goto again;
	}else if(n<0){
		err_sys("Read error.no Infomation \n");
	}
	close(fprcv);
}
void function1(int sockfd)
{
	ssize_t n;
	char rv[MAXLINE];
	int i=0;
again:
	while((n=read(sockfd,rv,MAXLINE))>0){
		if(rv[0]=='!'){
			for(i=0;i<MAXLINE;i++) rv[i]='\0';	
			printf("Client exit current function,enter Menu\n");
			break;
		}
		Writen(sockfd,rv,n);
		printf("%s",rv);
		for(i=0;i<n;i++) rv[i]='\0';
	}
	if(n==0){
		CLI_NUM--;
		printf("Current CLI_NUM:%d\n",CLI_NUM);
		Close(sockfd);
		err_quit("Client disconnected\n");		
	}else if(n<0&&errno==EINTR){
		goto again;	
	}else if(n<0){
		err_sys("Read error.no Infomation \n");
	}
}
double calculate(char str[]) {
	struct {
		char data[MAXSIZE];
		int top;
	}op;
	struct {
		double data[MAXSIZE];
		int top;
	}od;
	char ch;
	char tempch[20];
	int j = 0, t;
	double d;
	double od1, od2;
	char tempop;
	int length = strlen(str);
	op.top = -1;
	od.top = -1;
	while (j < length) {
		ch = str[j];
		if ((ch >= '0'&&ch <= '9') || ch == '.') {
			d = 0;
			t = 0;
			while ((ch >= '0'&&ch <= '9') || ch == '.') {
				tempch[t] = ch;
				t++;
				j++;
				ch = str[j];
			}
			tempch[t] = '\0';
			d = atof(tempch);
			od.top++;
			od.data[od.top] = d;
		}
		else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '(' || ch == ')') {
			if (ch == '(') {
				op.top++;
				op.data[op.top] = ch;
			}
			else if (ch == ')') {
				while (level(op.data[op.top]) != -1) {
					od2 = od.data[od.top];
					od.top--;
					od1 = od.data[od.top];
					tempop = op.data[op.top];
					op.top--;
					od.data[od.top] = calvalue(od1, od2, tempop);
					if (op.top == -1)
						break;
				}
				op.top--;
			}
			else if (op.top == -1) {
				op.top++;
				op.data[op.top] = ch;
			}
			else if (level(ch) > level(op.data[op.top])) {
				op.top++;
				op.data[op.top] = ch;
			}
			else {
				while (level(ch) <= level(op.data[op.top])) {
					od2 = od.data[od.top];
					od.top--;
					od1 = od.data[od.top];
					tempop = op.data[op.top];
					op.top--;
					od.data[od.top] = calvalue(od1, od2, tempop);
					if (op.top == -1)
						break;
				}
				op.top++;
				op.data[op.top] = ch;
			}
			j++;
		}
	}
	while (op.top != -1) {
		od2 = od.data[od.top];
		od.top--;
		od1 = od.data[od.top];
		tempop = op.data[op.top];
		op.top--;
		od.data[od.top] = calvalue(od1, od2, tempop);
	}
	return od.data[od.top];
}

int level(char op) {
	if (op == '+' || op == '-')
		return 1;
	else if (op == '*' || op == '/' || op == '%')
		return 2;
	else if (op == '(')
		return -1;
	else
		return -3;
}

double calvalue(double od1, double od2, char tempop) {
	switch (tempop) {
	case'+':
		return od1 + od2;
	case'-':
		return od1 - od2;
	case'*':
		return od1*od2;
	case'/':
		return od1 / od2;
	}
	return 0;
}
/*Written  BY  WangQi and  LiSongTing . All Rights Reserved*/
