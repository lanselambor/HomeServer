#ifndef _HEAD4CHAT_H_
#define _HEAD4CHAT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/time.h>

#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

typedef struct node
{
	int connfd;
	unsigned int port;
	char ip[16];

	struct node *prev;
	struct node *next;
}listnode, *linklist;

#define DEFAULT_PORT 50000

#endif
