/*
 * @Author: yohey 2607867045@qq.com
 * @Date: 2022-10-17 04:45:52
 * @LastEditors: yohey 2607867045@qq.com
 * @LastEditTime: 2022-10-28 14:08:06
 * @FilePath: \mp3code\music_server\node.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef NODE_H
#define NODE_H

#include <event.h>
#include <time.h>
#include <list>

struct Node 
{
	struct bufferevent *app_bev;
	struct bufferevent *device_bev;
	char app_id[8];
	char device_id[8];
	int online_flag;                     //表示设备是否在线  0 表示不在线
	int app_online_flag;                 //表示app 是否在线
	time_t time;
	struct event timeout;                //定时器事件
};
typedef struct Node Node;

struct timeout_node
{
	std::list<Node> *l;
	char id[8];
};
typedef struct timeout_node tNode;

#endif
