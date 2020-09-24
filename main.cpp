#include <iostream>
#include "tetris.hpp"

int main(int argc, const char * argv[]) {
	HANDLE hdl = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hdl, &cursorInfo);		//获取控制台光标信息
	cursorInfo.bVisible = false;	//隐藏控制台光标
	SetConsoleCursorInfo(hdl, &cursorInfo);	//设置控制台光标状态
	srand((unsigned int)time(nullptr));
	Game ab;
	ab.Run();
}
