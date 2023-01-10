//用于操作硬件设备

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "main.h"
#include <sys/ioctl.h>

extern int g_buttonfd;
extern int g_ledfd;
extern int g_mixerfd;
extern int g_maxfd;

int InitDriver()
{
	//打开按键设备文件
	g_buttonfd = open("/dev/buttons", O_RDONLY);
	if (-1 == g_buttonfd)
	{
		return FAILURE;
	}

	//打开led设备文件
	g_ledfd = open("/dev/leds", O_WRONLY);
	if (-1 == g_ledfd)
	{
		return FAILURE;
	}

	//所有LED灭
	int i;
	for (i = 0; i < 4; i++)
	{
		ioctl(g_ledfd, 0, i);
	}

	//打开minxer设备文件
	g_mixerfd = open("/dev/mixer", O_WRONLY);
	if (-1 == g_mixerfd)
	{
		return FAILURE;
	}

	if (g_mixerfd > g_maxfd)
	{
		g_maxfd = g_mixerfd;
	}

	return SUCCESS;
}

void led_on(int which)
{
	ioctl(g_ledfd, 1, which);	
}

void led_off(int which)
{
	ioctl(g_ledfd, 0, which);
}

int get_key_id()
{
	char buttons[6] = {'0', '0', '0', '0', '0', '0'};
	char cur_buttons[6] = {0};

	int ret = read(g_buttonfd, cur_buttons, sizeof(cur_buttons));
	if (-1 == ret)
	{
		perror("read");
	}

	int i;
	for (i = 0; i < 6; i++)
	{
		if (buttons[i] != cur_buttons[i])
		{
			return i + 1;
		}
	}
	return 0;
}
