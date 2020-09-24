#ifndef _TETRIS_HPP
#define _TETRIS_HPP

#ifndef IOSTREAM
#define IOSTREAM
#include <iostream>
#endif

#ifndef _CONIO_H
#define _CONIO_H
#include "conio.h"
#endif

#ifndef _WINDOWS_H
#define _WINDOWS_H
#include <windows.h>
#endif

#ifndef _TIME_H
#define _TIME_H
#include "time.h"
#endif

#define row 17		//至少设定为17，其中0行、row-1行用于划定游戏窗口
#define col 28		//至少设定为23，后14列用于游戏提示（下一个图形、当前等级、当前得分），
					//前col-14列用于划定游戏窗口（其中2列用于边界）
#define dim 5		//方阵维度，用于定义方块形状
#define timeSlice 16

void SetCOORD(int posX, int posY);
class Game;

class Block {	//定义方块的形状的基类（O、I、L、J、Z、S、T共7种不同形状）
public:
	Block();
	virtual void Turn();	//逆时针旋转矩阵shape[5][5]，旋转轴为中心点
	virtual void AntiTurn();		//顺时针旋转
	int mPosX, mPosY;	//mShape[2][2]在Game类中mInterface数组的x坐标为mPosX
	int Shape(int x, int y) const {
		return mShape[x][y];
	}
protected:
	int mShape[5][5];
	void InitTurn();	//设置方块出现时的初始旋转次数，以增加随机性
};

class OShape : public Block {
public:
	OShape();
	virtual void Turn() override { return; }	//旋转并不改变形状
	virtual void AntiTurn() override { return; }
};

class IShape : public Block {
public:
	IShape();
};

class LShape : public Block {
public:
	LShape();
};

class JShape : public Block {
public:
	JShape();
};

class ZShape : public Block {
public:
	ZShape();
};

class SShape : public Block {
public:
	SShape();
};

class TShape : public Block {
public:
	TShape();
};

class Game {
public:
	Game();
	~Game();
	void DrawInterface();
	char Welcome();
	bool Pause();	//如果返回true，则开始新的一盘游戏
	bool Lose();	//同上
	void Close();
	bool Elimination();		//true代表有至少一行被消除了
	bool IsMovable(char dir);	//判断W、S、A、D操作是否可行
	void Run();
private:
	int mLevel, mCnt;	//每当mCnt等于5时，mLevel加一，且mCnt归0
	int mScore, mTopScore;
	int mInterface[row][col];
	Block *mCurBlock, *mNextBlock;
	void AttachBlock(Block *block, bool flag = false);		//把mShape方阵打印进mInterface矩阵中
	void DetachBlock(Block *block, bool flag = false);
	Block *CreateBlock();	//随机选择7种方块形状中的一种（从7个派生类中随机选择）
	void BlocksInit();		//初始化即将下落的Block和下一个Block（包括设置初始位置）
	void SetInitPos(Block *blockCur, Block *blockNext);	//保证新方块刚下落时，只占游戏窗口里的一行，
									//即I型方块刚开始下落时，只会在游戏窗口里显示其1/4的长度
};
#endif
