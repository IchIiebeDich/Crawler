#include	"unp.h"
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 1024

int getFileSize(char file_name[]){
	struct stat temp;
	stat(file_name, &temp);
	return temp.st_size;
}

void SendFile(int sockfd, char file_name[]){

	char buffer[BUFFER_SIZE];
	FILE *fp= fopen(file_name, "r");
	if(fp == NULL){
		printf("File:%s Not Found\n",file_name);
		return ;
	}
	else{
		printf("begin to send the file:%s\n",file_name);
		bzero(buffer, BUFFER_SIZE);
		int length = 0;
		int t = 1;

		while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0){
			if(send(sockfd, buffer, length, 0) <0){
				printf("Send File:%s Failed\n",file_name);
				break;
			}
			printf("%d\n",t++);
			bzero(buffer, BUFFER_SIZE);
		}

		fclose(fp);
		printf("File:%s Transfer Successful!\n",file_name);
	}
}

int
main(int argc, char **argv)
{	
	//tcpcli01.c
	int			sockfd;
	struct sockaddr_in	servaddr;
	//strcliselect02.c
	int			maxfdp1, stdineof;
	fd_set			rset;
	char			buf[MAXLINE], buf2[MAXLINE];
	int			n,l;
	//byteorder.c
	union {
	 	short  s;
   		char   c[sizeof(short)];
   	 } un;
   	 //name
   	 char 			name[30] = "zzbao,";
	 char* 			endian;
	 char* 			order1 = "-s";
	 time_t			ticks;

   	un.s = 0x0102;
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2)
			endian = "big-endian";
		else if (un.c[0] == 2 && un.c[1] == 1)
			endian = "little-endian";
		else
			endian = "unknown";
	} else
		printf("sizeof(short) = %d\n", sizeof(short));


	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

//	str_cli(stdin, sockfd);		/* do it all */

	char result[40];
	snprintf(result,sizeof(result),"%s%s",name,endian);
	Writen(sockfd, result, strlen(result));

	stdineof = 0;
	FD_ZERO(&rset);
	for ( ; ; ) {
		if (stdineof == 0)
			FD_SET(fileno(stdin), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(stdin), sockfd) + 1;

		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {	/* socket is readable */
			if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
				if (stdineof == 1)
					exit(0);		/* normal termination */
				else{
					ticks = time(NULL);
					printf("End time: %.24s\r\n",ctime(&ticks));
					exit(0);
				}
			}

			Write(fileno(stdout), buf, n);
			for(l = 0;l < MAXLINE;l++)
				buf[l] = '\0';
		}

		if (FD_ISSET(fileno(stdin), &rset)) {  /* input is readable */
			if ( (n = Read(fileno(stdin), buf, MAXLINE)) == 0) {
				stdineof = 1;
				Shutdown(sockfd, SHUT_WR);	/* send FIN */
				FD_CLR(fileno(stdin), &rset);
				continue;
			}

			if(n > 3){
				char order[10];
				strlcpy(order, buf, 3);
				if(strcmp(order,order1) == 0){
					strcpy(buf2, buf);
					char* ple = NULL;
					char* file_name = strtok_r(buf2," ", &ple);
					file_name = strtok_r(NULL, "\n", &ple);
					int filelength = getFileSize(file_name);
					char send[100];
					printf("%s\n", buf);
					snprintf(send, sizeof(send), "%s %d", buf, filelength);
					Writen(sockfd, send, strlen(send));
					SendFile(sockfd, file_name);
				}
				else
					Writen(sockfd, buf, n);
			}
			else
				Writen(sockfd, buf, n);
			for(l = 0;l < MAXLINE;l++)
				buf[l] = '\0';
		}
	}


	exit(0);
}
