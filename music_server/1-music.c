#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <json-c/json.h>

void *receive(void *arg)
{
	int sockfd = *(int *)arg;
	char buf[1024] = {0};
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		recv(sockfd, buf, sizeof(buf), 0);
		printf("收到服务器的消息 %s\n", buf);

		struct json_object *obj = json_tokener_parse(buf);
		struct json_object *json;
		json_object_object_get_ex(obj, "cmd", &json);
		if (!strcmp(json_object_get_string(json), "start"))
		{
			printf("收到开始播放\n");
			const char *buf = "{\"cmd\":\"reply\",\"result\":\"start_success\"}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(json_object_get_string(json), "suspend"))
		{
			printf("收到暂停播放\n");
			const char *buf = "{\"cmd\":\"reply\",\"result\":\"suspend_success\"}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(json_object_get_string(json), "continue"))
		{
			printf("收到继续播放\n");
			const char *buf = "{\"cmd\":\"reply\",\"result\":\"continue__success\"}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(json_object_get_string(json), "next"))
		{
			printf("收到下一首\n");
			const char *buf = "{\"cmd\":\"reply\",\"result\":\"next_success\"}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(json_object_get_string(json), "prior"))
		{
			printf("收到上一首\n");
			const char *buf = "{\"cmd\":\"reply\",\"result\":\"prior_success\"}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(json_object_get_string(json), "voice_up"))
		{
			printf("收到增加音量\n");
			const char *buf = "{\"cmd\":\"reply\",\"result\":\"success\"}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(json_object_get_string(json), "voice_down"))
		{
			printf("收到减小音量\n");
			const char *buf = "{\"cmd\":\"reply\",\"result\":\"success\"}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(json_object_get_string(json), "sequence"))
		{
			printf("收到顺序播放\n");
			const char *buf = "{\"cmd\":\"reply\",\"result\":\"success\"}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(json_object_get_string(json), "random"))
		{
			printf("收到随机播放\n");
			const char *buf = "{\"cmd\":\"reply\",\"result\":\"success\"}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(json_object_get_string(json), "get"))
		{
			printf("收到获取状态\n");
			const char *buf = "{\"cmd\":\"reply_status\",\"status\":\"start\",\"music\":\"x.mp3\",\"voice\":30}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(json_object_get_string(json), "music"))
		{
			printf("收到获取所有音乐\n");
			const char *buf = "{\"cmd\":\"reply_music\",\"music\":[\"1.mp3\",\"2.mp3\",\"3.mp3\",\"4.mp3\"]}";
			int ret = send(sockfd, buf, strlen(buf), 0);
		}
		

	}
}

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd)
	{
		perror("socket");
		exit(1);
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	server_addr.sin_addr.s_addr = inet_addr("172.17.7.99");
	//向服务器发起连接
	int ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (-1 == ret)
	{
		perror("connect");
		exit(1);
	}

	pthread_t tid;
	pthread_create(&tid, NULL, receive, &sockfd);

	//char buf[32] = {0};
	while (1)
	{
		//scanf("%s", buf);
		const char *buf = "{\"cmd\":\"info\",\"status\":\"alive\",\"deviceid\":\"001\"}";
		ret = send(sockfd, buf, strlen(buf), 0);
		if (-1 == ret)
		{
			perror("send");
			exit(1);
		}
		sleep(1);

		//memset(buf, 0, sizeof(buf));
	}

	close(sockfd);

	return 0;
}
