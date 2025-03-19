﻿#include <cstdio>
#include "Logger.h"
#include "Process.h"

int CreateLogServer(CProcess* proc)
{
    printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    return 0;
}

int CreateClientServer(CProcess* proc)
{
    printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
	int fd = -1;
	int ret = proc->RecvFD(fd);
	printf("%s(%d):<%s> ret = %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
	printf("%s(%d):<%s> fd = %d\n", __FILE__, __LINE__, __FUNCTION__, fd);
	sleep(1);
	char buf[10] = "";
    lseek(fd, 0, SEEK_SET);
	read(fd, buf, sizeof(buf));
	printf("%s(%d):<%s> buf is: %s\n", __FILE__, __LINE__, __FUNCTION__, buf);
	close(fd);
    return 0;
}

int main()
{
    // CProcess::SwitchDeamon();
    CProcess proclog, procclients;
    printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    proclog.SetEntryFunction(CreateLogServer, &proclog);
    int ret = proclog.CreateSubProcess();
    if (ret != 0) {
        printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
        return -1;
    }
    printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    procclients.SetEntryFunction(CreateClientServer, &procclients);
    ret = procclients.CreateSubProcess();
    if (ret != 0) {
        printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
        return -2;
    }
    printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    //usleep(100 * 000);
	int fd = open("./test.txt", O_RDWR | O_CREAT | O_APPEND);
	printf("%s(%d):<%s> fd = %d\n", __FILE__, __LINE__, __FUNCTION__, fd);
	if (fd == -1) return -3;
	ret = procclients.SendFD(fd);
	printf("%s(%d):<%s> ret = %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
	write(fd, "edoyun", 6);
	close(fd);

    return 0;
}