#ifndef CERTI_WIN 
#define CERTI_WIN

#ifdef WIN32
typedef unsigned short		ushort;

#define  strcasecmp			strcmp
#define	sleep(a)				Sleep(a * 1000)
#define	usleep(a)			Sleep(a / 1000)
#define	STAT_FUNCTION		_stat
#define	STAT_STRUCT			struct _stat

typedef	int					pid_t;

#include	<algorithm>
#include	<process.h>
#else
#define	STAT_FUNCTION		stat
#define	STAT_STRUCT			struct stat
#endif
#endif
