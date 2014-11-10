#include "http_post.h"

int sockfd;
int port = 8089;
//int port = 8080;

char *ip = "158.182.246.221";
//char *ip = "158.182.246.224";
char *host = "158.182.246.221:8089";
//char *host = "158.182.246.224:8080";
char *page = "/inpService/api/writeMacAddress";

int setup_http_request()
{
	struct sockaddr_in servaddr;
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	if((sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0){
		perror("Can't create TCP socket!\n");  
		return 0;
	}
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	int tmpres = inet_pton(AF_INET, ip, &servaddr.sin_addr);
	if(tmpres<0){
	  perror("Can't set remote->sin_addr.s_addr");
	  return 0;
	}else if(tmpres==0){
		fprintf(stderr,"%s is not a valid IP address\n", ip);
		return 0;
	}

	if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	{
		perror("setsockopt failed\n");
		return 0;
	}
  if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
  {
  	perror("setsockopt failed\n");
  	return 0;
  }

	if(connect(sockfd, (SA *) & servaddr, sizeof(servaddr))<0){
		perror("Could not connect!\n");  
		return 0;
  }

  return 1;
}

int process_post(char *params)
{
	if(setup_http_request()) {
		//printf("setup http requet succeed\n");
	} else {
		printf("setup http requet failed\n");
		return -1;
	}

	char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
	int n;

	snprintf(sendline, strlen(params) + MAXSUB,
		 "POST %s HTTP/1.0\r\n"
		 "Host: %s\r\n"
		 "Content-type: application/x-www-form-urlencoded\r\n"
		 "Content-length: %d\r\n\r\n"
		 "%s", page, host, strlen(params), params);

	write(sockfd, sendline, strlen(sendline));
	printf("******************************************************************\n");
  printf("*                       receive parameters...                    *\n");
	//while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
  if((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = '\0';
		printf("*       ");
		int i, index;
		int flag = 0;
		int position = 0;
		for(i=0; i<n; i++)
		{
			if (recvline[i] == '\n' || recvline[i] == '\r')
			{
				if(flag == 1)
				{
					for(index=position; index<57; index++)
					{
						printf(" ");
					}
					printf("*\n*       ");
					flag = 0;
					position = 0;
				}
			}
			else
			{
				printf("%c",recvline[i]);
				flag = 1;
				position++;
			}
		}
		for(index=position; index<57; index++)
		{
			printf(" ");
		}
		printf("*");
	}
	printf("\n******************************************************************\n");

	close(sockfd);
	return n;
}