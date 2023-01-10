#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include <string.h>
#include "link.h"
#include "player.h"
#include <time.h>

extern struct Node *head;

//初始化双向循环链表
int InitLink()
{
	head = (Node *)malloc(sizeof(Node) * 1);
	if (NULL == head)
	{
		return FAILURE;
	}

	head->next = head;
	head->prior = head;

	return SUCCESS;
}

int InsertLink(Node *h, const char *name)
{
	if (NULL == h || NULL == name)
	{
		return FAILURE;
	}

	Node *end = h->prior;


	Node *n = (Node *)malloc(sizeof(Node) * 1);
	if (NULL == n)
	{
		return FAILURE;
	}

	n->next = h;
	strcpy(n->music_name, name);
	end->next = n;
	n->prior = end;
	h->prior = n;

	return SUCCESS;
}

/*
函数描述：根据当前歌曲和播放模式，找到下一首歌
函数参数：cur：当前歌曲
          mode:播放模式
          next：存放下一首歌名
返回值：无
*/
void FindNextMusic(const char *cur, int mode, char *next)
{
	if (mode == CIRCLE)     //单曲循环
	{
		strcpy(next, cur);
		return;
	}
	else if (mode == SEQUENCEMODE)
	{
		Node *p = head->next;
		while (strcmp(p->music_name, cur) != 0)
		{
			p = p->next;
		}
			
		if (p->next == head)          //忽略头结点
		{
			strcpy(next, head->next->music_name);
		}
		else
		{
			strcpy(next, p->next->music_name);
		}
		return;
	}
	else 
	{
		Node *p = head->next;
		srand(time(NULL));
		int num = rand() % 100;

		int i;
		for (i = 0; i < num; i++)
		{
			p = p->next;
		}

		if (p == head)
		{
			strcpy(next, head->next->music_name);
		}
		else
		{
			strcpy(next, p->music_name);
		}

		return;
	}
}

void PriorMusic(const char *cur, int mode, char *prior)
{
	if (mode == SEQUENCEMODE || mode == CIRCLE)
	{
		Node *p = head->next;
		while (strcmp(p->music_name, cur) != 0)
		{
			p = p->next;
		}

		if (p == head->next)
		{
			printf("%s\n", head->prior->music_name);
			strcpy(prior, head->prior->music_name);
		}
		else
		{
			strcpy(prior, p->prior->music_name);
		}
		return;
	}
	else 
	{
		Node *p = head->next;
		srand(time(NULL));
		int num = rand() % 100;

		int i;
		for (i = 0; i < num; i++)
		{
			p = p->next;
		}

		if (p == head)
		{
			strcpy(prior, p->next->music_name);
		}
		else
		{
			strcpy(prior, p->music_name);
		}

		return;
	}
}

void NextMusic(const char *cur, int mode, char *next)
{
	if (mode == SEQUENCEMODE || mode == CIRCLE)
	{
		Node *p = head->next;
		while (strcmp(p->music_name, cur) != 0)
		{
			p = p->next;
		}

		if (p->next == head)
		{
			strcpy(next, head->next->music_name);
		}
		else
		{
			strcpy(next, p->next->music_name);
		}
		return;
	}
	else 
	{
		Node *p = head->next;
		srand(time(NULL));
		int num = rand() % 100;

		int i;
		for (i = 0; i < num; i++)
		{
			p = p->next;
		}

		if (p == head)
		{
			strcpy(next, p->next->music_name);
		}
		else
		{
			strcpy(next, p->music_name);
		}

		return;
	}
}
