/* This program use for VT100 Console Simulator */
/* Programmed by S. Thanwa */
#if defined (__CONIO_H)
#else
#define  __CONIO_H
#include <stdio.h>

#define BLACK   0
#define RED     1
#define GREEN   2
#define YELLOW  3
#define BLUE    4
#define MAGENTA 5
#define CYAN    6
#define WHITE   7

int   FOREcolor=7,BACKcolor=0;
char  ESC = 0x1b;

/* Prototype section */
/*================================================================
void clrscr();
void gotoxy(int x,int y);

void textcolor(int color);       => Not ready 
void textbackground(int color);  => Not ready
void lowvideo(void);
void highvideo(void);
void reversevideo(void);

void opencolor();                => Not ready
void openmono();                 => Not ready

void putch(int ch);
int  getch(void);
int  kbhit(void);
================================================================*/
/* Program section */

void clrscr()
{
    printf("%c[2J",ESC);
}

void gotoxy(int x,int y)
{
    printf("%c[%d;%dH",ESC,y,x);
}

void textcolor(int color)
{
    FOREcolor = color;
    printf("%c[%d;%dm",ESC,FOREcolor+30,BACKcolor+40);    
}

void textbackground(int color)
{
    BACKcolor = color;
    printf("%c[%d;%dm",ESC,FOREcolor+30,BACKcolor+40);    
}

void opencolor()
{
    printf("%c[3h",ESC);
}

void openmono()
{
    printf("%c[2h",ESC);
}

#define normvideo lowvideo

void lowvideo()
{
    printf("%c[0m",ESC);
}

void highvideo()
{
    printf("%c[1m",ESC);
}

void reversevideo()
{
    printf("%c[7m",ESC);
}

void putch(int ch)
{
    fputc(ch,stdout);
}

int  getch()
{   
     return fgetc(stdin);
}

int  kbhit()
{ 
   if(feof(stdin)) return 0;
       else return 1;
}
#endif
