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
		recv(sockfd, buf, sizeof(buf), 0);
		printf("收到服务器的回复 %s\n", buf);
		memset(buf, 0, sizeof(buf));
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
	server_addr.sin_addr.s_addr = inet_addr("192.168.31.105");
	//向服务器发起连接
	int ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (-1 == ret)
	{
		perror("connect");
		exit(1);
	}


	char buf[1024] = {0};
	//先判断 app 是否已经绑定
	const char *b1 = "{\"cmd\":\"search_bind\",\"appid\":\"101\"}";
	send(sockfd, b1, strlen(b1), 0);
	recv(sockfd, buf, sizeof(buf), 0);

	//解析json
	struct json_object *obj = json_tokener_parse(buf);
	struct json_object *json;
	json_object_object_get_ex(obj, "result", &json);
	if (!strcmp(json_object_get_string(json), "yes"))    //已经绑定
	{
		printf("app 已经绑定\n");
	}
	else if (!strcmp(json_object_get_string(json), "no"))
	{
		printf("APP 未绑定\n");

		//绑定操作
		const char *b2 = "{\"cmd\":\"bind\",\"appid\":\"101\",\"deviceid\":\"001\"}";
		ret = send(sockfd, b2, strlen(b2), 0);
		if (-1 == ret)
		{
			perror("send");
			exit(1);
		}
	}

	pthread_t tid;
	pthread_create(&tid, NULL, receive, &sockfd);

	sleep(5);
	const char *b = "{\"cmd\":\"app_start\"}";
	ret = send(sockfd, b, strlen(b), 0);

	sleep(2);

	close(sockfd);
	printf("xxx\n");
	sleep(10);

	return 0;
}
