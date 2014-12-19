#include "head4chat.h"

//scp server.c class71@120.24.101.184:/home/class71/adam/HomeServer/server.c

//const char *HOST = "192.168.1.123";
const char *HOST = "120.24.101.184";


void *routine(void *arg)
{
	pthread_detach(pthread_self());

	int fd = (int)arg;

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
		FD_SET(fd, &rset);	
		
		select(fd+1, &rset, &wset, &eset, &a);
		
		bzero(buf, 100);
		if(FD_ISSET(fd, &rset))
		{
			read(fd, buf, 100);
			printf("%s", buf);
		}
	}
}

int main(int argc, char **argv)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0); // TCP

	int on = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);

	struct sockaddr_in srvaddr;
	bzero(&srvaddr, sizeof srvaddr);
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_addr.s_addr = inet_addr(HOST);
	srvaddr.sin_port = htons(DEFAULT_PORT);

	if(connect(fd, (struct sockaddr *)&srvaddr, sizeof srvaddr) == -1)
	{
		perror("connect() error");
		exit(1);
	}

	struct sockaddr_in myaddr;
	socklen_t len = sizeof myaddr;
	bzero(&myaddr, len);
	getsockname(fd, (struct sockaddr *)&myaddr, &len);
	printf("my port: %hu\n", htons(myaddr.sin_port));


	//pthread_t tid;
	//pthread_create(&tid, NULL, routine, (void *)fd);


	char buf[100];
	
	fd_set rset, wset, eset; // 这些是描述符集合
	struct timeval a;
	a.tv_sec = 0;
	a.tv_usec = 0;
	
	while(1)
	{
#if 0 
		FD_ZERO(&rset); // 添加在这里面的描述符，我们关注他们的“读就绪”状态
		FD_ZERO(&wset); // 添加在这里面的描述符，我们关注他们的“写就绪”状态
		FD_ZERO(&eset); // 添加在这里面的描述符，我们关注他们的“异常就绪”状态
		FD_SET(fd, &rset);
		FD_SET(STDIN_FILENO, &rset); // 将两个描述符添加到rset里面！

		select(fd+1, &rset, &wset, &eset, &a);
		
		bzero(&buf, 100);
		if(FD_ISSET(STDIN_FILENO, &rset)) // 用户输入数据了！
		{
			fgets(buf, 100, stdin);				
			write(fd, buf, strlen(buf));
		}
#endif
		FD_ZERO(&rset); // 添加在这里面的描述符，我们关注他们的“读就绪”状态
		FD_ZERO(&wset); // 添加在这里面的描述符，我们关注他们的“写就绪”状态
		FD_ZERO(&eset); // 添加在这里面的描述符，我们关注他们的“异常就绪”状态
		FD_SET(fd, &rset);
		FD_SET(STDIN_FILENO, &rset); // 将两个描述符添加到rset里面！

		select(fd+1, &rset, &wset, &eset, &a);
		
		char buf[100];
		bzero(buf, 100);
		if(FD_ISSET(fd, &rset)) // 对方有消息来了！
		{
			if(read(fd, buf, 100) == 0)
				break;
			printf("broadcast: %s\n", buf);
		}
		if(FD_ISSET(STDIN_FILENO, &rset)) // 用户输入数据了！
		{
			fgets(buf, 100, stdin);
			write(fd, buf, strlen(buf));
		}
	}

	return 0;
}
