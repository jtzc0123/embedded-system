#include "draw.h"
int main(int argc, char* argv[])
{
	second();
	clearscreen();
	setbkcolor(4);
	setcolor(2);
	for (;;)
	{
		int i;
		for (i = 0; i <= 239; i++)
		{
			setbkcolor(4);
			setcolor(2);
			circle(400, 240, i);
		}
		for (i = 239; i >= 0; i--)
		{
			setbkcolor(4);
			setcolor(2);
			circle(400, 240, i);
		}
	}
	//printlcd(200,200,"photo");
	//printlcd(200,300,"hello");
	//display_bmp("a1.bmp");
	return 0;
}