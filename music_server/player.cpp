#include "player.h"

void Player::player_alive_info(std::list<Node> *l, struct bufferevent *bev, Json::Value val, struct event_base *base)
{
	char deviceid[8] = {0};
	strcpy(deviceid, val["deviceid"].asString().c_str());

	for (std::list<Node>::iterator it = l->begin(); it != l->end(); it++)
	{
		if (!strcmp(deviceid, it->device_id))	
		{
			if (it->online_flag == 0)           //表示设备不在线，第一次发送保活消息
			{
				tNode *t = new tNode;
				t->l = l;
				strcpy(t->id, deviceid);

				event_assign(&(it->timeout), base, -1, EV_PERSIST, timeout_cb, t);    //设置事件参数
				struct timeval tv;
				evutil_timerclear(&tv);
				tv.tv_sec = 1;                  //1秒执行一次
				event_add(&(it->timeout), &tv);

				it->online_flag = 1;            //表示设备在线
			}
			it->device_bev = bev;
			it->time = time(NULL);

			std::cout << "收到保活消息 信息更新成功" << std::endl;
			return;
		}
	}

	std::cout << "收到保活消息 但是设备未绑定" << std::endl;
}

/*
描述：服务器收到APP消息，转发给音箱
参数：app_bev 对应的是app的bufferevent
*/
void Player::player_operation(std::list<Node> *l, struct bufferevent *app_bev, const char *cmd)
{
	Json::Value val;

	if (!strcmp(cmd, "app_start"))
	{
		val["cmd"] = "start";
	}
	else if (!strcmp(cmd, "app_stop"))
	{
		val["cmd"] = "stop";
	}
	else if (!strcmp(cmd, "app_suspend"))
	{
		val["cmd"] = "suspend";
	}
	else if (!strcmp(cmd, "app_continue"))
	{
		val["cmd"] = "continue";
	}
	else if (!strcmp(cmd, "app_prior"))
	{
		val["cmd"] = "prior";
	}
	else if (!strcmp(cmd, "app_next"))
	{
		val["cmd"] = "next";
	}
	else if (!strcmp(cmd, "app_voice_up"))
	{
		val["cmd"] = "voice_up";
	}
	else if (!strcmp(cmd, "app_voice_down"))
	{
		val["cmd"] = "voice_down";
	}
	else if (!strcmp(cmd, "app_sequence"))
	{
		val["cmd"] = "sequence";
	}
	else if (!strcmp(cmd, "app_random"))
	{
		val["cmd"] = "random";
	}
	else if (!strcmp(cmd, "app_circle"))
	{
		val["cmd"] = "circle";
	}
	else if (!strcmp(cmd, "app_music"))
	{
		val["cmd"] = "music";
	}

	std::string str = Json::FastWriter().write(val);
	size_t ret;
	
	//遍历链表判断是否在线
	for (std::list<Node>::iterator it = l->begin(); it != l->end(); it++)
	{
		if (it->app_bev == app_bev)	
		{
			if (it->online_flag == 1)      //在线 
			{
				ret = bufferevent_write(it->device_bev, str.c_str(), strlen(str.c_str()));	
				if (ret < 0)
				{
					std::cout << "bufferevent_write error" << std::endl;
				}
			}
			else
			{
				Json::Value v;
				v["cmd"] = "app_reply";
				v["result"] = "off_line";
				std::string s = Json::FastWriter().write(v);
				ret = bufferevent_write(app_bev, s.c_str(), strlen(s.c_str()));	
				if (ret < 0)
				{
					std::cout << "bufferevent_write error" << std::endl;
				}

			}
		}
		break;
	}
}

/*
描述：收到音箱的回复消息，服务器回复APP
参数：bev 对应的是音箱的 bufferevent
*/
void Player::player_reply_result(std::list<Node> *l, struct bufferevent *bev, Json::Value val)
{	
	char cmd[32] = {0};
	strcpy(cmd, val["cmd"].asString().c_str());

	if (!strcmp(cmd, "reply"))
	{
		val["cmd"] = "app_reply";
	}
	else if (!strcmp(cmd, "reply_music"))
	{
		val["cmd"] = "app_reply_music";
	}
	else if (!strcmp(cmd, "reply_status"))
	{
		val["cmd"] = "app_reply_status";
	}

	std::string str = Json::FastWriter().write(val);
	size_t ret;
	//遍历链表，找到 app 对应的 bufferevent
	for (std::list<Node>::iterator it = l->begin(); it != l->end(); it++)
	{
		if (it->device_bev == bev)
		{
			if (it->app_online_flag == 1)
			{
				ret = bufferevent_write(it->app_bev, str.c_str(), strlen(str.c_str()));
				if (ret < 0)
				{
					std::cout << "bufferevent_write" << std::endl;
				}
			}

			return;
		}
	}

	std::cout << "app 不存在" << std::endl;
}

void Player::timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	std::cout << "定时器事件" << std::endl;
	tNode t;
	memcpy(&t, arg, sizeof(tNode));
	
	//1、根据 time 判断音箱是否在线
	std::list<Node>::iterator it;
	for (it = (t.l)->begin(); it != (t.l)->end(); it++)
	{
		if (!strcmp(it->device_id, t.id))
		{
			if (time(NULL) - it->time > 1)
			{
				it->online_flag = 0;     //表示不在线
			}
			else 
			{
				it->online_flag = 1;
			}
			break;
		}
	}

	//2、如果音箱和 APP 同时在线，则发起状态请求
	if (it->app_online_flag == 1 && it->online_flag == 1)
	{
		Json::Value val;
		val["cmd"] = "get";
		std::string str = Json::FastWriter().write(val);

		size_t ret = bufferevent_write(it->device_bev, str.c_str(), strlen(str.c_str()));
		if (ret < 0)
		{ 
			std::cout << "bufferevent_write error" << std::endl;
		}
	}
}
