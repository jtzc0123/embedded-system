#include <stdio.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include<unistd.h>
#define PI 3.1415
unsigned char RGB[4];
int fd;
unsigned char* fb;
struct fb_var_screeninfo vinfo; // #include <linux/fb.h>
int style = 0;
int fbsize;
void second(void) //initial
{
	int fd;
	int screensize = 0;
	fd = open("/dev/fb", O_RDWR);//open device
	ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
	printf("bits_per_pixel:%d\n", vinfo.bits_per_pixel);
	printf("xres:%d\n", vinfo.xres);
	printf("yres:%d\n", vinfo.yres);
	screensize = vinfo.bits_per_pixel*vinfo.xres*vinfo.yres / 8;
	printf("screensize:%d\n", screensize);
	fbsize = screensize * 8;

	fb = (unsigned char *)mmap(0, screensize, \
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//arg1: mapping device file, 0 means kernel decide mapping to which memory space
	//arg2: screensize, the size of the framebuffer
	//arg3: permission:read and write
	//arg4: the change of mapping will be writen to the decide file and show on screen
	//arg5: fd as the mapped file descriptor 
	//arg6: offset address 0, from the start of fd mapping to memeory

}
void clearscreen(void)
{
	bzero(fb, 800 * 480 * 4); when initial make the whole screen to be zero
}
display_bmp(char* path) image show function
{
	int bmp_fd, x, y;
	char buf[3] = { 0 };
	if ((bmp_fd = open(path, O_RDONLY, 0666)) < 0)
	{
		fprintf(stderr, "Cannot open file %s", path);
		exit(1);
	}
	lseek(bmp_fd, 54, SEEK_SET);
	for (y = vinfo.yres - 1; y >= 0; --y)
	{
		for (x = 0; x < vinfo.xres; ++x)
		{
			read(bmp_fd, buf, 3);
			memcpy(fb + 4 * (x + y * vinfo.xres), buf, 4);
		}
	}
	close(bmp_fd);
}
void point(int x, int y) show define color before draw the point
{
	*(fb + 4 * (x + y * vinfo.xres) + 0) = RGB[0];
	*(fb + 4 * (x + y * vinfo.xres) + 1) = RGB[1];
	*(fb + 4 * (x + y * vinfo.xres) + 2) = RGB[2];
	*(fb + 4 * (x + y * vinfo.xres) + 3) = RGB[3];
}
void line(int x1, int y1, int x2, int y2)

{
	int dx, dy;
	int x, y, i;
	int temp;
	int p;
	if (x2 < x1) //to achieve bresenham algo first decide to point's location
	{
		temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
	}
	dx = x2 - x1;
	dy = y2 - y1;
	//****************slope=0*******************
	if (dy == 0)
	{
		for (x = x1; x <= x2; ++x)
		{
			point(x, y1);
		}
		return;
	}
	//****************no slope*******************
	if (dx == 0)
	{
		if (y1 < y2)
		{
			for (y = y1; y <= y2; ++y)
			{
				point(x1, y);
			}
			return;
		}
		else
		{
			for (y = y2; y <= y1; ++y)
			{

				point(x1, y);
			}
			return;
		}
	}
	if (dx == dy)
	{
		for (i = 0; i < (x2 - x1); ++i)
		{
			point(x1 + i, y1 + i);
		}
		return;
	}
	//****************slope=-1*******************
	if (dx + dy == 0)
	{
		for (i = 0; i < (x2 - x1); ++i)
		{
			point(x1 + i, y1 - i);
		}
		return;
	}
	//***************0<slope<1******************
	if (dy < dx && dy > 0)
	{
		x = x1;
		y = y1;
		point(x1, y1);
		p = 2 * dy - dx;
		for (i = 0; i < (x2 - x1); ++i)
		{
			if (p < 0)
			{
				point(++x, y);
				p += 2 * dy;
			}
			else
			{
				point(++x, ++y);
				p += 2 * dy - 2 * dx;
			}
		}

		return;
	}
	if (dy > dx && dy > 0)
	{
		//********slope>1********************
		x = x1;
		y = y1;
		point(x1, y1);
		p = 2 * dx - dy;
		for (i = 0; i < (y2 - y1); ++i)
		{
			if (p < 0)
			{
				point(x, ++y);
			}
			else
			{
				point(++x, ++y);
			}
			p += 2 * dx;
		}
		return;
	}
	//**********-1<slope<0****************
	if ((-dy) < dx && dy < 0)
	{
		x = x1;
		y = y1;
		point(x1, y1);
		p = 2 * (-dy) - dx;
		for (i = 0; i < (x2 - x1); ++i)
		{
			if (p < 0)
			{
				point(++x, y);
				p += 2 * (-dy);
			}
			else
			{
				point(++x, --y);
				p += 2 * (-dy) - 2 * dx;
			}

		}
		return;
	}
	if ((-dy) > dx && dy < 0)
	{
		//********slope<-1********************
		x = x1;
		y = y1;
		point(x1, y1);
		p = 2 * dx + dy;
		for (i = 0; i < (y1 - y2); ++i)
		{
			if (p < 0)
			{
				point(x, --y);
				p += 2 * dx;
			}
			else
			{
				point(++x, --y);
				p += 2 * dy - 2 * dx;
			}
		}
		return;
	}
}
void circle(int x, int y, int r) //Midpoint circle algorithm
{
	int x0 = 0, y0 = r;
	float p;
	p = 1.25 - r; //emprical value for initial p0
	point(x0 + x, y0 + y); //draw initial point
	point(y0 + x, x0 + y);
	point(y0 + x, -x0 + y);
	point(x0 + x, -y0 + y);
	point(-x0 + x, -y0 + y);
	point(-y0 + x, -x0 + y);
	point(-y0 + x, x0 + y);
	point(-x0 + x, y0 + y);
	while (x0 < y0)// this loop condition decide the 1/8 circle from the highest point
	{
		p = (x0 + 1)*(x0 + 1) + (y0 - 0.5)*(y0 - 0.5) - r * r;

		if (p < 0)
		{
			++x0;
			point(x0 + x, y0 + y);
			point(y0 + x, x0 + y);
			point(y0 + x, -x0 + y);
			point(x0 + x, -y0 + y);
			point(-x0 + x, -y0 + y);
			point(-y0 + x, -x0 + y);
			point(-y0 + x, x0 + y);
			point(-x0 + x, y0 + y);
			p = p + 2 * x0 + 3;
		}
		else
		{
			++x0;
			--y0;
			point(x0 + x, y0 + y);
			point(y0 + x, x0 + y);
			point(y0 + x, -x0 + y);
			point(x0 + x, -y0 + y);
			point(-x0 + x, -y0 + y);
			point(-y0 + x, -x0 + y);
			point(-y0 + x, x0 + y);
			point(-x0 + x, y0 + y);
			p = p + 2 * x0 + 3 - 2 * y0 - 2;
		}
	}
}
void setcolor(int COLOR) //decide the color by every bit
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
void setbkcolor(int COLOR)

{
	int x, y;
	setcolor(COLOR);
	for (x = 0; x < vinfo.xres; x++)
	{
		for (y = 0; y < vinfo.yres; y++)
		{
			point(x, y);
		}
	}
}
void printlcd(int x, int y, char *s) //show character function
{
	register int i, j, k;
	FILE *fpa, *fph;
	char buffera[16], bufferh[32];
	unsigned char qh, wh;
	unsigned long location;
	if ((fpa = fopen("./asc16", "rb")) == NULL)
		perror("open asc16");
	if ((fph = fopen("./hzk16", "rb")) == NULL)
		perror("open hzk16");
	printf("*s=%d\n", *s);
	while (*s)
	{
		if ((char)(*s) > 0)
		{
			printf("*s>0\n");
			fseek(fpa, (*s) * 16, 0);
			fread(buffera, 1, 16, fpa);
			for (i = 0; i < 16; i++)
				for (j = 0; j < 8; j++)
				{
					if (((buffera[i] >> (7 - j)) & 0x01) != 0)
						point(j + x, i + y);
				}
			s++;
			x += 8;
		}
		else
		{

			qh = *s - 0xa0;
			printf("qh=%d\n", qh);
			wh = *(s + 1) - 0xa0;
			printf("wh=%d\n", wh);
			location = (94 * (qh - 1) + (wh - 1)) * 32L;
			fseek(fph, location, SEEK_SET);
			fread(bufferh, 32, 1, fph);
			for (i = 0; i < 16; i++)
				for (j = 0; j < 2; j++)
					for (k = 0; k < 8; k++)
						if (((bufferh[i * 2 + j] >> (7 - k)) & 0x01) != 0)
							point(x + 8 * j + k, y + i);
			s += 2;
			x += 16;
		}
	}
	fclose(fpa);
	fclose(fph);
}