//
// Author:		James Callahan
// Class:		CS 438
// File:		HRSearch.h
// Assignment:	Homework 2
// Due Date:	6 February 2018
//

#pragma once
#include <list>
#include <set>
#include <fstream>
#include <time.h>

typedef int direction;
typedef int search;

const int rowSize = 5;
const int colSize = 4;
const int blank = 10;
const int numBlanks = 2;
const direction UP = 1;
const direction DOWN = 2;
const direction LEFT = 3;
const direction RIGHT = 4;
const search bfs = 8;
const search dfs = 9;

using namespace std;

struct node {
	int arr[rowSize][colSize];
	int depth;
	node *parent;
	long num;
};

class HRSearch
{
public:
	//HRSearch(int board[][colSize]);
	HRSearch(string filename);
	~HRSearch();
	void BFS();
	void DFS();

private:
	// Variables
	node *root, *current, *newN, *nN;
	int blankRow, blankCol;
	list<node*> frontier, extracted;
	list<int> oneByOne;
	list<int> oneByTwo;
	list<int> twoByOne;
	int twoByTwo;
	ifstream in;
	ofstream out;
	string inFilename, outFilename;
	time_t timer;

	int foundIndex;
	set<long> foundBoards;
	int moves;

	// functions
	bool moveBlankUp(node *n);
	bool moveBlankDown(node *n);
	bool moveBlankLeft(node *n);
	bool moveBlankRight(node *n);
	void findNextBlank(node *n);
	bool makeMove(direction dir, search s);

	bool targetFound(node n);
	void copyBoard(node *source, node *target);
	void printBoard(node n);
	void outputToFile(ofstream &outfile);
	string stringPuzzle(node n);
	void outputLists();

	long getNum(node *n);

	void startTimer();
	void endTimer();

	bool openFile(ifstream &infile, string filename);
};

