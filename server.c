#include "head4chat.h"

linklist head;

char *temperature = NULL;  //温度
char *humidity = NULL;     //湿度

void broad_cast(const char *msg, int fd)
{
	linklist tmp;

	for(tmp=head->next; tmp!=head; tmp=tmp->next)
	{
		if(tmp->connfd == fd)
			continue;

		write(tmp->connfd, msg, strlen(msg));
	}
}

linklist find_node(linklist head, int fd)
{
	linklist tmp;

	for(tmp=head->next; tmp!=head; tmp=tmp->next)
	{
		if(tmp->connfd == fd)
			return tmp;
	}
	
	return NULL;
}

void remove_node(linklist head, linklist p)
{
	p->prev->next = p->next;
	p->next->prev = p->prev;

	p->prev = p->next = NULL;
}

void private_talk(const char *msg, char *peer)
{
	unsigned short peerport = atoi(peer);

	linklist tmp;
	for(tmp=head->next; tmp!=head; tmp=tmp->next)
	{
		if(tmp->port == peerport)
		{
			write(tmp->connfd, msg, strlen(msg));
			return;
		}
	}
}

void *routine(void *arg)
{
	pthread_detach(pthread_self());
	
	linklist client = (linklist)arg;
	
	//int connfd = (int)arg;
	int connfd = client->connfd;
	
	char buf[100];	
	
	struct timeval a;
	a.tv_sec = 0;
	a.tv_usec = 0;
	
	fd_set rset, wset, eset;
	
	while(1)
	{
		FD_ZERO(&rset); // 添加在这里面的描述符，我们关注他们的“读就绪”状态
		FD_ZERO(&wset); // 添加在这里面的描述符，我们关注他们的“写就绪”状态
		FD_ZERO(&eset); // 添加在这里面的描述符，我们关注他们的“异常就绪”状态
		FD_SET(connfd, &rset);
		FD_SET(STDIN_FILENO, &rset); // 将两个描述符添加到rset里面！
		
		select(connfd+1, &rset, &wset, &eset, &a);
		
		bzero(buf, 100);
		
		if(FD_ISSET(connfd, &rset)) // 对方有消息来了！
		{
			if(read(connfd, buf, 100) == 0 || !strcmp(buf, "bye\n"))
			{
				close(connfd);
				linklist offline = find_node(head, connfd);
				if(offline != NULL)
				{
					remove_node(head, offline);
					free(offline);
				}
				
				printf("client [%s] offline\n", client->ip);
				pthread_exit(NULL);
			}
			
			printf("%s\n", buf);
			printf("receive %dsize\n", strlen(buf));
			
			//广播客户端
			char *bk = malloc(100);
			bzero(bk,100);
			strncpy(bk, buf, strlen(buf));
			broad_cast(bk, connfd);
			free(bk);
		}
	}
}

linklist init_list(void)
{
	linklist head = malloc(sizeof(listnode));
	head->prev = head->next = head;

	return head;
}

linklist new_node(int fd, struct sockaddr_in *p)
{
	unsigned int port = ntohs(p->sin_port);
	char *ip = NULL;
	ip = inet_ntoa(p->sin_addr);
	
	linklist new = malloc(sizeof(listnode));
	new->connfd = fd;
	new->port = port;
	strncpy(new->ip,ip, 16);
	printf("new client [%s] online\n", new->ip);

	return new;
}

void add_node(linklist head, linklist new)
{
	new->prev = head->prev;
	new->next = head;

	head->prev = new;
	new->prev->next = new;
}

//2秒广播一次传感器信息
void *BroadThread(void *arg)
{
	pthread_detach(pthread_self());
	int connfd = (int)arg;
	const char *message = "server:";
	
	while(1)
	{
		sleep(2);
		
		temperature = "25D";  //温度
		humidity = "49%";  	//湿度
		
		char bk[50];	
		bzero(bk, 50);
		
		strcat(bk,"$");
		strncat(bk, message, strlen(message));
		strcat(bk,":");
		strncat(bk,temperature,strlen(temperature));
		strcat(bk,":");
		strncat(bk,humidity,strlen(humidity));
		
		broad_cast(bk, connfd);
	}
}
int main(int argc, char **argv)
{
	head = init_list();

	int sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP

	struct sockaddr_in srvaddr;
	bzero(&srvaddr, sizeof srvaddr);
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvaddr.sin_port = htons(DEFAULT_PORT);
	bind(sockfd, (struct sockaddr *)&srvaddr, sizeof srvaddr);

	listen(sockfd, 5);

	struct sockaddr_in cliaddr;
	socklen_t len = sizeof cliaddr;
	
	int flag = 0;
	while(1)
	{	
		bzero(&cliaddr, len);
		int connfd = accept(sockfd,
				(struct sockaddr *)&cliaddr, &len);
	
		linklist new = new_node(connfd, &cliaddr);
		add_node(head, new);
	
		if(flag == 0)
		{
			flag = 1;
			
		//	pthread_t MyBroadThread;
		//	pthread_create(&MyBroadThread, NULL, BroadThread, (void *)head->connfd);
		}
		
		pthread_t tid;
		pthread_create(&tid, NULL, routine, (void *)new);//创建客户端处理线程
		
	}

	return 0;
}
