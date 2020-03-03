#include "stdafx.h"
#include "Sudoku.h"


#include <iostream>
#include <fstream>
#include <vector>

void Sudoku::init() {
	for (int i = 0; i < 9; i++)for (int j = 0; j < 9; j++) {
		for (int k = 0; k < 9; k++) {
			possibilities[i][j][k] = true;
		}
	}
	for (int i = 0; i < 9; i++)for (int j = 0; j < 9; j++) {
		if (board[i][j] > 0 && board[i][j] < 10) {
			std::cout << "Applying input" << std::endl;
			applyNumber(i, j, board[i][j]);
			foundCount++;
		}
	}
}

void Sudoku::logMessage(std::string message,std::string title) {
	log.push_back(message);
	MessageBoxA(NULL, message.c_str(), title.c_str(), MB_OK);
}

int Sudoku::boxToRow(int box, int index) {
	return box / 3 * 3 + index / 3;
}
int Sudoku::boxToCol(int box, int index) {
	return box % 3 * 3 + index % 3;
}
int Sudoku::rcToBox(int row, int col) {
	return row / 3 * 3 + col / 3;
}
int Sudoku::rcToIndex(int row, int col) {
	return row % 3 * 3 + col % 3;
}

void Sudoku::printBoard() {
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			if (board[row][col] > 0)std::cout << board[row][col] << " ";
			else std::cout << "  ";
			if (col == 2 || col == 5)std::cout << "| ";
		}
		std::cout << std::endl;
		if (row == 2 || row == 5)std::cout << "------|-------|------" << std::endl;
	}

}

void Sudoku::applyNumber(int row, int col, int num) {
	board[row][col] = num;
	for (int i = 0; i < 9; i++) {
		possibilities[i][col][num - 1] = false;
		possibilities[row][i][num - 1] = false;
		possibilities[row / 3 * 3 + i / 3][col / 3 * 3 + i % 3][num - 1] = false;
		possibilities[row][col][i] = false;
	}
	printBoard();
	foundCount++;
}

void Sudoku::loadBoard(std::string filename) {
	std::ifstream file = std::ifstream(filename, 1, 64);
	char c;
	for (int row = 0; row < 9; row++)for (int col = 0; col < 9; col++) {
		file.get(c);
		while (c < '0' || c > '9') {
			file.get(c);
		}
		board[row][col] = c - '0';
	}
	file.close();
}

bool Sudoku::checkNakedSet() {
	bool numMask[9] = { 0,0,0,0,0,0,0,0,0 };
	for (int count = 1; count < 6; count++) {
		for (int i = 0; i < 9; i++)numMask[i] = i < count;
		bool countDone = false;
		while (!countDone) {
			//For each row
			for (int row = 0; row < 9; row++) {
				int matchCount = 0;
				bool matchMask[9] = { 1,1,1,1,1,1,1,1,1 };
				bool findMask[9] = { 0,0,0,0,0,0,0,0,0 };//
				bool eliminatable = false;
				for (int col = 0; col < 9; col++) {
					bool found = false;
					for (int n = 0; n < 9; n++) {
						if (!numMask[n] && possibilities[row][col][n]) {
							matchMask[col] = false;
						}
						if (numMask[n] && possibilities[row][col][n]) {
							found = true;
							findMask[col] = true;//
						}
					}
					if (!found)matchMask[col] = false;
					if (matchMask[col] && found)matchCount++;
				}
				for (int col = 0; col < 9; col++) {
					if (findMask[col] && !matchMask[col]) eliminatable = true;
				}
				if (matchCount != count || !eliminatable)continue;
				// Found Naked Set
				std::string mess = "Found naked set in row ";
				mess += ('0' + row);
				mess += std::string(" with numbers ");
				bool comma = false;
				for (int i = 0; i < 9; i++) {
					if (numMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i + 1;
						comma = true;
					}
				}
				mess = mess + " in columns ";
				comma = false;
				for (int i = 0; i < 9; i++) {
					if (matchMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i;
						comma = true;
					}
				}
				logMessage(mess, "Found Naked Set");
				//Apply reductions
				if (count == 1) {
					//Single
					int col = 0;
					for (int i = 0; i < 9; i++) {
						if (matchMask[i]) {
							col = i;
						}
					}
					int num = 0;
					for (int i = 0; i < 9; i++) {
						if (numMask[i]) {
							num = i + 1;
						}
					}
					applyNumber(row, col, num);
				}
				else {
					//Multiple
					for (int col = 0; col < 9; col++) {
						for (int n = 0; n < 9; n++) {
							if (!matchMask[col] && numMask[n] && possibilities[row][col][n]) {
								possibilities[row][col][n] = false;
							}
						}
					}
				}
				return true;
			}
			//For each col
			for (int col = 0; col < 9; col++) {
				int matchCount = 0;
				bool matchMask[9] = { 1,1,1,1,1,1,1,1,1 };
				bool findMask[9] = { 0,0,0,0,0,0,0,0,0 };//
				bool eliminatable = false;
				for (int row = 0; row < 9; row++) {
					bool found = false;
					for (int n = 0; n < 9; n++) {
						if (!numMask[n] && possibilities[row][col][n]) {
							matchMask[row] = false;
						}
						if (numMask[n] && possibilities[row][col][n]) {
							found = true;
							findMask[row] = true;//
						}
					}
					if (!found)matchMask[row] = false;
					if (matchMask[row] && found)matchCount++;
				}
				for (int row = 0; row < 9; row++) {
					if (findMask[row] && !matchMask[row]) eliminatable = true;
				}
				if (matchCount != count || !eliminatable)continue;
				// Found Naked Set
				std::string mess = "Found naked set in col ";
				mess += ('0' + col);
				mess += std::string(" with numbers ");
				bool comma = false;
				for (int i = 0; i < 9; i++) {
					if (numMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i + 1;
						comma = true;
					}
				}
				mess = mess + " in rows ";
				comma = false;
				for (int i = 0; i < 9; i++) {
					if (matchMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i;
						comma = true;
					}
				}
				logMessage(mess, "Found Naked Set");
				//Apply reductions
				if (count == 1) {
					//Single
					int row = 0;
					for (int i = 0; i < 9; i++) {
						if (matchMask[i]) {
							row = i;
						}
					}
					int num = 0;
					for (int i = 0; i < 9; i++) {
						if (numMask[i]) {
							num = i + 1;
						}
					}
					applyNumber(row, col, num);
				}
				else {
					//Multiple
					for (int row = 0; row < 9; row++) {
						for (int n = 0; n < 9; n++) {
							if (!matchMask[row] && numMask[n] && possibilities[row][col][n]) {
								possibilities[row][col][n] = false;
							}
						}
					}
				}
				return true;
			}
			//For each box
			for (int box = 0; box < 9; box++) {
				int matchCount = 0;
				bool matchMask[9] = { 1,1,1,1,1,1,1,1,1 };
				bool findMask[9] = { 0,0,0,0,0,0,0,0,0 };//
				bool eliminatable = false;
				for (int index = 0; index < 9; index++) {
					bool found = false;
					for (int n = 0; n < 9; n++) {
						if (!numMask[n] && possibilities[boxToRow(box,index)][boxToCol(box, index)][n]) {
							matchMask[index] = false;
						}
						if (numMask[n] && possibilities[boxToRow(box, index)][boxToCol(box, index)][n]) {
							found = true;
							findMask[index] = true;
						}
					}
					if (!found)matchMask[index] = false;
					if (matchMask[index] && found)matchCount++;
				}
				for (int index = 0; index < 9; index++) {
					if (findMask[index] && !matchMask[index]) eliminatable = true;
				}
				if (matchCount != count || !eliminatable)continue;
				// Found Naked Set
				std::string mess = "Found naked set in box ";
				mess += ('0' + box);
				mess += std::string(" with numbers ");
				bool comma = false;
				for (int i = 0; i < 9; i++) {
					if (numMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i + 1;
						comma = true;
					}
				}
				mess = mess + " in indices ";
				comma = false;
				for (int i = 0; i < 9; i++) {
					if (matchMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i;
						comma = true;
					}
				}
				logMessage(mess, "Found Naked Set");
				//Apply reductions
				if (count == 1) {
					//Single
					int index = 0;
					for (int i = 0; i < 9; i++) {
						if (matchMask[i]) {
							index = i;
						}
					}
					int num = 0;
					for (int i = 0; i < 9; i++) {
						if (numMask[i]) {
							num = i+1;
						}
					}
					applyNumber(boxToRow(box, index), boxToCol(box, index), num);
				}
				else {
					//Multiple
					int row=-1;
					int col=-1;
					for (int index = 0; index < 9; index++) {
						for (int n = 0; n < 9; n++) {
							if (!matchMask[index] && numMask[n] && possibilities[boxToRow(box, index)][boxToCol(box, index)][n]) {
								possibilities[boxToRow(box, index)][boxToCol(box, index)][n] = false;
							}
						}
					}
				}
				return true;
			}
			//Check Count Done
			countDone = true;
			for (int i = 0; i < count; i++) {
				if (!numMask[8-i])countDone = false;
			}
			//Increment numMask
			if (!countDone) {
				if (numMask[8]) {
					int count = 1;
					for (int i = 7; i >= 0; i--) {
						if (numMask[i] && !numMask[i + 1]) {
							numMask[i + 1] = true;
							numMask[i] = false;
							for (int j = i + 2; j < 9; j++) {
								if (count > 0) {
									numMask[j] = true;
									count--;
								}
								else numMask[j] = false;
							}
							break;
						}
						else if (numMask[i]) {
							count++;
						}
					}
				}
				else {
					for (int i = 7; i >= 0; i--) {
						if (numMask[i] && !numMask[i + 1]) {
							numMask[i + 1] = true;
							numMask[i] = false;
							break;
						}
					}
				}
			}
		}
	}
	return false;
}

bool Sudoku::checkHiddenSet() {
	bool numMask[9] = { 0,0,0,0,0,0,0,0,0 };
	for (int count = 1; count < 6; count++) {
		for (int i = 0; i < 9; i++)numMask[i] = i < count;
		bool countDone = false;
		while (!countDone) {
			//For each row
			for (int row = 0; row < 9; row++) {
				int matchCount = 0;
				bool matchMask[9] = { 0,0,0,0,0,0,0,0,0 };
				bool foundMask[9] = { 0,0,0,0,0,0,0,0,0 };
				bool allFound = true;
				for (int col = 0; col < 9; col++) {
					for (int n = 0; n < 9; n++) {
						if (numMask[n] && possibilities[row][col][n]) {
							matchMask[col] = true;
							foundMask[n] = true;
							matchCount++;
						}
					}
				}
				for (int i = 0; i < 9; i++) {
					if (numMask[i] && !foundMask[i])allFound = false;
				}
				if (matchCount != count || !allFound)continue;
				// Found Naked Set
				std::string mess = "Found hidden set in row ";
				mess += ('0' + row);
				mess += std::string("\nwith numbers ");
				bool comma = false;
				for (int i = 0; i < 9; i++) {
					if (numMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i + 1;
						comma = true;
					}
				}
				mess = mess + "\nin columns ";
				comma = false;
				for (int i = 0; i < 9; i++) {
					if (matchMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i;
						comma = true;
					}
				}
				MessageBoxA(NULL, mess.c_str(), "Found Hidden Set", MB_OK);
				//Apply reductions
				if (count == 1) {
					//Single
					int col = 0;
					for (int i = 0; i < 9; i++) {
						if (matchMask[i]) {
							col = i;
						}
					}
					int num = 0;
					for (int i = 0; i < 9; i++) {
						if (numMask[i]) {
							num = i + 1;
						}
					}
					applyNumber(row, col, num);
				}
				else {
					//Multiple
					for (int col = 0; col < 9; col++) {
						for (int n = 0; n < 9; n++) {
							if (matchMask[col] && !numMask[n] && possibilities[row][col][n]) {
								possibilities[row][col][n] = false;
							}
						}
					}
				}
				return true;
			}
			//For each col
			for (int col = 0; col < 9; col++) {
				int matchCount = 0;
				bool matchMask[9] = { 0,0,0,0,0,0,0,0,0 };
				bool foundMask[9] = { 0,0,0,0,0,0,0,0,0 };
				bool allFound = true;
				for (int row = 0; row < 9; row++) {
					for (int n = 0; n < 9; n++) {
						if (numMask[n] && possibilities[row][col][n]) {
							matchMask[row] = true;
							foundMask[n] = false;
							matchCount++;
						}
					}
				}
				for (int i = 0; i < 9; i++) {
					if (numMask[i] && !foundMask[i])allFound = false;
				}
				if (matchCount != count || !allFound)continue;
				// Found Naked Set
				std::string mess = "Found hidden set in column ";
				mess += ('0' + col);
				mess += std::string("\nwith numbers ");
				bool comma = false;
				for (int i = 0; i < 9; i++) {
					if (numMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i + 1;
						comma = true;
					}
				}
				mess = mess + "\nin rows ";
				comma = false;
				for (int i = 0; i < 9; i++) {
					if (matchMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i;
						comma = true;
					}
				}
				MessageBoxA(NULL, mess.c_str(), "Found Hidden Set", MB_OK);
				//Apply reductions
				if (count == 1) {
					//Single
					int row = 0;
					for (int i = 0; i < 9; i++) {
						if (matchMask[i]) {
							row = i;
						}
					}
					int num = 0;
					for (int i = 0; i < 9; i++) {
						if (numMask[i]) {
							num = i + 1;
						}
					}
					applyNumber(row, col, num);
				}
				else {
					//Multiple
					for (int row = 0; row < 9; row++) {
						for (int n = 0; n < 9; n++) {
							if (matchMask[row] && !numMask[n] && possibilities[row][col][n]) {
								possibilities[row][col][n] = false;
							}
						}
					}
				}
				return true;
			}
			//For each box
			for (int box = 0; box < 9; box++) {
				int matchCount = 0;
				bool matchMask[9] = { 0,0,0,0,0,0,0,0,0 };
				bool foundMask[9] = { 0,0,0,0,0,0,0,0,0 };
				bool allFound = true;
				for (int index = 0; index < 9; index++) {
					for (int n = 0; n < 9; n++) {
						if (numMask[n] && possibilities[boxToRow(box,index)][boxToCol(box,index)][n]) {
							matchMask[index] = true;
							foundMask[n] = true;
							matchCount++;
						}
					}
				}
				for (int i = 0; i < 9; i++) {
					if (numMask[i] && !foundMask[i])allFound = false;
				}
				if (matchCount != count || !allFound)continue;
				// Found Naked Set
				std::string mess = "Found hidden set in box ";
				mess += ('0' + box);
				mess += std::string("\nwith numbers ");
				bool comma = false;
				for (int i = 0; i < 9; i++) {
					if (numMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i + 1;
						comma = true;
					}
				}
				mess = mess + "\nin indices ";
				comma = false;
				for (int i = 0; i < 9; i++) {
					if (matchMask[i]) {
						if (comma)mess += ",";
						mess += '0' + i;
						comma = true;
					}
				}
				MessageBoxA(NULL, mess.c_str(), "Found Hidden Set", MB_OK);
				//Apply reductions
				if (count == 1) {
					//Single
					int index = 0;
					for (int i = 0; i < 9; i++) {
						if (matchMask[i]) {
							index = i;
						}
					}
					int num = 0;
					for (int i = 0; i < 9; i++) {
						if (numMask[i]) {
							num = i + 1;
						}
					}
					applyNumber(boxToRow(box,index), boxToCol(box, index), num);
				}
				else {
					//Multiple
					for (int index = 0; index < 9; index++) {
						for (int n = 0; n < 9; n++) {
							if (matchMask[index] && !numMask[n] && possibilities[boxToRow(box, index)][boxToCol(box, index)][n]) {
								possibilities[boxToRow(box, index)][boxToCol(box, index)][n] = false;
							}
						}
					}
				}
				return true;
			}
			//Check Count Done
			countDone = true;
			for (int i = 0; i < count; i++) {
				if (!numMask[8 - i])countDone = false;
			}
			//Increment numMask
			if (!countDone) {
				if (numMask[8]) {
					int count = 1;
					for (int i = 7; i >= 0; i--) {
						if (numMask[i] && !numMask[i + 1]) {
							numMask[i + 1] = true;
							numMask[i] = false;
							for (int j = i + 2; j < 9; j++) {
								if (count > 0) {
									numMask[j] = true;
									count--;
								}
								else numMask[j] = false;
							}
							break;
						}
						else if (numMask[i]) {
							count++;
						}
					}
				}
				else {
					for (int i = 7; i >= 0; i--) {
						if (numMask[i] && !numMask[i + 1]) {
							numMask[i + 1] = true;
							numMask[i] = false;
							break;
						}
					}
				}
			}
		}
	}
	return false;
}

bool Sudoku::checkPointingSet() {
	//row
	for (int row = 0; row < 9; row++) {
		for (int n = 0; n < 9; n++) {
			int failure = false;
			int box = -1;
			for (int col = 0; col < 9; col++) {
				if (possibilities[row][col][n]) {
					if (box == -1 || box == rcToBox(row, col))box = rcToBox(row, col);
					else failure = true;
				}
				if (failure)break;
			}
			if (box==-1 || failure)continue;
			//Found row with all n in one box
			bool messSent = false;
			for (int index = 0; index < 9; index++) {
				if (boxToRow(box,index)!=row && possibilities[boxToRow(box, index)][boxToCol(box, index)][n]) {
					if (!messSent) {
						std::string mess = std::string("Found Pointing Set in row ") + std::string(1, '0' + row) + " for " + std::string(1, '0' + n+1) + " in box " + std::string(1, '0' + box);
						MessageBoxA(NULL, mess.c_str(), "Found Pointing Set", MB_OK);
						messSent = true;
					}
					possibilities[boxToRow(box, index)][boxToCol(box, index)][n] = false;
				}
			}
			if (messSent)return true;
		}
	}
	//col
	for (int col = 0; col < 9; col++) {
		for (int n = 0; n < 9; n++) {
			int failure = false;
			int box = -1;
			for (int row = 0; row < 9; row++) {
				if (possibilities[row][col][n]) {
					if (box == -1 || box == rcToBox(row, col))box = rcToBox(row, col);
					else failure = true;
				}
				if (failure)break;
			}
			if (box == -1 || failure)continue;
			//Found col with all n in one box
			bool messSent = false;
			for (int index = 0; index < 9; index++) {
				if (boxToCol(box, index) != col && possibilities[boxToRow(box, index)][boxToCol(box, index)][n]) {
					if (!messSent) {
						std::string mess = std::string("Found Pointing Set in col ") + std::string(1, '0' + col) + " for " + std::string(1, '0' + n + 1) + " in box " + std::string(1, '0' + box);
						MessageBoxA(NULL, mess.c_str(), "Found Pointing Set", MB_OK);
						messSent = true;
					}
					possibilities[boxToRow(box, index)][boxToCol(box, index)][n] = false;
				}
			}
			if (messSent)return true;
		}
	}
	//box
	for (int box = 0; box < 9; box++) {
		for (int n = 0; n < 9; n++) {
			int row = -1, col = -1;
			for (int index = 0; index < 9; index++) {
				if (possibilities[boxToRow(box, index)][boxToCol(box, index)][n]) {
					if (row == -1 || row == boxToRow(box, index))row = boxToRow(box, index);
					else row = -2;
					if (col == -1 || col == boxToCol(box, index))col = boxToCol(box, index);
					else col = -2;
				}
			}
			if (row >= 0) {
				//Success in row
				bool messSent = false;
				for (int scanCol = 0; scanCol < 9; scanCol++) {
					if (possibilities[row][scanCol][n] && rcToBox(row, scanCol) != box) {
						if (!messSent) {
							std::string mess = std::string("Found Pointing Set in box ") + std::string(1, '0' + box) + " for " + std::string(1, '0' + n + 1) + " in row " + std::string(1, '0' + row);
							MessageBoxA(NULL, mess.c_str(), "Found Pointing Set", MB_OK);
							messSent = true;
						}
						possibilities[row][scanCol][n] = false;
					}
				}
				if (messSent)return true;
			}
			if (col >= 0) {
				//Success in col
				bool messSent = false;
				for (int scanRow = 0; scanRow < 9; scanRow++) {
					if (possibilities[scanRow][col][n] && rcToBox(scanRow, col) != box) {
						if (!messSent) {
							std::string mess = std::string("Found Pointing Set in box ") + std::string(1, '0' + box) + " for " + std::string(1, '0' + n + 1) + " in col " + std::string(1, '0' + col);
							MessageBoxA(NULL, mess.c_str(), "Found Pointing Set", MB_OK);
							messSent = true;
						}
						possibilities[row][scanRow][n] = false;
					}
				}
				if (messSent)return true;
			}
		}
	}
	return false;
}

bool Sudoku::checkXWing() {
	//row
	for (int row = 0; row < 8; row++) {
		for (int n = 0; n < 9; n++) {
			int mask = 0;
			int count = 0;
			for (int col = 0; col < 9; col++) {
				if (possibilities[row][col][n]) {
					mask = mask * 2 + 1;
					count++;
				}
				else mask = mask * 2;
			}
			if (count > 1 && count < 9) {
				int rows[8] = { 0,0,0,0,0,0,0,0 };
				rows[0] = row;
				int rowCount = 1;
				for (int scanRow = row + 1; scanRow < 9; scanRow++) {
					int scanMask = 0;
					for (int scanCol = 0; scanCol < 9; scanCol++) {
						if (possibilities[scanRow][scanCol][n]) {
							scanMask = scanMask * 2 + 1;
						}
						else scanMask = scanMask * 2;
					}
					if (scanMask == mask) {
						rows[rowCount] = scanRow;
						rowCount++;
					}
				}
				if (count == rowCount) {
					//Found X-Wing
					bool messSent = false;
					for (int scanRow = 0; scanRow < 9; scanRow++) {
						bool skip = false;
						for (int r = 0; r < rowCount; r++) {
							if (scanRow == rows[r]) {
								skip = true;
								break;
							}
						}
						if (skip)continue;
						int scanMask = 1;
						for (int scanCol = 8; scanCol >= 0; scanCol--) {
							if (mask&scanMask && possibilities[scanRow][scanCol][n]) {
								if (!messSent) {
									std::string mess = std::string("Found X-Wing for ") + std::string(1, '0' + n + 1) + " in rows ";
									for (int r = 0; r < rowCount; r++) {
										if (r > 0)mess = mess + ", ";
										mess = mess + (char)('0' + rows[r]);
									}
									MessageBoxA(NULL, mess.c_str(), "Found X-Wing", MB_OK);
									messSent = true;
								}
								possibilities[scanRow][scanCol][n] = false;
							}
							scanMask *= 2;
						}
					}
					if (messSent)return true;
				}
			}
		}
	}
	//col
	for (int col = 0; col < 8; col++) {
		for (int n = 0; n < 9; n++) {
			int mask = 0;
			int count = 0;
			for (int row = 0; row < 9; row++) {
				if (possibilities[row][col][n]) {
					mask = mask * 2 + 1;
					count++;
				}
				else mask = mask * 2;
			}
			if (count > 1 && count < 9) {
				int cols[8] = { 0,0,0,0,0,0,0,0 };
				cols[0] = col;
				int colCount = 1;
				for (int scanCol = col + 1; scanCol < 9; scanCol++) {
					int scanMask = 0;
					for (int scanRow = 0; scanRow < 9; scanRow++) {
						if (possibilities[scanRow][scanCol][n]) {
							scanMask = scanMask * 2 + 1;
						}
						else scanMask = scanMask * 2;
					}
					if (scanMask == mask) {
						cols[colCount] = scanCol;
						colCount++;
					}
				}
				if (count == colCount) {
					//Found X-Wing
					bool messSent = false;
					for (int scanCol = 0; scanCol < 9; scanCol++) {
						bool skip = false;
						for (int c = 0; c < colCount; c++) {
							if (scanCol == cols[c]) {
								skip = true;
								break;
							}
						}
						if (skip)continue;
						int scanMask = 1;
						for (int scanRow = 8; scanRow >= 0; scanRow--) {
							if (mask&scanMask && possibilities[scanRow][scanCol][n]) {
								if (!messSent) {
									std::string mess = std::string("Found X-Wing for ") + std::string(1, '0' + n + 1) + " in cols ";
									for (int c = 0; c < colCount; c++) {
										if (c > 0)mess = mess + ", ";
										mess = mess + (char)('0' + cols[c]);
									}
									MessageBoxA(NULL, mess.c_str(), "Found X-Wing", MB_OK);
									messSent = true;
								}
								possibilities[scanRow][scanCol][n] = false;
							}
							scanMask *= 2;
						}
					}
					if (messSent)return true;
				}
			}
		}
	}
	return false;
}

bool Sudoku::checkXYWing() {
	struct Wing {
		int row, col, box;
		int n1, n2;
	};
	std::vector<Wing> wings;
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			int count = 0;
			Wing w = { row, col, rcToBox(row,col), 0, 0 };
			for (int n = 0; n < 9; n++) {
				if (possibilities[row][col][n]) {
					if (count == 0)w.n1 = n;
					else if (count == 1)w.n2 = n;
					count++;
				}
			}
			if (count == 2)wings.push_back(w);
		}
	}
	for (int i = 0; i < wings.size(); i++) {
		for (int j = 0; j < wings.size(); j++) {
			if (i == j)continue;
			for (int k = 0; k < wings.size(); k++) {
				if (i == k || j == k)continue;
				int wi = (1 << wings[i].n1) | (1 << wings[i].n2);
				int wj = (1 << wings[j].n1) | (1 << wings[j].n2);
				int wk = (1 << wings[k].n1) | (1 << wings[k].n2);
				if ((wings[i].row == wings[j].row || wings[i].col == wings[j].col || wings[i].box == wings[j].box) &&
					(wings[i].row == wings[k].row || wings[i].col == wings[k].col || wings[i].box == wings[k].box) &&
					(wings[k].row != wings[j].row && wings[k].col != wings[j].col && wings[k].box != wings[j].box)) {
					if ((wi & wj) && (wi & wk) && (wj & wk) && !(wi & (wj & wk))) {
						//Found XY candidate
						int n = ((wj & wk) == (1 << wings[j].n1)) ? wings[j].n1 : wings[j].n2;
						bool messSent = false;
						for (int row = 0; row < 9; row++) {
							for (int col = 0; col < 9; col++) {
								if (possibilities[row][col][n] &&
									(row == wings[j].row || col == wings[j].col || rcToBox(row, col) == wings[j].box) &&
									(row == wings[k].row || col == wings[k].col || rcToBox(row, col) == wings[k].box)) {
									//Found reduction
									if (!messSent) {
										std::string mess = std::string("Found XY Wing for (");
										mess = mess + std::to_string(wings[j].row)
											+ std::string(",") + std::to_string(wings[j].col)
											+ std::string(")(") + std::to_string(wings[i].row)
											+ std::string(",") + std::to_string(wings[i].col)
											+ std::string(")(") + std::to_string(wings[k].row)
											+ std::string(",") + std::to_string(wings[k].col)
											+ std::string(") for n = ") + std::to_string(n+1);
										//mess.replace("$", std::string((char)('0' + wings[j].row),1),1);
										logMessage(mess.c_str(), "XY Wing");
										messSent = true;
									}
									possibilities[row][col][n] = false;
								}
							}
						}
						if (messSent)return true;
					}
				}
			}
		}
	}
	return false;
}

void Sudoku::chain(int groups[9][9], int row, int col, int n) {
	int sr = -1, sc = -1, si = -1;
	int box = rcToBox(row, col);
	int nextLinkGroup = groups[row][col] % 2 ? groups[row][col] + 1 : groups[row][col] - 1;
	for (int i = 0; i < 9; i++) {
		if (i != row && possibilities[i][col][n]) {
			if (sr == -1)sr = i;
			else sr = -2;
		}
		if (i != col && possibilities[row][i][n]) {
			if (sc == -1)sc = i;
			else sc = -2;
		}
		if (i != rcToIndex(row,col) && possibilities[boxToRow(box,i)][boxToCol(box, i)][n]) {
			if (si == -1)si = i;
			else si = -2;
		}
	}
	if (sr >= 0 && groups[sr][col] == 0) {
		groups[sr][col] = nextLinkGroup;
		chain(groups, sr, col, n);
	}
	if (sc >= 0 && groups[row][sc] == 0) {
		groups[row][sc] = nextLinkGroup;
		chain(groups, row, sc, n);
	}
	if (si >= 0 && groups[boxToRow(box, si)][boxToCol(box, si)] == 0) {
		groups[boxToRow(box, si)][boxToCol(box, si)] = nextLinkGroup;
		chain(groups, boxToRow(box, si), boxToCol(box, si), n);
	}
}
bool Sudoku::checkChain() {
	static bool display = false;
	for (int n = 0; n < 9; n++) {
		//Create group map for n
		for (int row = 0; row < 9; row++)for (int col = 0; col < 9; col++)groups[row][col] = 0;
		int group = 1;
		for (int row = 0; row < 9; row++) {
			for (int col = 0; col < 9; col++) {
				if (possibilities[row][col][n] && groups[row][col] == 0) {
					groups[row][col] = group;
					chain(groups, row, col, n);
					group += 2;
				}
			}
		}
		int maxGroup = group;
		// Check for two of the same group in the same row/col/box
		for (group = 1; group < maxGroup; group++) {
			bool rows[9] = { 0,0,0,0,0,0,0,0,0 };
			bool cols[9] = { 0,0,0,0,0,0,0,0,0 };
			bool boxes[9] = { 0,0,0,0,0,0,0,0,0 };
			for (int row = 0; row < 9; row++) {
				for (int col = 0; col < 9; col++) {
					if (groups[row][col] == group) {
						if (rows[row] || cols[col] || boxes[rcToBox(row, col)]) {
							display = !display;
							if (display) return true;
							std::string mess = "Chaining found collisions for ";
							mess += (char)('1' + n);
							if (rows[row])mess = (mess + " in row ") + (char)('0' + row);
							else if (cols[col])mess = (mess + " in col ") + (char)('0' + col);
							else if (boxes[rcToBox(row,col)])mess = (mess + " in box ") + (char)('0' + rcToBox(row,col));
							MessageBoxA(NULL, mess.c_str(), "Found Chain", MB_OK);
							int otherGroup = group % 2 ? group + 1 : group - 1;
							for (int r = 0; r < 9; r++) {
								for (int c = 0; c < 9; c++) {
									if (groups[r][c] == otherGroup) {
										applyNumber(r, c, n + 1);
									}
								}
							}
							return true;
						}
						else rows[row] = true;
					}
				}
			}
		}
		// Check for one group collides with both of another group
		std::vector<int> mask;
		for (group = 1; group < maxGroup; group++) mask.push_back(0);
		for (int row = 0; row < 9; row++)for (int col = 0; col < 9; col++) {
			int box = rcToBox(row, col);
			if (groups[row][col]) {
				mask[groups[row][col] - 1] |= (1 << row) | (1 << (col + 9)) | (1 << (box + 18));
			}
		}
		for (group = 1; group < maxGroup; group++) {
			for (int otherGroup = 1; otherGroup < maxGroup; otherGroup+=2) {
				if (otherGroup != group && otherGroup + 1 != group ) {
					if (mask[group - 1] & mask[otherGroup - 1] && mask[group - 1] & mask[otherGroup]) {
						int g1 = group % 2 ? group + 1 : group - 1;
						bool messSent = false;
						for (int r = 0; r < 9; r++) {
							for (int c = 0; c < 9; c++) {
								if (groups[r][c] == g1) {
									display = !display;
									if (display) return true;
									if (!messSent) {
										std::string mess = "Chaining found ";
										mess += (char)('1' + n);
										mess += " failed chaining test 2";
										MessageBoxA(NULL, mess.c_str(), "Found Chain", MB_OK);
										messSent = true;
									}
									applyNumber(r, c, n + 1);
								}
							}
						}
						if(messSent)return true;
					}
				}
			}
		}
		continue; //BYPASS TEST 3
		// Check for two group pinch
		for (group = 1; group < maxGroup; group++) {
			for (int otherGroup = group+1; otherGroup < maxGroup; otherGroup ++) {
				if (group % 2 && group + 1 == otherGroup)continue;
				if (mask[group - 1] & mask[otherGroup - 1]) {
					bool messSent = false;
					int g1 = group % 2 ? group + 1 : group - 1;
					int g2 = otherGroup % 2 ? otherGroup + 1 : otherGroup - 1;
					for (int row = 0; row < 9; row++) {
						for (int col = 0; col < 9; col++) {
							if (possibilities[row][col][n]) {
								int box = rcToBox(row, col);
								bool collidesG1 = false;
								bool collidesG2 = false;
								for (int i = 0; i < 9; i++) {
									if (groups[i][col] == g1)collidesG1 = true;
									if (groups[row][i] == g1)collidesG1 = true;
									if (groups[boxToRow(box, i)][boxToCol(box, i)] == g1)collidesG1 = true;
									if (groups[i][col] == g2)collidesG2 = true;
									if (groups[row][i] == g2)collidesG2 = true;
									if (groups[boxToRow(box, i)][boxToCol(box, i)] == g2)collidesG2 = true;
								}
								if (collidesG1 && collidesG2) {
									display = !display;
									if (display) return true;
									if (!messSent) {
										std::string mess = "Chaining found ";
										mess += (char)('1' + n);
										mess += " failed chaining test 3";
										MessageBoxA(NULL, mess.c_str(), "Found Chain", MB_OK);
										messSent = true;
									}
									possibilities[row][col][n] = false;
								}
							}
						}
					}
					if (messSent)return true;
				}
			}
		}
	}
	for (int row = 0; row < 9; row++)for (int col = 0; col < 9; col++)groups[row][col] = 0;
	return false;
}

bool Sudoku::solve()
{
	while (false) {
		int oldFoundCount = foundCount;

		//Check for cells with only one possibility
		for (int row = 0; row < 9; row++) {
			for (int col = 0; col < 9; col++) {
				int possibleNumber = 0;
				for (int n = 0; n < 9; n++) {
					if (possibilities[row][col][n]) {
						if (possibleNumber == 0) possibleNumber = n + 1;
						else possibleNumber = -1;
					}
				}
				if (possibleNumber > 0) {
					std::cout << "Found number " << possibleNumber << " is the only possible number for " << row << "," << col << std::endl;
					applyNumber(row, col, possibleNumber);
					foundCount++;
				}
			}
		}
		//Check for rows where a number can only fit in one place
		for (int row = 0; row < 9; row++) {
			int numCheck[9] = { 0,0,0,0,0,0,0,0,0 };
			for (int col = 0; col < 9; col++) {
				for (int n = 0; n < 9; n++) {
					if (possibilities[row][col][n]) {
						if (numCheck[n] == 0)numCheck[n] = col + 1;
						else numCheck[n] = -1;
					}
				}
			}
			for (int n = 0; n < 9; n++) {
				if (numCheck[n] > 0) {
					std::cout << "Found number " << n + 1 << " only possible in column " << numCheck[n] - 1 << " for row " << row << std::endl;
					applyNumber(row, numCheck[n] - 1, n + 1);
					foundCount++;
				}
			}
		}
		//Check for cols where a number can only fit in one place
		for (int col = 0; col < 9; col++) {
			int numCheck[9] = { 0,0,0,0,0,0,0,0,0 };
			for (int row = 0; row < 9; row++) {
				for (int n = 0; n < 9; n++) {
					if (possibilities[row][col][n]) {
						if (numCheck[n] == 0)numCheck[n] = row + 1;
						else numCheck[n] = -1;
					}
				}
			}
			for (int n = 0; n < 9; n++) {
				if (numCheck[n] > 0) {
					std::cout << "Found number " << n + 1 << " only possible in row " << numCheck[n] - 1 << " for column " << col << std::endl;
					applyNumber(numCheck[n] - 1, col, n + 1);
					foundCount++;
				}
			}
		}
		//Check for squares where a number can only fit in one place
		for (int square = 0; square < 9; square++) {
			int numCheck[9] = { 0,0,0,0,0,0,0,0,0 };
			for (int index = 0; index < 9; index++) {
				int row = square / 3 * 3 + index / 3;
				int col = (square % 3) * 3 + index % 3;
				for (int n = 0; n < 9; n++) {
					if (possibilities[row][col][n]) {
						if (numCheck[n] == 0)numCheck[n] = index + 1;
						else numCheck[n] = -1;
					}
				}
			}
			for (int n = 0; n < 9; n++) {
				if (numCheck[n] > 0) {
					std::cout << "Found number " << n + 1 << " only possible in index " << numCheck[n] - 1 << " for square " << square << std::endl;
					int row = square / 3 * 3 + (numCheck[n] - 1) / 3;
					int col = (square % 3) * 3 + (numCheck[n] - 1) % 3;
					applyNumber(row, col, n + 1);
					foundCount++;
				}
			}
		}
		//Check for squares that limit rows or columns
		if (oldFoundCount == foundCount) for (int square = 0; square < 9; square++) {
			int rowLimits[9] = { 0,0,0,0,0,0,0,0,0 };
			int colLimits[9] = { 0,0,0,0,0,0,0,0,0 };
			for (int n = 0; n < 9; n++) {
				for (int index = 0; index < 9; index++) {
					int row = square / 3 * 3 + index / 3;
					int col = square % 3 * 3 + index % 3;
					if (possibilities[row][col][n]) {
						if (rowLimits[n] == 0 || rowLimits[n] == row + 1) rowLimits[n] = row + 1;
						else rowLimits[n] = -1;
						if (colLimits[n] == 0 || colLimits[n] == col + 1) colLimits[n] = col + 1;
						else colLimits[n] = -1;
					}
				}
			}
			for (int n = 0; n < 9; n++) {
				if (rowLimits[n] > 0) {
					for (int col = 0; col < 9; col++) {
						if (col / 3 != square % 3) {
							if (possibilities[rowLimits[n] - 1][col][n]) {
								std::cout << "Square " << square << " limiting " << n + 1 << " from row " << rowLimits[n] - 1 << std::endl;
								possibilities[rowLimits[n] - 1][col][n] = false;
								oldFoundCount = -1;
							}
						}
					}
				}
				if (colLimits[n] > 0) {
					for (int row = 0; row < 9; row++) {
						if (row / 3 != square / 3) {
							if (possibilities[row][colLimits[n] - 1][n]) {
								std::cout << "Square " << square << " limiting " << n + 1 << " from col " << colLimits[n] - 1 << std::endl;
								possibilities[row][colLimits[n] - 1][n] = false;
								oldFoundCount = -1;
							}
						}
					}
				}
			}
		}
		//Check for X-Wing Elimination
		for (int row = 0; row < 9; row++) {
			for (int n = 0; n < 9; n++) {
				bool failed = false;
				bool colMask[9] = { 0,0,0,0,0,0,0,0,0 };
				int colCount = 0;
				for (int col = 0; col < 9; col++) {
					if (possibilities[row][col][n]) {
						colMask[col] = true;
						colCount++;
					}
				}
				if (colCount != 2) continue;
				for (int row2 = row + 1; row2 < 9; row2++) {
					int colCount2 = 0;
					for (int col = 0; col < 9; col++) {
						if (possibilities[row2][col][n]) {
							if (!colMask[col]) {
								failed = true;
								break;
							}
							colCount2++;
						}
					}
					if (failed)continue;
					if (colCount2 != 2)continue;
					for (int col = 0; col < 9; col++) {
						if (colMask[col]) {
							for (int row3 = 0; row3 < 9; row3++) {
								if (row3 != row && row3 != row2 && possibilities[row3][col][n]) {
									std::cout << "Row X-Wing eliminates " << n + 1 << " from col " << col << " by rows " << row << " and " << row2 << std::endl;
									possibilities[row3][col][n] = false;
								}
							}
						}
					}
					break;
				}
			}
		}
		//
		for (int col = 0; col < 9; col++) {
			for (int n = 0; n < 9; n++) {
				bool failed = false;
				bool rowMask[9] = { 0,0,0,0,0,0,0,0,0 };
				int rowCount = 0;
				for (int row = 0; row < 9; row++) {
					if (possibilities[row][col][n]) {
						rowMask[row] = true;
						rowCount++;
					}
				}
				if (rowCount != 2) continue;
				for (int col2 = col + 1; col2 < 9; col2++) {
					int rowCount2 = 0;
					for (int row = 0; row < 9; row++) {
						if (possibilities[row][col2][n]) {
							if (!rowMask[row]) {
								failed = true;
								break;
							}
							rowCount2++;
						}
					}
					if (failed)continue;
					if (rowCount2 != 2)continue;
					for (int row = 0; row < 9; row++) {
						if (rowMask[row]) {
							for (int col3 = 0; col3 < 9; col3++) {
								if (col3 != col && col3 != col2 && possibilities[row][col3][n]) {
									std::cout << "Col X-Wing eliminates " << n + 1 << " from row " << row << " by cols " << col << " and " << col2 << std::endl;
									possibilities[row][col3][n] = false;
								}
							}
						}
					}
					break;
				}
			}
		}
		//Check for rows where pairs can eliminate possibilities
		if (oldFoundCount == foundCount) for (int row = 0; row < 9; row++) {
			for (int n = 1; n < 0b011111111; n++) {
				bool numMask[9] = { 0,0,0,0,0,0,0,0,0 };
				int colMask[9] = { 0,0,0,0,0,0,0,0,0 };
				int nShift = n;
				int bitCount = 0;
				int colCount = 0;
				bool skip = false;
				for (int i = 0; i < 9; i++) {
					if (nShift % 2 == 1) {
						for (int col = 0; col < 9; col++) {
							if (board[row][col] == i + 1) {
								skip = true;
							}
						}
						if (skip)break;
						bitCount++;
						numMask[i] = true;
					}
					nShift = nShift >> 1;
				}
				if (skip)continue;
				if (bitCount < 2)continue;
				for (int i = 0; i < 9; i++) {
					if (numMask[i]) {
						for (int col = 0; col < 9; col++) {
							colMask[col] += possibilities[row][col][i];
						}
					}
				}
				for (int col = 0; col < 9; col++) {
					if (colMask[col] == bitCount)colCount++;
					else if (colMask[col] > 0) {
						skip = true;
						break;
					}
				}
				if (skip)continue;
				if (colCount == bitCount) {
					for (int col = 0; col < 9; col++) {
						for (int i = 0; i < 9; i++) {
							if (possibilities[row][col][i] && !numMask[i] && colMask[col] > 0) {
								possibilities[row][col][i] = false;
								std::cout << "Row Pairing eliminates " << i + 1 << " from " << row << "," << col << std::endl;
								oldFoundCount = -1;
							}
						}
					}
				}
			}
		}
		//Check for cols where pairs can eliminate possibilities
		for (int col = 0; col < 9; col++) {
			for (int n = 1; n < 0b011111111; n++) {
				bool numMask[9] = { 0,0,0,0,0,0,0,0,0 };
				int rowMask[9] = { 0,0,0,0,0,0,0,0,0 };
				int nShift = n;
				int bitCount = 0;
				int rowCount = 0;
				bool skip = false;
				for (int i = 0; i < 9; i++) {
					if (nShift % 2 == 1) {
						for (int row = 0; row < 9; row++) {
							if (board[row][col] == i + 1) {
								skip = true;
							}
						}
						if (skip)break;
						bitCount++;
						numMask[i] = true;
					}
					nShift = nShift >> 1;
				}
				if (skip)continue;
				if (bitCount < 2)continue;
				for (int i = 0; i < 9; i++) {
					if (numMask[i]) {
						for (int row = 0; row < 9; row++) {
							rowMask[row] += possibilities[row][col][i];
						}
					}
				}
				for (int row = 0; row < 9; row++) {
					if (rowMask[row] == bitCount)rowCount++;
					else if (rowMask[row] > 0) {
						skip = true;
						break;
					}
				}
				if (skip)continue;
				if (rowCount == bitCount) {
					for (int row = 0; row < 9; row++) {
						for (int i = 0; i < 9; i++) {
							if (possibilities[row][col][i] && !numMask[i] && rowMask[row] > 0) {
								std::cout << "Column Pairing eliminates " << i + 1 << " from " << row << "," << col << std::endl;
								possibilities[row][col][i] = false;
								oldFoundCount = -1;
							}
						}
					}
				}
			}
		}
		//Check for squares where pairs can eliminate possibilities
		for (int square = 0; square < 9; square++) {
			for (int n = 1; n < 0b011111111; n++) {
				bool numMask[9] = { 0,0,0,0,0,0,0,0,0 };
				int indexMask[9] = { 0,0,0,0,0,0,0,0,0 };
				int nShift = n;
				int bitCount = 0;
				int indexCount = 0;
				bool skip = false;
				for (int i = 0; i < 9; i++) {
					if (nShift % 2 == 1) {
						for (int index = 0; index < 9; index++) {
							if (board[square / 3 * 3 + index / 3][square % 3 * 3 + index % 3] == i + 1) {
								skip = true;
							}
						}
						if (skip)break;
						bitCount++;
						numMask[i] = true;
					}
					nShift = nShift >> 1;
				}
				if (skip)continue;
				if (bitCount < 2)continue;
				for (int i = 0; i < 9; i++) {
					if (numMask[i]) {
						for (int index = 0; index < 9; index++) {
							indexMask[index] += possibilities[square / 3 * 3 + index / 3][square % 3 * 3 + index % 3][i];
						}
					}
				}
				for (int index = 0; index < 9; index++) {
					if (indexMask[index] == bitCount)indexCount++;
					else if (indexMask[index] > 0) {
						skip = true;
						break;
					}
				}

				if (skip)continue;
				if (indexCount == bitCount) {
					for (int index = 0; index < 9; index++) {
						for (int i = 0; i < 9; i++) {
							if (possibilities[square / 3 * 3 + index / 3][square % 3 * 3 + index % 3][i] && !numMask[i] && indexMask[index] > 0) {
								std::cout << "Square Pairing eliminates " << i + 1 << " from " << square / 3 * 3 + index / 3 << "," << square % 3 * 3 + index % 3 << std::endl;
								possibilities[square / 3 * 3 + index / 3][square % 3 * 3 + index % 3][i] = false;
								oldFoundCount = -1;
							}
						}
					}
				}
			}
		}
		//Hidden Tuples: Check for ### where n numbers are limited to n spots


		//Exit if no progress is made
		if (oldFoundCount == foundCount)break;
	}

	if (checkXYWing() || checkChain() || checkNakedSet() || checkHiddenSet() || checkPointingSet() || checkXWing()) {
		return false;
	}
	return true;
}

void Sudoku::draw(HDC hdc) {

	int boardx = 1; int boardy = 1;
	int boxw = 50; int boxh = 50;

	Rectangle(hdc, 0, 0, 10000, 10000);

	//Draw Background
	SelectObject(hdc, GetStockObject(DC_PEN));
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	for (int row = 0; row < 9; row++)for (int col = 0; col < 9; col++) {
		if (groups[row][col]) {

			int g = groups[row][col] - 1;
			int red = 0, blue = 0, green = 0;
			int amount = 255;
			while (g > 5) {
				g = g - 6;
				amount = amount * 3 / 4;
			}
			if (g == 0 || g == 1 || g == 5)red = amount;
			if (g == 1 || g == 2 || g == 3)blue = amount;
			if (g == 3 || g == 4 || g == 5)green = amount;
			SetDCPenColor(hdc, RGB(red, blue, green));
			SetDCBrushColor(hdc, RGB(red, blue, green));
			Rectangle(hdc,1+ boardx + col * boxw+col,1+ boardy + row * boxh+row,1+ boardx + col * boxw + boxw+col,1+ boardy + row * boxh + boxh+row);
			Rectangle(hdc, boxw * 9 + 50, 10 + groups[row][col] * 15, boxw * 9 + 50 + 10, 10 + groups[row][col] * 15 + 10);
		}
	}
	SetDCPenColor(hdc, RGB(0, 0, 0));
	//Draw Outer Border
	MoveToEx(hdc, boardx, boardy, NULL);
	LineTo(hdc, boardx + 9 * boxw + 9, boardy);
	LineTo(hdc, boardx + 9 * boxw + 9, boardy + 9 * boxh + 9);
	LineTo(hdc, boardx, boardy + 9 * boxh + 9);
	LineTo(hdc, boardx, boardy);
	//Draw Inner Borders
	for (int i = 1; i < 9; i++) {
		MoveToEx(hdc, boardx + i * boxw+i, boardy, NULL);
		LineTo(hdc, boardx + i * boxw+i, boardy + 9 * boxh + 9);
		MoveToEx(hdc, boardx, boardy + i * boxh+i, NULL);
		LineTo(hdc, boardx + 9 * boxw + 9, boardy + i * boxh+i);
		if (i == 3 || i == 6) {
			MoveToEx(hdc, boardx + i * boxw + i - 1, boardy, NULL);
			LineTo(hdc, boardx + i * boxw + i - 1, boardy + 9 * boxh + 9);
			MoveToEx(hdc, boardx, boardy + i * boxh + i - 1, NULL);
			LineTo(hdc, boardx + 9 * boxw + 9, boardy + i * boxh + i - 1);
			MoveToEx(hdc, boardx + i * boxw + i + 1, boardy, NULL);
			LineTo(hdc, boardx + i * boxw + i + 1, boardy + 9 * boxh + 9);
			MoveToEx(hdc, boardx, boardy + i * boxh + i + 1, NULL);
			LineTo(hdc, boardx + 9 * boxw + 9, boardy + i * boxh + i + 1);
		}
	}
	HFONT font_b = CreateFont(40, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 2, 0, TEXT("SYSTEM_FIXED_FONT"));
	HFONT font_p = CreateFont(10, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 2, 0, TEXT("SYSTEM_FIXED_FONT"));
	SetTextAlign(hdc, TA_BASELINE | TA_CENTER);
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			if (board[row][col] > 0) {
				SelectObject(hdc, font_b);
				SetTextColor(hdc, RGB(0, 0, 0));
				TextOutA(hdc,
					boardx + col * boxw + col + boxw / 2,
					boardy + row * boxh + row + boxh*3/4,
					std::string(1, '0' + board[row][col]).c_str(), 1);
			}
			else {
				for (int n = 0; n < 9; n++) {
					if (possibilities[row][col][n]) {
						SelectObject(hdc, font_p);
						SetTextColor(hdc, RGB(128, 128, 128));
						TextOutA(hdc,
							boardx + col * boxw + col + (boxw+3) * (n % 3 + 1) / 4,
							boardy + row * boxh + row + (boxh+3) * (n / 3 + 1) / 4,
							std::string(1, '0' + n+1).c_str(), 1);
					}
				}
			}
		}
	}

	SelectObject(hdc, font_p);
	SetTextColor(hdc, RGB(0, 0, 0));
	SetTextAlign(hdc, TA_BASELINE | TA_LEFT);
	int i = 0;
	for (std::string message : log) {
		TextOutA(hdc, boxw * 9 + 100, i * 9 + 10, message.c_str(), message.size());
		i++;
	}
	DeleteObject(font_b);
	DeleteObject(font_p);
}

Sudoku::Sudoku()
{
}

Sudoku::Sudoku(std::string filename)
{
	loadBoard(filename);
	init();
}

Sudoku::~Sudoku()
{
}
