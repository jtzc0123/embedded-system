#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/soundcard.h>
#include <unistd.h>
#include <pthread.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
#include <linux/fb.h>
#include<sys/mman.h>
#define lenth 10
#define rate 44100
#define size 16
#define channel 2
#define pix_x 800
#define pix_y 480
#define pix_sz 4
#define RED 1
#define YELLOW 2
#define GREEN 3
#define BLUE 4
#define WHITE 5
#define BLACK 6
unsigned char buf[lenth*rate*size*channel / 8];
unsigned char buffer[100];
static unsigned char RGB[4];
int fd;
unsigned char *addr;
struct fb_var_screeninfo vinfo; // #include <linux/fb.h>
int style = 0;
char choice = 0;
int flag_stop = 1;
int flag = 1;
int left = 80, right = 80;
int vol_set(int left, int right)//setting voice volume
{
	int arg;
	int ret_val;
	int fd_mixer;
	fd_mixer = open("/dev/mixer", O_RDWR);
	arg = (right << 8 & 0xff00) | (left & 0x00ff);
	ret_val = ioctl(fd_mixer, MIXER_WRITE(SOUND_MIXER_VOLUME), &arg);
	return ret_val;
}
int screensize = 0;

void initset(void)
{
	int fd;
	// int screensize=0;
	fd = open("/dev/fb", O_RDWR);
	ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
	screensize = vinfo.bits_per_pixel*vinfo.xres*vinfo.yres / 8;
	addr = (unsigned char *)mmap(0, screensize, \
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}
void clear(void)
{
	bzero(addr, pix_x*pix_y*pix_sz);
}
void setcolor(unsigned char COLOR)
{
	RGB[3] = 255;
	switch (COLOR)
	{
	case 1: {RGB[0] = 255; RGB[1] = 0; RGB[2] = 0; break; }
	case 2: {RGB[0] = 238; RGB[1] = 253; RGB[2] = 49; break; }
	case 3: {RGB[0] = 0; RGB[1] = 255; RGB[2] = 0; break; }
	case 4: {RGB[0] = 0; RGB[1] = 0; RGB[2] = 255; break; }
	case 5: {RGB[0] = 255; RGB[1] = 255; RGB[2] = 255; break; }
	case 6: {RGB[0] = 0; RGB[1] = 0; RGB[2] = 0; RGB[3] = 0; break; }
	default: {RGB[0] = 255; RGB[1] = 255; RGB[2] = 255; break; }
	}
}
void dot(int x, int y)
{
	*(addr + pix_sz * (x + y * pix_x) + 0) = RGB[0];
	*(addr + pix_sz * (x + y * pix_x) + 1) = RGB[1];
	*(addr + pix_sz * (x + y * pix_x) + 2) = RGB[2];
	*(addr + pix_sz * (x + y * pix_x) + 3) = RGB[3];
}
void setbkcolor(unsigned char COLOR)

{
	int x, y;
	setcolor(COLOR);
	for (x = 0; x < pix_x; x++)
	{
		for (y = 0; y < pix_y; y++)
		{
			dot(x, y);
		}
	}
}
void key()
{
	char c;
	while (1)
	{
		c = getchar();
		switch (c)
		{
		case '1':choice = '1'; break;
		case '2':choice = '2'; break;
		case '3':choice = '3'; break;
		case '4':
		{
			printf("the left volume is %d%%,the right is
				%d%%\n",left,right);
				if (left < 100)left += 10;
				else printf("warn:highest volume");
				if (right < 100)right += 10;
				else printf("warn:highest volume");
				vol_set(left, right);
				printf("the left volume is %d%%,the right is
					%d%%\n",left,right);
		}break;
		case '5':
		{
			printf("the left volume is %d%%,the right is
				%d%%\n",left,right);
				if (left > 0)left -= 10;
				else printf("warn:lowest volume");
				if (right > 0) right -= 10;
				else printf("warn:lowest volume");
				vol_set(left, right);
				printf("the left volume is %d%%,the right is
					%d%%\n",left,right);
		}break;
		case '6':

			flag = 0;
			break;
		default:break;
		}
	}
}
int main()
{
	initset();
	clear();
	setbkcolor(6);
	setcolor(5);
	int fd_dsp, fd_mixer, status;
	int arg;
	int vol;
	pthread_t keyscanf;
	pthread_create(&keyscanf, NULL, (void *)key, NULL);
	fd_dsp = open("/dev/dsp", O_RDWR);
	if (fd_dsp < 0)
	{
		printf("open /dev/dsp failed\n");
		exit(1);
	}
	fd_mixer = open("/dev/mixer", O_RDONLY);
	if (fd_mixer < 0)
	{
		printf("open /dev/mixer failed\n");
		exit(2);
	}
	arg = size;
	status = ioctl(fd_dsp, SOUND_PCM_WRITE_BITS, &arg);
	if (status == -1 || arg != size)printf("unable to set sample size\n");
	arg = rate;
	status = ioctl(fd_dsp, SOUND_PCM_WRITE_RATE, &arg);
	if (status == -1 || arg != rate)printf("unable to set rate\n");
	vol = (right << 8 & 0xff00) | (left & 0x00ff);
	ioctl(fd_mixer, MIXER_WRITE(SOUND_MIXER_VOLUME), &vol);
	vol = 88;
	ioctl(fd_mixer, MIXER_WRITE(SOUND_MIXER_PCM), &vol);
	vol = 100;
	ioctl(fd_mixer, MIXER_WRITE(SOUND_MIXER_MIC), &vol);

	vol = 100;
	ioctl(fd_mixer, MIXER_WRITE(SOUND_MIXER_IGAIN), &vol);
	flag = 1;
	printf("#################################\n");
	printf("please input 1 to play wav\n");
	printf("please input 2 to record sounds\n");
	printf("please input 3 to replay sounds\n");
	printf("please input 4 to up the vol\n");
	printf("please input 5 to lower the vol\n");
	printf("please input 6 to quit the menu\n");
	printf("################################\n");
	while (1)
	{
		switch (choice)
		{
		case '1':
		{
			FILE *wav;
			wav = fopen("hello.wav", "r");
			puts("open success\n");
			while (!feof(wav))
			{
				fread(buffer, sizeof(buffer), 1, wav);
				write(fd_dsp, buffer, sizeof(buffer));
				if (flag == 0)break;
			}
			printf("finish playing\n");
			break;
		case '2':
		{
			printf("\nsay something:\n");
			status = read(fd_dsp, buf, sizeof(buf));
			int x = 0, i = 0;
			for (x = 0; x < 800; x = x + 2)
			{
				dot(x, 240 + buf[i] / 3); i = i + 2;
				usleep(100000);
				if (x >= 798)
				{
					x = 0;
					clear();
					setbkcolor(6);
					setcolor(5);
				}

				if (i == sizeof(buf))
					break;
				if (x < 0 || 240 + buf[i] / 3>480 || 240 + buf[i] / 3 < 0)
				{
					printf("wrong"); exit(-1);
				}
			}
			if (status != sizeof(buf))
				perror("\nread wrong number of bytes");
		}
		break;
		case '3':
		{
			printf("you said:\n");
			status = write(fd_dsp, buf, sizeof(buf));
			if (status != sizeof(buf))
				perror("\nwrite wrong number of bytes");
			status = ioctl(fd_dsp, SOUND_PCM_SYNC, 0);
			if (status == -1)
				perror("\nSOUND_PCM_SYNC ioctl failed");
		}
		break;
		case '6':
			flag = 0;
			break;
		default:break;
		}
		close(fd_dsp);
		close(fd_mixer);
		return 0;
		}
	}
}