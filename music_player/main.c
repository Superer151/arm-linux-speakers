#include <stdio.h>
#include "main.h"
#include "device.h"
#include <stdlib.h>
#include "socket.h"
#include "link.h"
#include "player.h"
#include <signal.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <string.h>
#include <sys/select.h>

int g_buttonfd = 3;
int g_ledfd;
int g_mixerfd;
int g_sockfd = 3;
struct Node *head;
extern int shmid;
extern void *g_addr;
fd_set readfd, tmpfd;
int g_maxfd;


void m_select();

void handler(int sig)
{
	shm s;
	memset(&s, 0, sizeof(s));
	memcpy(&s, g_addr, sizeof(s));
	kill(s.child_pid, SIGKILL);
	kill(s.grand_pid, SIGKILL);

	shmdt(g_addr);
	shmctl(shmid, IPC_RMID, NULL);

	close(g_sockfd);

	exit(0);
}

int main()
{
	int ret;
	ret = InitDriver();     //打开设备文件
	if (FAILURE == ret)
	{
		printf("初始化设备文件失败\n");
		exit(1);
	}
	
	InitSelect();

	ret = InitSocket();      //初始化网络
	if (FAILURE == ret)
	{
		printf("初始化网络失败\n");
		//初始化失败，点亮 2 个LED灯
		led_on(0);
		led_on(1);
	}

	//初始化链表
	ret = InitLink();
	if (FAILURE == ret)
	{
		printf("链表初始化失败\n");
		exit(1);
	}

	//初始化共享内存
	ret = InitShm();
	if (FAILURE == ret)
	{
		printf("共享内存初始化失败\n");
		exit(1);
	}

	//读取音乐
	GetMusic();

	//捕获ctrl c信号
	signal(SIGINT, handler);

	m_select();

	return 0;
}
