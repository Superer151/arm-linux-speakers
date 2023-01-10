#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "player.h"
//#include <json-c/json.h>
#include <json/json.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "socket.h"
#include <errno.h>

extern int g_buttonfd;
extern int g_sockfd;
extern fd_set readfd;
extern fd_set tmpfd;
extern int g_maxfd;

void show()
{
	printf("1、开始播放\n");
	printf("2、结束播放\n");
	printf("3、暂停播放\n");
	printf("4、继续播放\n");
	printf("5、上一首\n");
	printf("6、下一首\n");
	printf("7、增加音量\n");
	printf("8、减小音量\n");
	printf("9、顺序播放\n");
	printf("a、随即播放\n");
	printf("b、单曲循环\n");
}

void parse_message(const char *m, char *c)
{
	struct json_object *obj = json_tokener_parse(m);

	struct json_object *json;
	//json_object_object_get_ex(obj, "cmd", &json);
	json = json_object_object_get(obj, "cmd");
	strcpy(c, json_object_get_string(json));
}

void InitSelect()
{
	FD_ZERO(&readfd);
	FD_ZERO(&tmpfd);
	FD_SET(g_buttonfd, &readfd);
}

void m_select()
{
	show();

	fd_set tmpfd;
	int ret;
	char message[1024] = {0};

	while (1)
	{
		tmpfd = readfd;
		ret = select(g_maxfd + 1, &tmpfd, NULL, NULL, NULL);
		if (-1 == ret && errno != EINTR)
		{
			perror("select");
		}
		else if (-1 == ret && errno == EINTR)
		{
			continue;
		}

		if (FD_ISSET(g_sockfd, &tmpfd))           //TCP有数据可读
		{
			memset(message, 0, sizeof(message));
			ret = recv(g_sockfd, message, sizeof(message), 0);
			if (-1 == ret)
			{
				perror("recv");
			}

			char cmd[64] = {0};
			parse_message(message, cmd);

			if (!strcmp(cmd, "start"))
			{
				socket_start_play();			
			}
			else if (!strcmp(cmd, "stop"))
			{
				socket_stop_play();
			}
			else if (!strcmp(cmd, "suspend"))
			{
				socket_suspend_play();
			}
			else if (!strcmp(cmd, "continue"))
			{
				socket_continue_play();
			}
			else if (!strcmp(cmd, "prior"))
			{
				socket_prior_play();
			}
			else if (!strcmp(cmd, "next"))
			{
				socket_next_play();
			}
			else if (!strcmp(cmd, "voice_up"))
			{
				socket_voice_up_play();
			}
			else if (!strcmp(cmd, "voice_down"))
			{
				socket_voice_down_play();
			}
			else if (!strcmp(cmd, "sequence"))
			{
				socket_mode_play(SEQUENCEMODE);
			}
			else if (!strcmp(cmd, "random"))
			{
				socket_mode_play(RANDOM);
			}
			else if (!strcmp(cmd, "circle"))
			{
				socket_mode_play(CIRCLE);
			}
			else if (!strcmp(cmd, "get"))     //获取状态
			{	
				socket_get_status();
			}
			else if (!strcmp(cmd, "music"))   //获取所有音乐
			{
				socket_get_music();								
			}
		}
		else if (FD_ISSET(g_buttonfd, &tmpfd))    //按键有数据可读
		{
			int id = get_key_id();
			switch (id)
			{
				case 1:
					start_play();
					break;
				case 2:
					stop_play();
					break;
				case 3:
					suspend_play();
					break;
				case 4:
					continue_play();
					break;
				case 5:
					prior_play();
					break;
				case 6:
					next_play();
					break;
			}
		}
	}
}
