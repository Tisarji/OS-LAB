//////////////////////////////////////////////////
// WINCONIO.H  Win32 Console I/O emulation 
// This code emulate classic Console I/O functions
// for DOS console in a new Win32 console application. 
//////////////////////////////////////////////////
// This is an unsafe code to be used in C++
//////////////////////////////////////////////////

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <wincon.h>

#define		BLACK	0
#define		BLUE	1
#define		GREEN	2
#define		CYAN	3
#define		RED		4
#define		MAGENTA	5
#define		BROWN	6
#define		LIGHTGRAY	7
#define		DARKGRAY	8
#define		LIGHTBLUE	9
#define		LIGHTGREEN	10
#define		LIGHTCYAN	11
#define		LIGHTRED		12
#define		LIGHTMAGENTA	13
#define		YELLOW			14
#define		WHITE			15

void clrscr(){
  COORD coordScreen = { 0, 0 };
  DWORD cCharsWritten;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD dwConSize;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  GetConsoleScreenBufferInfo(hConsole, &csbi);
  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  FillConsoleOutputCharacter(hConsole, TEXT(' '),
                             dwConSize,
                             coordScreen,
                             &cCharsWritten);
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  FillConsoleOutputAttribute(hConsole,
                             csbi.wAttributes,
                             dwConSize,
                             coordScreen,
                             &cCharsWritten);
  SetConsoleCursorPosition(hConsole, coordScreen);
}

void gotoxy(int x, int y){
  COORD point;
  point.X = x; point.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
                           point);
}

void textcolor(int color){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	WORD newcolor=0;

	if(color&0x01) newcolor|=FOREGROUND_BLUE;
	if(color&0x02) newcolor|=FOREGROUND_GREEN;
	if(color&0x04) newcolor|=FOREGROUND_RED;
	if(color&0x08) newcolor|=FOREGROUND_INTENSITY;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
	csbi.wAttributes&=(BACKGROUND_INTENSITY | BACKGROUND_RED |
		               BACKGROUND_GREEN | BACKGROUND_BLUE);
	csbi.wAttributes|=newcolor;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		csbi.wAttributes);
}

void textbackground(int color){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	WORD newcolor=0;

	if(color&0x01) newcolor|=BACKGROUND_BLUE;
	if(color&0x02) newcolor|=BACKGROUND_GREEN;
	if(color&0x04) newcolor|=BACKGROUND_RED;
	if(color&0x08) newcolor|=BACKGROUND_INTENSITY;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
	csbi.wAttributes&=(FOREGROUND_INTENSITY | FOREGROUND_RED |
		               FOREGROUND_GREEN | FOREGROUND_BLUE);
	csbi.wAttributes|=newcolor;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		csbi.wAttributes);
}

void textattr(int color){
	textcolor(color&0x0f);
	textbackground((color&0xf0)>>4);
}

void highvideo(void){
	textattr(WHITE);
}

void normvideo(void){
	textattr(LIGHTGRAY);
}

void lowvideo(void){
	normvideo();
}

void consoletitle(LPCTSTR title){
  SetConsoleTitle(title);
}