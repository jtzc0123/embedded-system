#include<stdio.h>
#include<stdlib.h>
#include<linux/fs.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#define INIT 1
#define START 2
#define FRE 3
#define STOP 4
#define f0 0x0968
#define f1 0x0A90
#define f2 0x0BEB
#define f3 0x0D51
#define f4 0x0E16
#define f5 0x0FCF

#define f6 0x11BD
int main()
{
	int fd;
	fd = open("./8253", O_RDWR);
	ioctl(fd, INIT, 0);
	for (;;)
	{
		ioctl(fd, STOP, 0);
		switch (getchar())
		{
		case '7':ioctl(fd, FRE, f0); break;
		case '6':ioctl(fd, FRE, f1); break;
		case '5':ioctl(fd, FRE, f2); break;
		case '4':ioctl(fd, FRE, f3); break;
		case '3':ioctl(fd, FRE, f4); break;
		case '2':ioctl(fd, FRE, f5); break;
		case '1':ioctl(fd, FRE, f6); break;
		default:goto end; break;
		}
		ioctl(fd, START, 0);
		usleep(10000);
	}
end:;
	close(fd);
	return 0;
}