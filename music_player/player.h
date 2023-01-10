#ifndef PLAYER_H
#define PLAYER_H

#include <unistd.h>

//#define MUSICPATH   "/root/music_list/"
#define MUSICPATH   "/mount/usb/"
#define SHMKEY     1234
#define SHMSIZE    4096

#define SEQUENCEMODE    1
#define RANDOM          2
#define CIRCLE          3

//共享内存数据
struct shm
{
	int play_mode;
	char cur_name[64];
	pid_t ppid;
	pid_t child_pid;
	pid_t grand_pid;
};
typedef struct shm shm;

int InitShm();
void GetMusic();
void start_play();
void stop_play();
void suspend_play();
void continue_play();
void prior_play();
void next_play();
void voice_up();
void voice_down();
void set_mode(int mode);


#endif
