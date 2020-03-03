#pragma once

#include <string>
#include <vector>

class Sudoku
{
private:
	HWND hWnd;
	int board[9][9];
	bool possibilities[9][9][9];
	int groups[9][9];
	int foundCount = 0;
	std::vector<std::string> log;
	void init();
	void logMessage(std::string message, std::string title);
	int boxToRow(int box, int index);
	int boxToCol(int box, int index);
	int rcToBox(int row, int col);
	int rcToIndex(int row, int col);
	void printBoard();
	void applyNumber(int row, int col, int num);
	void loadBoard(std::string filename);
	bool checkNakedSet();
	bool checkHiddenSet();
	bool checkPointingSet();
	bool checkXWing();
	bool checkXYWing();
	void chain(int groups[9][9], int row, int col, int n);
	bool checkChain();
public:
	bool solve();
	void draw(HDC hdc);
	Sudoku();
	Sudoku(std::string filename);
	~Sudoku();
};

