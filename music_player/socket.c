//跟网络相关的代码

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "main.h"
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "device.h"
#include "socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
//#include <json-c/json.h>
#include <json/json.h>
#include "player.h"
#include <sys/select.h>
#include "link.h"

extern Node *head;
extern int g_sockfd;
extern int iLeft;
extern int g_start_flag;
extern int g_suspend_flag;
extern fd_set readfd;
extern int g_maxfd;
extern void *g_addr;

// 5秒执行一次，向服务器发送 alive 字符串
void send_server(int sig)
{
	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("info"));
		(json, "status", json_object_new_string("alive"));
	json_object_object_add(json, "deviceid", json_object_new_string("001"));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}

	alarm(TIMEOUT);
}

void *connect_cb(void *arg)
{
	int count = 5, ret;
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	while (count--)
	{
		ret = connect(g_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
		if (ret == -1)
		{
			sleep(5);
			continue;
		}

		//连接成功，点亮 4 个LED灯
		led_on(0);
		led_on(1);
		led_on(2);
		led_on(3);

		alarm(TIMEOUT);
		signal(SIGALRM, send_server);

		//连接服务器成功，fd添加到集合
		FD_SET(g_sockfd, &readfd);
		if (g_maxfd < g_sockfd)
		{
			g_maxfd = g_sockfd;
		}

		break;
	}

	return NULL;
}

int InitSocket()
{
	g_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (-1 == g_sockfd)
	{
		return FAILURE;
	}

	//启动线程向服务器发起连接
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, connect_cb, NULL);
	if (ret != 0)
	{
		return FAILURE;
	}

	return SUCCESS;
}

void socket_start_play()
{
	start_play();

	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply"));
	json_object_object_add(json, "result", json_object_new_string("start_success"));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}

void socket_stop_play()
{
	stop_play();

	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply"));
	json_object_object_add(json, "result", json_object_new_string("stop_success"));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}

void socket_suspend_play()
{
	suspend_play();

	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply"));
	json_object_object_add(json, "result", json_object_new_string("suspend_success"));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}

void socket_continue_play()
{
	continue_play();

	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply"));
	json_object_object_add(json, "result", json_object_new_string("continue_success"));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}

void socket_prior_play()
{
	prior_play();

	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply"));
	json_object_object_add(json, "result", json_object_new_string("success"));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}

void socket_next_play()
{
	next_play();

	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply"));
	json_object_object_add(json, "result", json_object_new_string("success"));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}

void socket_voice_up_play()
{
	voice_up();

	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply"));
	json_object_object_add(json, "result", json_object_new_string("success"));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}

void socket_voice_down_play()
{
	voice_down();

	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply"));
	json_object_object_add(json, "result", json_object_new_string("success"));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}

void socket_mode_play(int mode)
{
	set_mode(mode);

	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply"));
	json_object_object_add(json, "result", json_object_new_string("success"));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}

void socket_get_status()
{
	//播放状态  当前歌曲名  音量
	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply_status"));
	if (g_start_flag == 1 && g_suspend_flag == 0)
	{
		json_object_object_add(json, "status", json_object_new_string("start"));
	}
	else if (g_start_flag == 1 && g_suspend_flag == 1)
	{
		json_object_object_add(json, "status", json_object_new_string("suspend"));
	}
	else if (g_start_flag == 0)
	{
		json_object_object_add(json, "status", json_object_new_string("stop"));
	}
	json_object_object_add(json, "voice", json_object_new_int(iLeft));

	shm s;
	memset(&s, 0, sizeof(s));
	memcpy(&s, g_addr, sizeof(s));
	json_object_object_add(json, "music", json_object_new_string(s.cur_name));

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}

void socket_get_music()
{
	struct json_object *json = json_object_new_object();
	json_object_object_add(json, "cmd", json_object_new_string("reply_music"));

	struct json_object *array = json_object_new_array();
	Node *p = head->next;
	while (p != head)
	{
		json_object_array_add(array, json_object_new_string(p->music_name));
		p = p->next;
	}

	json_object_object_add(json, "music", array);

	const char *buf = json_object_to_json_string(json);
	int ret = send(g_sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
	}
}
