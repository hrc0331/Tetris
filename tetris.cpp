#include "tetris.hpp"

HANDLE handle;
void SetCOORD(int posX, int posY) {
	COORD coord;
	coord.X = posY;	//coord的X和Y分别代表col和row，
	coord.Y = posX;	//与一般认为的X代表row有所出路
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(handle, coord);
}

Block::Block() {
	for (int i = 0; i < dim; ++i)
		for (int j = 0; j < dim; ++j)
			mShape[i][j] = 0;
}

void Block::InitTurn() {	//不适用于OShape
	int initTurn = rand() % 4;
	for (int i = 0; i < initTurn; ++i)
		Turn();
}

OShape::OShape() : Block() {
	mShape[1][2] = 1;
	mShape[1][3] = 1;
	mShape[2][2] = 1;
	mShape[2][3] = 1;
}



IShape::IShape() : Block() {
	mShape[2][1] = 1;
	mShape[2][2] = 1;
	mShape[2][3] = 1;
	mShape[2][4] = 1;
	InitTurn();
}

LShape::LShape() : Block() {
	mShape[2][2] = 1;
	mShape[2][3] = 1;
	mShape[2][4] = 1;
	mShape[3][2] = 1;
	InitTurn();
}

JShape::JShape() : Block() {
	mShape[1][2] = 1;
	mShape[2][2] = 1;
	mShape[2][3] = 1;
	mShape[2][4] = 1;
	InitTurn();
}

ZShape::ZShape() : Block() {
	mShape[2][1] = 1;
	mShape[2][2] = 1;
	mShape[3][2] = 1;
	mShape[3][3] = 1;
	InitTurn();
}

SShape::SShape() : Block() {
	mShape[1][2] = 1;
	mShape[1][3] = 1;
	mShape[2][1] = 1;
	mShape[2][2] = 1;
	InitTurn();
}

TShape::TShape() : Block() {
	mShape[2][1] = 1;
	mShape[2][2] = 1;
	mShape[2][3] = 1;
	mShape[3][2] = 1;
	InitTurn();
}

void Block::Turn() {	//逆时针旋转90度
	for (int i = 0, d = dim - 1; i < 2; ++i, d -= 1) {
		for (int j = i; j < d; ++j) {
			int temp = mShape[i][j];
			mShape[i][j] = mShape[j][4-i];
			mShape[j][4-i] = mShape[4-i][4-j];
			mShape[4-i][4-j] = mShape[4-j][i];
			mShape[4-j][i] = temp;
		}
	}
}

void Block::AntiTurn() {	//顺时针旋转90度
	for (int i = 0, d = dim - 1; i < 2; ++i, d -= 1) {
		for (int j = i; j < d; ++j) {
			int temp = mShape[i][j];
			mShape[i][j] = mShape[4-j][i];
			mShape[4-j][i] = mShape[4-i][4-j];
			mShape[4-i][4-j] = mShape[j][4-i];
			mShape[j][4-i] = temp;
		}
	}
}

Game::Game() : mLevel(0), mCnt(0), mTopScore(0), mScore(0),
				mCurBlock(nullptr), mNextBlock(nullptr) {
	for (int i = 0; i < row; ++i)
		for (int j = 0; j < col; ++j)
			mInterface[i][j] = 0;
}

Game::~Game() {
	delete mNextBlock;
	delete mCurBlock;
}

void Game::AttachBlock(Block* block, bool flag) {
	//mShape数组的[0,0]坐标映射到mInterference数组内的坐标为[x,y]
	int x = block -> mPosX - 2, y = block -> mPosY - 2;
	for (int i = 0; i < dim; ++i) {
		if (i + x < 0 || i + x > row - 1)
			continue;
		for (int j = 0; j < 5; ++j) {
			if (j + y <= 0)
				continue;
			if (block -> Shape(i, j) != 0)
				mInterface[i + x][j + y] = block -> Shape(i, j);
		}
	}
	if (flag == false)
		return;
	for (int i = 0; i < 5; ++i) {	//flag为true时，更新游戏界面的方块
		for (int j = 0; j < 5; ++j) {
			if (block -> Shape(i, j) != 0 && i + x > 0) {
				SetCOORD(i + x, (j + y) * 2);
				std::cout << "■";
			}
		}
	}
}

void Game::DetachBlock(Block* block, bool flag) {
	int x = block -> mPosX - 2, y = block -> mPosY - 2;
	for (int i = 0; i < dim; ++i) {
		if (i + x < 0 || i + x > row - 1)
			continue;
		for (int j = 0; j < dim; ++j) {
			if (j + y <= 0)
				continue;
			if (block -> Shape(i, j) != 0)
				mInterface[i + x][j + y] = 0;
		}
	}
	if (flag == false)
		return;
	for (int i = 0; i < 5; ++i) {	//flag为true时，更新游戏界面的方块
		for (int j = 0; j < 5; ++j) {
			if (block -> Shape(i, j) != 0 && i + x > 0) {
				SetCOORD(i + x, (j + y) * 2);
				std::cout << "  ";	//用2个空格覆盖1个■，因为1个■相当于2个空格的宽度
			}
		}
	}
}

bool Game::IsMovable(char dir) {
	int posX = mCurBlock -> mPosX - 2;	//将方阵mShape中的坐标[0,0]映射到mInterface矩阵
	int posY = mCurBlock -> mPosY - 2;	//同上
	DetachBlock(mCurBlock);
	if (dir == 'a' || dir == 'A') {
		int tempY = posY - 1;
		for (int j = 0; j < 3; ++j) {	//只扫描3/5列就足够了，因为移动前的位置是合法的
			for (int i = 0; i < dim; ++i) {
				if (posX + i <= 0)	//为了让方块刚生成下落时，可以左右移动，提升手感
					continue;
				if (mCurBlock -> Shape(i, j) != 0 &&
					(tempY + j <= 0 || mInterface[posX + i][tempY + j] != 0)) {
					AttachBlock(mCurBlock);
					return false;
				}
			}
		}
	} else if (dir == 'd' || dir == 'D') {
		int tempY = posY + 1;
		for (int j = dim - 1; j > 1; --j) {
			for (int i = 0; i < dim; ++i) {
				if (posX + i <= 0)
					continue;
				if (mCurBlock -> Shape(i, j) != 0 &&
					(tempY + j >= col - 14 - 1 || mInterface[posX + i][tempY + j] != 0)) {
					AttachBlock(mCurBlock);
					return false;
				}
			}
		}
	} else if (dir == 's' || dir == 'S') {
		int tempX = posX + 1;
		for (int i = dim - 1; i > 1; --i) {
			int x = tempX + i;
			for (int j = 0; j < dim; ++j) {
				if (mCurBlock -> Shape(i, j) != 0 && (x >= row - 1 ||
					 (x >= 0 && mInterface[x][posY + j] != 0))) {
					AttachBlock(mCurBlock);
					return false;
				}
			}
		}
	} else if (dir == 'w' || dir == 'W') {
		mCurBlock -> Turn();
		//阻止旋转操作发生后，出现进入墙体、重复占有已经存在的方块的情况
		for (int i = 0; i < dim; ++i) {
			for (int j = 0; j < dim; ++j) {
				if (mCurBlock -> Shape(i, j) != 0 &&
					(posX + i >= row - 1 ||
					 posY + j <= 0 || posY + j >= col - 14 - 1 ||
					 mInterface[posX + i][posY + j] != 0)) {
					mCurBlock -> AntiTurn();
					AttachBlock(mCurBlock);
					return false;
				}
			}
		}
		//为了让方块刚生成下落时，可以旋转，提升手感
		for (int i = dim - 1; i >= 0; --i) {
			if (posX + i <= 0)
				continue;
			for (int j = 0; j < dim; ++j) {
				if (mCurBlock -> Shape(i, j) != 0) {
					mCurBlock -> AntiTurn();
					return true;
				}
			}
		}
		mCurBlock -> AntiTurn();
		return false;
	}
	return true;
}

void Game::SetInitPos(Block *blockCur, Block *blockNext) {
	int xCorrect = 1;
	for (int i = 3; i < dim; ++i)
		for (int j = 0; j < dim; ++j)
			if (blockCur -> Shape(i,j) != 0) {
				--xCorrect;
				break;
			}
	blockCur -> mPosX = xCorrect;
	int yCorrect = 0;
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < dim; ++i) {
			if (blockCur -> Shape(i,j) != 0) {
				goto here1;
			}
		}
		--yCorrect;
	}
	here1:
	for (int j = 4; j > 2; --j) {
		for (int i = 0; i < dim; ++i) {
			if (blockCur -> Shape(i,j) != 0) {
				goto here2;
			}
		}
		++yCorrect;
	}
	here2:
	blockCur -> mPosY = (col - 14 - 1) / 2 + yCorrect / 2;
	blockNext -> mPosX = 4;
	blockNext -> mPosY = col - 14 + 5;
}

//随机选择7种方块形状中的一种
template<typename T>
Block *Create() {
	return new T;
}
typedef Block* (*CreateFP)();
CreateFP createArr[] = {
	&Create<OShape>,
	&Create<IShape>,
	&Create<LShape>,
	&Create<JShape>,
	&Create<ZShape>,
	&Create<ZShape>,	//出现的几率提高
	&Create<SShape>,
	&Create<TShape>
};	//在createArr数组中，可以通过增加某一个类的数量，来增加其出现的概率
Block *Game::CreateBlock() {
	return createArr[rand() % 8]();		//8是代表createArr数组的长度
}

void Game::BlocksInit() {
	if (mCurBlock != nullptr)
		delete mCurBlock;
	if (mNextBlock != nullptr)
		delete mNextBlock;
	mCurBlock = CreateBlock();		//记住需要delete
	mNextBlock = CreateBlock();		//同上
	SetInitPos(mCurBlock, mNextBlock);
	AttachBlock(mCurBlock);
	AttachBlock(mNextBlock);
}

void Game::DrawInterface() {
	for (int i = 1; i < row - 1; ++i) {
		for (int j = 1; j < col; ++j) {
			if (j == col - 14 - 1)
				continue;
			SetCOORD(i, j * 2);
			std::cout << "  ";
		}
	}
	for (int i = 0; i < dim; ++i) {
		for (int j = 0; j < dim; ++j) {
			if (mNextBlock -> Shape(i, j) != 0) {
				SetCOORD(mNextBlock -> mPosX - 2 + i, (mNextBlock -> mPosY - 2 + j) * 2);
				std::cout << "■";
			}
		}
	}
	for (int i = 1; i < row - 1; ++i) {
		for (int j = 0; j <= col - 14; ++j) {
			if (mInterface[i][j] != 0) {
				SetCOORD(i, j * 2);
				std::cout << "■";
			}
			else if (i == 1 && j == col - 14) {
				SetCOORD(i, j * 2 + 4);
				std::cout << "next :";
			}
			else if (i == 7 && j == col - 14) {
				SetCOORD(i, j * 2 + 4);
				std::cout << "level : " << mLevel;
			}
			else if (i == 9 && j == col - 14) {
				SetCOORD(i, j * 2 + 4);
				std::cout << "score : ";
			}
			else if (i == 10 && j == col - 14) {
				SetCOORD(i, j * 2 + 4);
				std::cout << "   " << mScore;
			}
			else if (i == 12 && j == col - 14) {
				SetCOORD(i, j * 2 + 4);
				std::cout << "top score :";
			}
			else if (i == 13 && j == col - 14) {
				SetCOORD(i, j * 2 + 4);
				std::cout << "   " << mTopScore;
			}
			else if (i == 15 && j == col - 14) {
				SetCOORD(i, j * 2 + 4);
				std::cout << "pause : key P";
			}
		}
	}
}

char Game::Welcome() {
	system("cls");
	int yMidOpt = (col - 14 + 1);	//为了让选项居中
	int xMidOpt = (row + 1) / 2;
	for (int k = 0; k < col - 14; ++k) {
		SetCOORD(0, k * 2);		//k乘以2是因为：1个■相当于2个空格的宽度
		std::cout << "□";
	}
	for (int i = 1; i < row - 1; ++i) {
		SetCOORD(i, 0);
		std::cout << "□";
		SetCOORD(i, (col - 14) * 2 - 2);
		std::cout << "□";
		if (i == xMidOpt - 7) {
			SetCOORD(i, yMidOpt - 8);	//数字8是为了居中的目的
			std::cout << "■  welcome  ■ ";
		}
		else if (i == xMidOpt - 5) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "■  control  ■ ";
		}
		else if (i == xMidOpt - 4) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "□  W S A D  □ ";
		}
		else if (i == xMidOpt - 2) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "■   start   ■ ";
		}
		else if (i == xMidOpt - 1) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "□ key space □ ";
		}
		else if (i == xMidOpt + 1) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "■  level " << mLevel << "  ■ ";
		}
		else if (i == xMidOpt + 2) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "□  key 0~9  □ ";
		}
		else if (i == xMidOpt + 4) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "■ quit game ■ ";
		}
		else if (i == xMidOpt + 5) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "□  key ESC  □ ";
		}
	}
	for (int k = 0; k < col - 14; ++k) {
		SetCOORD(row - 1, k * 2);
		std::cout << "□";
	}
	char input;
	while (1) {
		input = 0;
		while (!input) {
			if (!_kbhit()) {
				Sleep(100);
				continue;
			}
			input = _getch();
		}
		if (input == '\040' || input == '\033' || (input >= '0' && input <= '9'))
			break;
	}
	return input;
}

void Game::Close() {
	system("cls");
	exit(0);
}

bool Game::Pause() {
	int j = col - 14 + 2;
	for (int i = 1; i < row - 1; ++i) {
		SetCOORD(i, j * 2);
		std::cout << "             ";	//13个空格，足以清空右侧窗口
	}
	for (int i = 1; i < row - 1; ++i) {
		if (i == 2) {
			SetCOORD(i, j * 2);
			std::cout << "■   game   ■";
		}
		else if (i == 3) {
			SetCOORD(i, j * 2);
			std::cout << "■  paused  ■";
		}
		else if (i == 5) {
			SetCOORD(i, j * 2);
			std::cout << "□  resume  □";
		}
		else if (i == 6) {
			SetCOORD(i, j * 2);
			std::cout << "□  key  R  □";
		}
		else if (i == 8) {
			SetCOORD(i, j * 2);
			std::cout << "■ new game ■";
		}
		else if (i == 9) {
			SetCOORD(i, j * 2);
			std::cout << "■  key  N  ■";
		}
		else if (i == 11) {
			SetCOORD(i, j * 2);
			std::cout << "□   quit   □";
		}
		else if (i == 12) {
			SetCOORD(i, j * 2);
			std::cout << "□ key  ESC □";
		}
	}
	char input;
	while (1) {
		input = 0;
		while (!input) {
			if (!_kbhit()) {
				Sleep(100);
				continue;
			}
			input = _getch();
		}
		if (input == 'r' || input == 'R') {
			return false;
		} else if (input == 'n' || input == 'N') {
			return true;
		} else if (input == '\033') {
			Close();
		}
	}
}

bool Game::Lose() {
	for (int i = 1; i < row - 1; ++i) {
		for (int j = 1; j < col - 14 - 1; ++j) {	//清空左侧窗口
			SetCOORD(i, j * 2);
			std::cout << "  ";
		}
		SetCOORD(i, (col - 14 + 2) * 2);
		std::cout << "             ";	//清空右侧窗口
	}
	int yMidOpt = (col - 14 + 1);
	int xMidOpt = (row + 1) / 2;
	for (int i = 1; i < row - 1; ++i) {
		if (i == xMidOpt - 4) {
			SetCOORD(i, yMidOpt - 8);		//数字8是为了选项居中的目的
			std::cout << "■ game over ■ ";
		}
		else if (i == xMidOpt - 2) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "■ new  game ■ ";
		}
		else if (i == xMidOpt - 1) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "□   key N   □ ";
		}
		else if (i == xMidOpt + 1) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "□ quit game □ ";
		}
		else if (i == xMidOpt + 2) {
			SetCOORD(i, yMidOpt - 8);
			std::cout << "■  key ESC  ■ ";
		}
	}
	int j = col - 14 + 2;
	SetCOORD(1, j * 2);
	std::cout << "your score :";
	SetCOORD(2, j * 2 + 2);
	std::cout << mScore;
	SetCOORD(4, j * 2);
	std::cout << "top score :";
	SetCOORD(5, j * 2 + 2);
	std::cout << mTopScore;
	char input;
	while (1) {
		input = 0;
		while (!input) {
			if (!_kbhit()) {
				Sleep(100);
				continue;
			}
			input = _getch();
		}
		if (input == 'n' || input == 'N') {
			return true;
		} else if (input == '\033') {
			Close();
		}
	}
	return false;
}

bool Game::Elimination() {
	int rowCandidate[4] = {-1, -1, -1, -1};
	int posX = mCurBlock -> mPosX - 2;
	int posY = mCurBlock -> mPosY - 2;
	//找出方块所占的行数，并记录进rowCandidate数组
	for (int i = dim - 1, k = 0; i >= 0; --i) {
		if (posX + i <= 0 || posX + i >= row - 1)
			continue;
		for (int j = 0; j < dim; ++j) {
			if (posY + j <= 0 || posY + j >= col - 14 - 1)
				continue;
			if (mCurBlock -> Shape(i, j) != 0) {
				rowCandidate[k++] = posX + i;
				break;
			}
		}
	}
	//从rowCandidate数组中筛选出可以被清除的行
	//不能被清除的行，其rowCandidate[k]的值将被设为-1
	for (int k = 0; k < 4 && rowCandidate[k] != -1; ++k) {
		for (int j = 1; j <= col - 14 - 2; ++j) {
			if (mInterface[rowCandidate[k]][j] == 0) {
				rowCandidate[k] = -1;
				break;
			}
		}
	}
	//清除rowCandidate数组中所列出的行（用值非-1列出）
	int numEli = 0;
	for (int k = 0; k < 4; ++k) {
		if (rowCandidate[k] == -1)
			continue;
		for (int j = 1; j <= col - 14 - 2; ++j) {
			for (int i = rowCandidate[k] + numEli; i > numEli; --i) {
				mInterface[i][j] = mInterface[i-1][j];
			}
		}
		++numEli;
		mScore += (mLevel + 1);	//记分
		mTopScore = (mTopScore > mScore) ? mTopScore : mScore;
		if (mLevel < 9 && ++mCnt == 5) {	//更新等级
			mLevel += 1;
			mCnt = 0;
		}
	}
	if (numEli == 0)
		return false;
	else
		return true;
}

void Game::Run() {
	//开启Welcome界面，并根据选项做不同的反应
	char input = Welcome();
	while (input >= '0' && input <= '9') {	//选择难度
		mLevel = input - '0';
		input = Welcome();
		Sleep(100);
	}
	if (input == '\033') {
		Close();
	}
	//开始游戏
	int initLevel = mLevel;		//以便在pause界面开启新游戏时，恢复welcome界面所设置的难度
	BlocksInit();
	DrawInterface();
	int fallCycle = 38 - mLevel * 2;	//无键盘输入时，每fallCycle * time毫秒下落一格
	int speedUp = 8 - mLevel / 2;	//按住S时，下落速度逐渐加快，越低的难度加速越快
	while (1) {
		if (fallCycle <= 0) {
			if (IsMovable('s')) {
				DetachBlock(mCurBlock, true);
				mCurBlock -> mPosX += 1;
				AttachBlock(mCurBlock, true);
			} else {
				//判断是否输了
				bool flag = false;	//假设没输，通过后续判断决定是否推翻
				for (int j = 1; j <= col - 14 - 2; ++j) {
					if (mInterface[1][j] != 0) {
						flag = true;
						break;
					}
				}
				if (flag && Lose()) {
					mLevel = initLevel;
					mScore = 0;
					for (int i = 0; i < row; ++i)
						for (int j = 0; j < col; ++j)
							mInterface[i][j] = 0;
					BlocksInit();
					DrawInterface();
				} else if (Elimination() == true) {
					DetachBlock(mNextBlock);
					delete mCurBlock;
					mCurBlock = mNextBlock;
					mNextBlock = CreateBlock();
					SetInitPos(mCurBlock, mNextBlock);
					DrawInterface();
				} else {
					DetachBlock(mNextBlock, true);
					delete mCurBlock;
					mCurBlock = mNextBlock;
					mNextBlock = CreateBlock();
					SetInitPos(mCurBlock, mNextBlock);
					AttachBlock(mCurBlock, true);
					AttachBlock(mNextBlock, true);
				}
			}
			fallCycle = 38 - mLevel * 2;
			continue;
		}
		input = 0;
		Sleep(timeSlice);	//level为0时，默认xx次循环下落一格
		--fallCycle;
		if (!_kbhit()) {
			continue;
		}
		input = _getch();
		if (input == 's' || input == 'S') {
			fallCycle -= speedUp;
			speedUp += 4 - mLevel / 3;
			continue;
		} else if ((input == 'a' || input == 'A') && IsMovable('a')) {
			DetachBlock(mCurBlock, true);
			mCurBlock -> mPosY -= 1;
			AttachBlock(mCurBlock, true);
			fallCycle -= 1;		//补偿数据处理所消耗的时间
		} else if ((input == 'd' || input == 'D') && IsMovable('d')) {
			DetachBlock(mCurBlock, true);
			mCurBlock -> mPosY += 1;
			AttachBlock(mCurBlock, true);
			fallCycle -= 1;
		} else if ((input == 'w' || input == 'W') && IsMovable('w')) {
			DetachBlock(mCurBlock, true);
			mCurBlock -> Turn();
			AttachBlock(mCurBlock, true);
			fallCycle -= 1;
		} else if (input == 'p' || input == 'P') {
			if (!Pause()) {		//Pause返回false时，代表不开启新的游戏，并返回旧游戏
				DrawInterface();
			} else {	//Pause返回true时，开启新的游戏
				mLevel = initLevel;
				mScore = 0;
				for (int i = 0; i < row; ++i)
					for (int j = 0; j < col; ++j)
						mInterface[i][j] = 0;
				BlocksInit();
				DrawInterface();
			}
		}
	}
}
