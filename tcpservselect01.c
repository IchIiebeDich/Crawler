/* include fig01 */
#include	"unp.h"
#define MAXNUM 2
int
main(int argc, char **argv)
{
	int					i, k, l, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, client[MAXNUM],flag[MAXNUM];
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[MAXLINE],buf2[MAXLINE];
	char				buff[MAXLINE];
	char				ip[MAXLINE];
	time_t				ticks;
	time_t				start[MAXNUM], end[MAXNUM];
	struct timeval			timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	union {
		short  		s;
    		char   		c[sizeof(short)];
    		} un;
	char* 				str1 = "/exit/";
	char* 				str2 = "1";
	char* 				str3 = "2";
	char* 				str4 = "3";
	char 				error[20] = "Wrong input\n";
	char				brk[20] = "Time out! Break!";

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	maxfd = listenfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < MAXNUM; i++){
		client[i] = -1;			/* -1 indicates available entry */
		flag[i] = 0;
	}
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
/* end fig01 */

/* include fig02 */
	for ( ; ; ) {
		rset = allset;		/* structure assignment */
		nready = Select(maxfd+1, &rset, NULL, NULL, &timeout);
		if(nready == 0){
			for(i = 0;i < MAXNUM;i++)
				if(client[i] > 0){
					Writen(client[i], brk, strlen(brk));
					printf("close %d\n",client[i]);
					close(client[i]);
					FD_CLR(client[i], &allset);
					client[i] = -1;
					flag[i] = 0;
					printf("close done\n");
				}
			continue ;
		}
		else{
			for(i = 0;i < MAXNUM;i++){
				if(client[i] > 0){
					end[i] = time(NULL);
					if(difftime(end[i], start[i]) >= 10){
						Writen(client[i], brk, strlen(brk));
						printf("close %d\n",client[i]);
						close(client[i]);
						FD_CLR(client[i], &allset);
						client[i] = -1;
						flag[i] = 0;
						printf("close done.\n");
					}
				}
			}		
		}


		if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
			ticks = time(NULL);
        		snprintf(buff, sizeof(buff), "\n%.24s\r\n", ctime(&ticks));


			for (i = 0; i < MAXNUM; i++)
				if (client[i] < 0) {
					client[i] = connfd;	/* save descriptor */
					start[i] = time(NULL);
					break;
				}
			if (i == MAXNUM){
				char connerror[100] = "There're alright 2 clients and you can't connect.Please wait and try again!";
				Writen(connfd, connerror, strlen(connerror));
				close(connfd);
				continue;
			}
			else{
				if(n = read(connfd,buf,MAXLINE) > 0){
			
					char* ple = NULL;
					char* str_1 = strtok_r(buf, ",", &ple);
					char* str_2 = strtok_r(NULL, ",", &ple);
			
					printf("%d new client:\nName:%s IP Address:%s Port:%d Time:%s\n",
						connfd,
						str_1,
						Inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip)),
						ntohs(cliaddr.sin_port),
						buff);
			
					Writen(connfd, buff, strlen(buff));
			
					char* my;
					un.s = 0x0102;
					if(sizeof(short) == 2){
						if(un.c[0] == 1 && un.c[1] == 2)
							 my = "big-endian";
						else if(un.c[0] == 2 && un.c[1] == 1)
							 my = "little-endian";
					}else ;
					if(strcmp(str_2, my) == 0){
						char send[30] = "Same Host Byte Order.\n";
						Writen(connfd, send, strlen(send));
					}
					else{
						char send[30] = "Different Host Byte Order.\n";
						Writen(connfd, send, strlen(send));
					}
					HomeMenu(connfd);
				}
				else ;
				for(l=0;l<MAXLINE;l++){
					buf[l] = '\0';
					buf2[l] = '\0';
				}
			}

			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				start[i] = time(NULL);
				if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
						/*4connection closed by client */
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
			    else{
				       	strncpy(buf2,buf,n-1);
					if(strcmp(buf2, str1) == 0){
						flag[i] = 0;
						HomeMenu(sockfd);
					}
					else if(flag[i] == 0){
						
						if(strcmp(buf2, str2) == 0){
							flag[i] = 1;
							printf("%d-%s", sockfd, buf);
						}
						else if(strcmp(buf2, str3) == 0){
							flag[i] = 2;
							printf("%d-%s", sockfd, buf);
						}
						else if(strcmp(buf2, str4) == 0){
							flag[i] = 3;
							printf("%d-%s", sockfd, buf);
						}
						else
							Writen(sockfd, error, strlen(error));
					}
					else{
						if(flag[i] == 1){
							Writen(sockfd, buf, n);
							printf("%s",buf);
						}
						else if(flag[i] == 2){
							printf("%s",buf);
							calculator(buf2,sockfd);
						}
						else if(flag[i] == 3){
							printf("content:%s\n",buf);
							char* ple = NULL;
							char* file_name = strtok_r(buf, " ",&ple);
							file_name = strtok_r(NULL, "\n ", &ple);
							printf("file_name:%s ",file_name);
							char* file_length = strtok_r(NULL, " ", &ple);
							int filelength = atoi(file_length);
							printf("filelength:%d\n",filelength);
							RecvFile(sockfd, file_name, filelength);
						}
					}
					for( l = 0; l<MAXLINE;l++){
						buf[l] = '\0';
						buf2[l] = '\0';
					}
				}
	

				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
	}
}
/* end fig02 */
