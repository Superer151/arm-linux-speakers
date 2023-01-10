#ifndef PLAYER_H
#define PLAYER_H

#include <list>
#include "node.h"
#include <event.h>
#include <jsoncpp/json/json.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <event2/util.h>
#include <stdlib.h>

class Player
{
private:

public:
	void player_alive_info(std::list<Node> *l, struct bufferevent *bev, Json::Value val, struct event_base *base);
	void player_operation(std::list<Node> *l, struct bufferevent *app_bev, const char *cmd);
	void player_reply_result(std::list<Node> *l, struct bufferevent *bev, Json::Value val);

	static void timeout_cb(evutil_socket_t fd, short event, void *arg);
};

#endif
