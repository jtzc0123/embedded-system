#ifndef _DRAW_H_
#define _DRAW_H_
void draw(void);
void point(int x, int y);
void circle(int x, int y, int r);
void line(int x1, int x2, int y1, int y2);
void display_bmp(char* path);
void clearscreen(void);
void setcolor(int COLOR);
void setbkcolor(int COLOR);
void printlcd(int x, int y, char *s);
#endif