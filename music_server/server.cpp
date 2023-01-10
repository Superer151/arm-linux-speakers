#include "server.h"

Player *PlayerServer::p = new Player();      //创建播放器对象
std::list<Node> *PlayerServer::l = new std::list<Node>();

PlayerServer::PlayerServer(const char *ip, int port)
{
	base = event_base_new();               //创建事件集合

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip);

	listener = evconnlistener_new_bind(base, listener_cb, base, 
		LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (NULL == listener)
	{
		std::cout << "evconnlistener_new_bind error" << std::endl;
	}

	event_base_dispatch(base);      //监听集合
}

PlayerServer::~PlayerServer()
{
	//释放两个对象
	evconnlistener_free(listener);
	event_base_free(base);
}

void PlayerServer::listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg)
{
	struct event_base *base = (struct event_base *)arg;
	std::cout << "有客户端连接 " << fd << std::endl;

	//创建bufferevent事件
	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bev)
	{
		std::cout << "bufferevent_socket_new error" << std::endl;
	}

	bufferevent_setcb(bev, read_cb, NULL, event_cb, base);
	bufferevent_enable(bev, EV_READ);
}

void PlayerServer::read_cb(struct bufferevent *bev, void *ctx)
{
	struct event_base *base = (struct event_base *)ctx;
	char buf[1024] = {0};
	size_t ret = bufferevent_read(bev, buf, sizeof(buf));
	if (ret < 0)
	{
		std::cout << "bufferevent_read error" << std::endl;
	}

	std::cout << buf << std::endl;

	Json::Reader reader;
	Json::Value val;

	bool result = reader.parse(buf, val);
	if (!result)
	{
		std::cout << "解析json出错" << std::endl;
		return;
	}

	char cmd[32] = {0};
	strcpy(cmd, val["cmd"].asString().c_str());

	//app发送的消息
	if (!strcmp(cmd, "bind"))                //绑定功能
	{
		Node n;

		n.app_bev = bev;
		strcpy(n.app_id, val["appid"].asString().c_str());
		strcpy(n.device_id, val["deviceid"].asString().c_str());
		n.online_flag = 0;
		n.app_online_flag = 1;

		l->push_back(n);

		//回复APP
		val["cmd"] = "bind_success";
		//把Value对象转换成string
		std::string str = Json::FastWriter().write(val);
		size_t ret = bufferevent_write(bev, str.c_str(), strlen(str.c_str()));
		if (ret < 0)
		{
			std::cout << "bufferevent_write error" << std::endl;
		}
	}
	else if (!strcmp(cmd, "search_bind"))     //查询是否已经绑定
	{
		//遍历链表，判断链表中是否存在 appid
		std::list<Node>::iterator it;
		for (it = l->begin(); it != l->end(); it++)
		{
			if (!strcmp(it->app_id, val["appid"].asString().c_str()))
			{
				it->app_bev = bev;           //更新 app_bev
				it->app_online_flag = 1;     //app在线

				//appid 存在
				val["cmd"] = "reply_bind";
				val["result"] = "yes";
				break;
			}
		}

		if (it == l->end())            //链表遍历到最后，appid 不存在
		{
			val["cmd"] = "reply_bind";
			val["result"] = "no";
		}

		std::string str = Json::FastWriter().write(val);
		size_t ret = bufferevent_write(bev, str.c_str(), strlen(str.c_str()));
		if (ret < 0)
		{
			std::cout << "bufferevent_write error" << std::endl;
		}

	}
	else if (!strcmp(cmd, "app_start"))      //开始播放
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_stop"))       //结束播放
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_suspend"))    //暂停播放
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_continue"))   //继续播放
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_prior"))      //上一首
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_next"))       //下一首
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_voice_up"))   //增加音量
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_voice_down")) //减小音量
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_sequence"))   //顺序播放
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_random"))     //随机播放
	{	
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_circle"))     //单曲播放
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_music"))      //获取音乐
	{
		p->player_operation(l, bev, cmd);
	}
	else if (!strcmp(cmd, "app_off_line"))  //APP下线
	{
		for (std::list<Node>::iterator it = l->begin(); it != l->end(); it++)
		{
			if (it->app_bev == bev)
			{
				it->app_online_flag = 0;
				bufferevent_free(it->app_bev);
				std::cout << "APP下线" << std::endl;
				break;
			}
		}
	}
	//音箱发送的消息
	else if (!strcmp(cmd, "reply"))          //操作成功回复
	{
		p->player_reply_result(l, bev, val);
	}
	else if (!strcmp(cmd, "info"))           //音箱的保活消息
	{
		p->player_alive_info(l, bev, val, base);	
	}
	else if (!strcmp(cmd, "reply_status"))   //回复播放器状态
	{
		p->player_reply_result(l, bev, val);
	}
	else if (!strcmp(cmd, "reply_music"))      //返回所有音乐
	{
		p->player_reply_result(l, bev, val);
	}
}

void PlayerServer::event_cb(struct bufferevent *bev, short what, void *ctx)
{
	if (what & BEV_EVENT_EOF)
	{
		for (std::list<Node>::iterator it = l->begin(); it != l->end(); it++)
		{
			if (it->device_bev == bev)
			{
				std::cout << "音箱下线" << std::endl;
				it->online_flag = 0;              //表示不在线
				event_del(&it->timeout);           //移除定时器
				return;
			}
			/*if (it->app_bev == bev)
			{
				std::cout << "APP 下线" << std::endl;
				it->app_online_flag = 0;
			}*/
		}
	}
	else 
	{
		std::cout << "异常发生" << std::endl;
	}
}


