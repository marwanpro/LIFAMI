// Marwan ZOGHLAMI p1608585
// LIFAMI - Démineur

/*
 * Ce démineur a été crée avec Visual Studio 2015 et est fonctionnel 
 * Les 4 première variables sont éditables
 * Le démineur possède une coloration des cases automatiques ainsi que d'un timer et d'un indicateur de drapeau restant
 * Une ébauche non fonctionnel d'un systeme cascade (recursif) lors de la pression d'un 0 est inclus en commentaires
 * Si les #pragma region pose problèmes sous mingw ou gcc,  supprimez les
 *
 */



#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <iostream>
#include <Grapic.h>
#define CHMAX 255


using namespace grapic;


// Global Vars
const int DIMX = 12;
const int DIMY = 12;
const int BOMB = 14;
const bool DEBUG = true;
int glX = 0;
int glY = 0;
int randInt(int min, int max) { return min + (rand() % (int)(max - min + 1)); }

// Grapic Vars
const char title[CHMAX] = "Minesweeper++";
const int SIZE = 32;
const int WINX = 20 * 2 + (DIMX - 2) * SIZE + DIMX;
const int WINY = 80 + (DIMY - 2) * SIZE + DIMY;
bool stop = false;

// Game Vars
clock_t startTime = clock();
int loop = 0;
bool mgameOver = false;
bool mWon = false;



// Struct of a case
struct mCase
{
	int x;
	int y;
	int nearBomb = 0;
	bool isBomb = false;
	bool isVisible = true;
	bool isDisplayed = false;
	bool isFlagged = false;
	bool isScanned = false;

	void init()
	{
		x = glX;
		y = glY;
		nearBomb = 0;
		if (x == 0 || y == 0 || x == DIMX || y == DIMY) isVisible = false;
		isFlagged = false;
		isBomb = false;
		isScanned = false;
	}
};


// Init a double array of mCase
void init(mCase table[DIMX][DIMY])
{
	// Just calling init
	for (glX = 0; glX < DIMX; glX++)
	{
		for (glY = 0; glY < DIMY; glY++)
		{
			table[glX][glY].init();
		}
	}

	// Placing bombs
	for (int i = 0; i < BOMB; i++)
	{
		int x = randInt(1, DIMX - 2);
		int y = randInt(1, DIMY - 2);
		if (table[x][y].isBomb == false && table[x][y].isVisible == true) table[x][y].isBomb = true;
		else i--;
	}

	// Count near bombs
	for (glX = 1; glX < DIMX - 1; glX++)
	{
		//cout << "glX: " << glX;
		for (glY = 1; glY < DIMY - 1; glY++)
		{
			//cout << ", glY: " << glY << endl;
			for (int x = glX - 1; x <= glX + 1; x++)
			{
				for (int y = glY - 1; y <= glY + 1; y++)
				{
					//cout << "Case [" << glX << "][" << glY << "] : Checking (" << x << "," << y << ")" << endl;
					if (table[x][y].isBomb == true)
					{
						table[glX][glY].nearBomb++;
						//cout << "BOMB" << endl;
					}

				}
			}
		}
	}

	// Params vars
	mgameOver = false;
	mWon = false;
}


// Recursive: Domino effect when 0 is hitten
/*
void recursive(mCase table[DIMX][DIMY])
{
	struct coord
	{
		bool active = false;
		int x, y;
	};
	coord queue[10000];
	int i = 0;
	for (glX = 1; glX < DIMX - 1; glX++)
	{
		for (glY = 1; glY < DIMY - 1; glY++)
		{
			if (table[glX][glY].isDisplayed && !table[glX][glY].isFlagged && !table[glX][glY].isScanned && table[glX][glY].nearBomb == 0)
			{
				queue[i].active = true;
				queue[i].x = glX;
				queue[i].y = glY;
				i++;
			}
		}
	}
	for (int j = 0; j < 10000; j++)
	{
		if (queue[j].active)
		{
			table[queue[j].x][queue[j].y].isScanned = true;
			for (int x = queue[j].x - 1; x <= queue[j].x + 1; x++)
			{
				for (int y = queue[j].y - 1; y <= queue[j].y + 1; y++)
				{
					if (table[x][y].isVisible)
					{
						table[x][y].isDisplayed = true;
						if (table[x][y].nearBomb == 0)
						{
							queue[i].active = true;
							queue[i].x = x;
							queue[i].y = y;
							i++;
						}
					}
				}
			}
		}
		queue[j].active = false;
	}
}
*/


// Print in Console 
void mPrint(mCase table[DIMX][DIMY])
{
	std::cout << "To disable cheat, set DEBUG to false in Source Code" << std::endl << std::endl;
	for (glX = 1; glX < DIMX - 1; glX++)
	{
		for (glY = 1; glY < DIMY - 1; glY++)
		{
			if (table[glX][glY].isBomb) std::cout << 'B';
			else std::cout << table[glX][glY].nearBomb;
			std::cout << " ";
		}
		std::cout << std::endl;
	}
}


// Remaining Flags
int mRemaining(mCase table[DIMX][DIMY])
{
	int remaining = BOMB;
	for (glX = 1; glX < DIMX - 1; glX++)
	{
		for (glY = 1; glY < DIMY - 1; glY++)
		{
			if (table[glX][glY].isFlagged) remaining--;
		}
	}
	return remaining;
}


// Game Over (On left click on a mine)
void gameOver(mCase table[DIMX][DIMY])
{
	mgameOver = true;
	for (glX = 1; glX < DIMX - 1; glX++)
	{
		for (glY = 1; glY < DIMY - 1; glY++)
		{
			if (table[glX][glY].isBomb) table[glY][glX].isDisplayed = true;
		}
	}
	std::cout << "LOSE" << std::endl;
	system("cls");
	std::cout << "Vous avez fait explose une mine. Perdu :/" << std::endl;
}


// Check if the game is won 
void checkWin(mCase table[DIMX][DIMY])
{
	int tmp = (DIMX - 2)*(DIMY - 2) - BOMB;
	for (glX = 1; glX < DIMX - 1; glX++)
	{
		for (glY = 1; glY < DIMY - 1; glY++)
		{
			if (table[glX][glY].isDisplayed) tmp--;
		}
	}
	if (tmp == 0) mWon = true;
	if (DEBUG)
	{
		std::cout << "Left Case: " << tmp << std::endl;
		if (tmp == 0) std::cout << "WIN" << std::endl;
	}
}


// End the game
void finish()
{
	if (loop == 0)
	{
		loop++;
		return;
		if (mWon)
		{

		}
		if (mgameOver)
		{

		}
	}
	system("pause");
	winQuit();
	exit(0);
}


// Display Case on Window
void discover(mCase table[DIMX][DIMY], int x, int y)
{
	if (!table[x][y].isDisplayed && !table[x][y].isFlagged) return;
	
	int wX = x + SIZE * x;
	int wY = 30 + -y + SIZE * (DIMY - 2 - y);

	if (table[x][y].isFlagged) {
		color(255, 255, 255);
		print(wX, wY, "F");
	}
	else if (table[y][x].isBomb) {
		color(255, 0, 0);
		print(wX, wY, "B");
	}
	else {
		if (table[y][x].nearBomb == 0) color(0, 215, 0);
		if (table[y][x].nearBomb == 1) color(200, 215, 0);
		if (table[y][x].nearBomb == 2) color(250, 50, 30);
		if (table[y][x].nearBomb == 3) color(250, 270, 20);
		if (table[y][x].nearBomb >= 4) color(250, 0, 30);
		print(wX, wY, table[y][x].nearBomb);
	}
}


// Event Listener: Left Click
void leftClick(mCase table[DIMX][DIMY])
{
	int x, y;
	mousePos(x, y);
	_sleep(150);

	// Check if coords match a case, then get case with MousePos
	#pragma region casecalc
	if (x < 20 || x > WINX - 20 || y < 20 || y > WINY - 60) return;
	if ((x - 20) % (SIZE + 1) == 0 || (y - 20) % (SIZE + 1) == 0) return;

	int caseX = (x - 20) / (SIZE + 1) + 1;
	int caseY = abs(((y - 20) / (SIZE + 1) + 1) - DIMY) - 1;
	#pragma endregion


	if (DEBUG)
	{
		std::cout << "Case(" << caseX << "," << caseY << ")" << std::endl;
	}

	if (!table[caseX][caseY].isFlagged) table[caseX][caseY].isDisplayed = true;

	// Check if case is a Mine or not
	if (!table[caseY][caseX].isFlagged && table[caseY][caseX].isBomb) gameOver(table);
	else checkWin(table);
}


// Event Listener: Right Click
void rightClick(mCase table[DIMX][DIMY])
{
	int x, y;
	mousePos(x, y);
	_sleep(150);

	// Check if coords match a case, then get case with MousePos
	#pragma region casecalc
	if (x < 20 || x > WINX - 20 || y < 20 || y > WINY - 60) return;
	if ((x - 20) % (SIZE + 1) == 0 || (y - 20) % (SIZE + 1) == 0) return;
	
	int caseX = (x - 20) / (SIZE + 1) + 1;
	int caseY = abs(((y - 20) / (SIZE + 1) + 1) - DIMY) - 1;
	#pragma endregion

	if (DEBUG)
	{
		std::cout << "RC" << "Case(" << caseX << "," << caseY << ")" << std::endl;
	}

	if (!table[caseX][caseY].isDisplayed && !table[caseX][caseY].isFlagged) table[caseX][caseY].isFlagged = true;
	else if (!table[caseX][caseY].isDisplayed && table[caseX][caseY].isFlagged) table[caseX][caseY].isFlagged = false;
}


void grapicInit()
{
	backgroundColor(127, 127, 127);
}


void gridDrawer()
{
	for ( int vl = 20; vl < WINX - 20; vl += SIZE + 1 )
	{
		color(0, 0, 0);
		line(vl, 20, vl, WINY - 62);
	}
	
	for ( int hl = 20; hl < WINY - 60; hl += SIZE + 1 )
	{
		color(0, 0, 0);
		line(20, hl, WINX - 22, hl);
	}

	// Coord Tester
	if (DEBUG)
	{
		int x, y;
		mousePos(x, y);
		color(0);
		if ((x - 20) % (SIZE + 1) == 0 || (y - 20) % (SIZE + 1) == 0) color(255);
		if (x < 20 || x > WINX - 20 || y < 20 || y > WINY - 60)  color(255);
		print(WINX - 35, WINY - 24, x);
		print(WINX - 35, WINY - 40, y);
	}
}


void hud(mCase table[DIMX][DIMY])
{
	int remaining = mRemaining(table);
	double elapsedTime = double(clock() - startTime) / CLOCKS_PER_SEC;

	color(255, 255, 255);
	print(20, WINY - 32, "Time:");
	print(80, WINY - 32, int(elapsedTime));
	print(20, WINY - 55, "Remaining Flags:");
	print(180, WINY - 55, remaining);
}


void draw(mCase table[DIMX][DIMY])
{
	gridDrawer();
	if (isMousePressed(SDL_BUTTON_LEFT)) leftClick(table);
	if (isMousePressed(SDL_BUTTON_RIGHT)) rightClick(table);
	for (glX = 1; glX < DIMX - 1; glX++)
	{
		for (glY = 1; glY < DIMY - 1; glY++)
		{
			discover(table, glX, glY);
		}
	}
	hud(table);
	// recursive(table);
	if (mgameOver || mWon) finish();
}


int main(int, char**)
{
	srand(time(NULL));
	mCase table[DIMX][DIMY];
	init(table);
	mPrint(table);
	winInit(title, WINX, WINY);
	setKeyRepeatMode(true);
	grapicInit();
	while (!stop)
	{
		winClear();
		draw(table);
		stop = winDisplay();
	}
	winQuit();
	//system("pause");
	return 0;
}