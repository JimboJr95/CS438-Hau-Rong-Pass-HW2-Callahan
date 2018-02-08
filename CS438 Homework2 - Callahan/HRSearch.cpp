//
// Author:		James Callahan
// Class:		CS 438
// File:		HRSearch.cpp
// Assignment:	Homework 2
// Due Date:	6 February 2018
//


#include "stdafx.h"
#include "HRSearch.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>

using namespace std;

// Constructor
HRSearch::HRSearch(string filename)
{
	bool check = false, validOpen = false;
	int row = 0, col = 0, nums[10], previous = 777;
	string temp;
	root = new node();
	int arr[rowSize][colSize];

	inFilename = filename;
	blankRow = 0;
	blankCol = 0;
	twoByTwo = 977; // random throwaway value
	root->parent = NULL;
	root->depth = 0;

	validOpen = openFile(in, filename);

	if (!validOpen)
	{
		printf("Invalid filename.\n");
		return;
	}

	for (int i = 0; i < 10; i++)
	{
		nums[i] = 0;
	}
	
	while (!in.eof())
	{
		getline(in, temp, ' ');

		if ( !(string::npos == temp.find("\n")) )
		{
			check = true;
		}

		temp = temp.substr(0, temp.find("\n"));

		if (!temp.empty() && (temp != "\n") ) {
			if (temp == "b") {
				arr[row][col] = blank;
			}
			else {
				arr[row][col] = stoi(temp);
			}
			col++;
		}

		if (check) {
			col = 0;
			row++;
			check = false;
		}
	}

	// Copy array to root and find tiles
		for (int i = 0; i < rowSize; i++)
		{
			for (int j = 0; j < colSize; j++)
			{
				root->arr[i][j] = arr[i][j];
	
				if (root->arr[i][j] != blank) {
					nums[root->arr[i][j]]++;
	
					if (root->arr[i][j] == previous)
					{
						oneByTwo.emplace_front(root->arr[i][j]);
					}
	
					previous = root->arr[i][j];
				}
			}
		}

	// Add the remaining numbers to the correct lists
	for (int i = 0; i < 10; i++)
	{
		if (nums[i] == 1)
			oneByOne.emplace_front(i);
		else if (nums[i] == 4)
			twoByTwo = i;
		else if ((nums[i] == 2) && (find(oneByTwo.begin(), oneByTwo.end(), i) == oneByTwo.end()))
			twoByOne.emplace_front(i);
	}
	oneByTwo.remove(twoByTwo); // remove four by four from the oneByTwo list
	outputLists();
	std::cout << endl;
	std::cout << "initial board:\n";
	printBoard(*root);
	std::cout << endl;
}

// Destructor
HRSearch::~HRSearch()
{
	node *d;

	// Delete frontier list
	for (int i = 0; i < frontier.size(); i++)
	{
		d = frontier.front();
		frontier.pop_front();
		delete d;
	}

	// Delete extracted list
	for (int i = 0; i < extracted.size(); i++)
	{
		d = extracted.front();
		extracted.pop_front();
		delete d;
	}

	delete root;
}

// Makes a move on the board based on the direction and type of search being used
// The way I have it set up, it will consider moving the blocks 1 or 2 spaces as different moves, if it is possible to move a block 2 spaces.
bool HRSearch::makeMove(direction dir, search s)
{
	bool moveable1 = false, moveable2 = false;
	long temp;

	blankRow = 0;
	blankCol = -1;
	newN = new node();
	newN->parent = current;
	copyBoard(newN->parent, newN);
	switch (dir)
	{
	case UP:
		moveable1 = moveBlankUp(newN);
		break;
	case DOWN:
		moveable1 = moveBlankDown(newN);
		break;
	case LEFT:
		moveable1 = moveBlankLeft(newN);
		break;
	case RIGHT:
		moveable1 = moveBlankRight(newN);
		break;
	default:
		break;
	}
	
	// If it is possible to shift a block by 2 places as well as 1 place
	if (moves < numBlanks) {
		nN = new node();
		nN->parent = current;
		copyBoard(nN->parent, nN);
		switch (dir)
		{
		case UP:
			moveable2 = moveBlankUp(nN);
			break;
		case DOWN:
			moveable2 = moveBlankDown(nN);
			break;
		case LEFT:
			moveable2 = moveBlankLeft(nN);
			break;
		case RIGHT:
			moveable2 = moveBlankRight(nN);
			break;
		default:
			break;
		}

		if (moveable2) {
			temp = getNum(nN);
			if (foundBoards.find(temp) == foundBoards.end()) {
				foundBoards.insert(temp);
				nN->depth = current->depth + 1;
				if (targetFound(*nN))
				{
					current = nN;
					return true;
				}
				else {
					if (s == bfs)
						frontier.emplace_back(nN);
					else
						frontier.emplace_front(nN);
				}
			}
			else
				delete nN;
		}
		else
			delete nN;
	}

	// If the block was moveable
	if (moveable1) {
		temp = getNum(newN);
		if (foundBoards.find(temp) == foundBoards.end()) {
			foundBoards.insert(temp);
			newN->depth = current->depth + 1;
			if (targetFound(*newN))
			{
				current = newN;
				return true;
			}
			else {
				if (s == bfs)
					frontier.emplace_back(newN);
				else
					frontier.emplace_front(newN);
			}
		}
		else
			delete newN;
	}
	else
		delete newN;

	moves = 0;

	return false;
}

// Run BFS on HR Puzzle
void HRSearch::BFS() 
{
	size_t pos;
	moves = 0;
	current = root;

	pos = inFilename.find('.');
	outFilename = inFilename.substr(0, pos);
	outFilename.append("BFS.out");
	out.open(outFilename);

	startTimer();

	if (!targetFound(*root)) {
		while (true) {
			if (makeMove(UP, bfs))
				break;
			if (makeMove(DOWN, bfs))
				break;
			if(makeMove(LEFT, bfs))
				break;
			if(makeMove(RIGHT, bfs))
				break;

			if (frontier.size() > 0) {
				current = frontier.front();
				extracted.emplace_back(frontier.front());
				frontier.pop_front();
			}
			else {
				std::cout << "Frontier is empty. No solution found.\n";
				out.close();
				break;
			}
		} // end while
	} // end if

	endTimer();

	in.close();

	if (frontier.size() > 0) {
		outputToFile(out);
		cout << "Breadth-first search has found a solution. See output file for details.\n";
	}
	cout << "\nTotal of " << (frontier.size() + extracted.size()) << " nodes generated.\n\n";
}

// Run DFS on HR Puzzle
void HRSearch::DFS()
{
	size_t pos;
	bool found = false;

	moves = 0;
	current = root;

	pos = inFilename.find('.');
	outFilename = inFilename.substr(0, pos);
	outFilename.append("DFS.out");
	out.open(outFilename);

	startTimer();

	if (!targetFound(*root)) {
		while (true) {
			if (makeMove(UP, dfs))
				break;
			if (makeMove(DOWN, dfs))
				break;
			if(makeMove(LEFT, dfs))
				break;
			if(makeMove(RIGHT, dfs))
				break;
		
			if (frontier.size() > 0) {
				current = frontier.front();
				extracted.emplace_back(frontier.front());
				frontier.pop_front();
			}
			else {
				std::cout << "Frontier is empty. No solution found.\n";
				out.close();
				break;
			}
		}
	} // end if

	endTimer();

	in.close();

	if (frontier.size() > 0) {
		outputToFile(out);
		std::cout << "Breadth-first search has found a solution. See output file for details.\n";
	}
	else {
		std::cout << "Frontier is empty. No solution found.\n";
		out.close();
	}

	cout << "\nTotal of " << (frontier.size() + extracted.size()) << " nodes generated.\n\n";
}

// Writes final path to output file and closes it
void HRSearch::outputToFile(ofstream &outfile)
{
	int count = 0, x;
	node *arr[100000];

	// arrange list to print foward instead of backwards
	while (current != root)
	{
		arr[count] = current;
		count++;
		current = current->parent;
	}

	x = count;
	count = 0;
	// print list to output file
	for (int i = 0; i < x; i++)
	{
		outfile << (i) << endl;
		outfile << stringPuzzle(*arr[x - i - 1]);
	}

	outfile.close();
}

// Starts timer
void HRSearch::startTimer()
{
	timer = clock();
}

// Stops timer and outputs time
void HRSearch::endTimer()
{
	timer = clock() - timer;

	// Clock source: http://www.cplusplus.com/reference/ctime/clock/
	std::cout << "Time (s) to make calculations: " << ((float)timer) / CLOCKS_PER_SEC << endl;
}

// Move blank spot(s) up
bool HRSearch::moveBlankUp(node *n)
{
	bool moved = false;
	findNextBlank(n);

	// Valid to move blank up
	if (blankRow - 1 >= 0)
	{
		//// 1x1 2 moves
		//if ( blankRow - 2 >= 0 && find(oneByOne.begin(), oneByOne.end(), n->arr[blankRow - 2][blankCol]) != oneByOne.end() && n->arr[blankRow - 1][blankCol] == blank) {
		//	n->arr[blankRow][blankCol] = n->arr[blankRow - 2][blankCol];
		//	n->arr[blankRow - 2][blankCol] = blank;
		//	moves += 2;
		//	return true;
		//}
		// 1x1
		if (find(oneByOne.begin(), oneByOne.end(), n->arr[blankRow - 1][blankCol]) != oneByOne.end())
		{
			n->arr[blankRow][blankCol] = n->arr[blankRow - 1][blankCol];
			n->arr[blankRow - 1][blankCol] = blank;
			moves++;
			return true;
		}
		// 1x2
		if (find(oneByTwo.begin(), oneByTwo.end(), n->arr[blankRow - 1][blankCol]) != oneByTwo.end())
		{
			// blank to right
			if ((n->arr[blankRow][blankCol + 1] == blank) && (n->arr[blankRow - 1][blankCol + 1] == n->arr[blankRow - 1][blankCol]) )
			{
				n->arr[blankRow][blankCol] = n->arr[blankRow - 1][blankCol];
				n->arr[blankRow][blankCol + 1] = n->arr[blankRow - 1][blankCol + 1];
				n->arr[blankRow - 1][blankCol] = blank;
				n->arr[blankRow - 1][blankCol + 1] = blank;
				moves += 2;
				return true;
			}
			// blank to left
			else if ((n->arr[blankRow][blankCol - 1] == blank) && (n->arr[blankRow - 1][blankCol - 1] == n->arr[blankRow - 1][blankCol])) {
				n->arr[blankRow][blankCol] = n->arr[blankRow - 1][blankCol];
				n->arr[blankRow][blankCol - 1] = n->arr[blankRow - 1][blankCol - 1];
				n->arr[blankRow - 1][blankCol] = blank;
				n->arr[blankRow - 1][blankCol - 1] = blank;
				moves += 2;
				return true;
			}
		}
		// 2x1
		if ( blankRow - 2 >= 0 && find(twoByOne.begin(), twoByOne.end(), n->arr[blankRow - 1][blankCol]) != twoByOne.end())
		{
			n->arr[blankRow][blankCol] = n->arr[blankRow - 1][blankCol];
			n->arr[blankRow - 2][blankCol] = blank;
			moves++;
			return true;
		}
		//// 2x1 move 2 spaces (moves 2 spaces, but possible to move only one space)
		if ( blankRow - 3 >= 0 && find(twoByOne.begin(), twoByOne.end(), n->arr[blankRow - 2][blankCol]) != twoByOne.end() && n->arr[blankRow - 1][blankCol] == blank)
		{
			n->arr[blankRow][blankCol] = n->arr[blankRow - 2][blankCol];
			n->arr[blankRow - 1][blankCol] = n->arr[blankRow - 3][blankCol];
			n->arr[blankRow - 2][blankCol] = blank;
			n->arr[blankRow - 3][blankCol] = blank;
			moves++;
			return true;
		}
		// 2x2
		if ((blankRow - 2 >= 0) && (n->arr[blankRow][blankCol + 1] == blank) && (n->arr[blankRow - 1][blankCol] == twoByTwo))
		{
			// blank to right
			if (n->arr[blankRow][blankCol + 1] == blank && n->arr[blankRow - 1][blankCol] == n->arr[blankRow - 1][blankCol + 1]) {
				n->arr[blankRow][blankCol] = n->arr[blankRow - 1][blankCol];
				n->arr[blankRow][blankCol + 1] = n->arr[blankRow - 1][blankCol + 1];
				n->arr[blankRow - 2][blankCol] = blank;
				n->arr[blankRow - 2][blankCol + 1] = blank;
				moves += 2;
				return true;
			}
			// blank to left
			else if (n->arr[blankRow][blankCol - 1] == blank && n->arr[blankRow - 1][blankCol] == n->arr[blankRow - 1][blankCol - 1]) {
				n->arr[blankRow][blankCol] = n->arr[blankRow - 1][blankCol];
				n->arr[blankRow][blankCol + 1] = n->arr[blankRow - 1][blankCol - 1];
				n->arr[blankRow - 2][blankCol] = blank;
				n->arr[blankRow - 2][blankCol - 1] = blank;
				moves += 2;
				return true;
			}
		}
	}
	moves++;
	return false;
}

// Move blank spot(s) down
bool HRSearch::moveBlankDown(node *n)
{
	bool moved = false;
	findNextBlank(n);

	if (blankRow + 1 < rowSize)
	{
		// 1x1 2 spaces
		/*if (blankRow + 2 < rowSize && find(oneByOne.begin(), oneByOne.end(), n->arr[blankRow + 2][blankCol]) != oneByOne.end() && n->arr[blankRow + 1][blankCol] == blank)
		{
			n->arr[blankRow][blankCol] = n->arr[blankRow + 2][blankCol];
			n->arr[blankRow + 2][blankCol] = blank;
			moves += 2;
			return true;
		}*/
		// 1x1
		if (find(oneByOne.begin(), oneByOne.end(), n->arr[blankRow + 1][blankCol]) != oneByOne.end())
		{
			n->arr[blankRow][blankCol] = n->arr[blankRow + 1][blankCol];
			n->arr[blankRow + 1][blankCol] = blank;
			moves++;
			return true;
		}
		// 1x2
		if (find(oneByTwo.begin(), oneByTwo.end(), n->arr[blankRow + 1][blankCol]) != oneByTwo.end())
		{
			// blank to right
			if ((n->arr[blankRow][blankCol + 1] == blank) && (n->arr[blankRow + 1][blankCol + 1] == n->arr[blankRow + 1][blankCol]))
			{
				n->arr[blankRow][blankCol] = n->arr[blankRow + 1][blankCol];
				n->arr[blankRow][blankCol + 1] = n->arr[blankRow + 1][blankCol + 1];
				n->arr[blankRow + 1][blankCol] = blank;
				n->arr[blankRow + 1][blankCol + 1] = blank;
				moves += 2;
				return true;
			}
			// blank to left
			else if ((n->arr[blankRow][blankCol - 1] == blank) && (n->arr[blankRow + 1][blankCol - 1] == n->arr[blankRow + 1][blankCol])) {
				n->arr[blankRow][blankCol] = n->arr[blankRow + 1][blankCol];
				n->arr[blankRow][blankCol - 1] = n->arr[blankRow + 1][blankCol - 1];
				n->arr[blankRow + 1][blankCol] = blank;
				n->arr[blankRow + 1][blankCol - 1] = blank;
				moves += 2;
				return true;
			}
		}
		// 2x1 move 2 spaces (moves 2 spaces, but possible to move only one space)
		if ( (blankRow + 3 < rowSize) && find(twoByOne.begin(), twoByOne.end(), n->arr[blankRow + 2][blankCol]) != twoByOne.end() && n->arr[blankRow + 1][blankCol] == blank)
		{
			n->arr[blankRow][blankCol] = n->arr[blankRow + 2][blankCol];
			n->arr[blankRow + 1][blankCol] = n->arr[blankRow + 3][blankCol];
			n->arr[blankRow + 2][blankCol] = blank;
			n->arr[blankRow + 3][blankCol] = blank;
			moves++;
			return true;
		}
		// 2x1 move 1 space
		if ( blankRow + 2 < rowSize && find(twoByOne.begin(), twoByOne.end(), n->arr[blankRow + 1][blankCol]) != twoByOne.end()  )
		{
			n->arr[blankRow][blankCol] = n->arr[blankRow + 1][blankCol];
			n->arr[blankRow + 2][blankCol] = blank;
			moves++;
			return true;
		}
		// 2x2
		if ((blankRow + 2 < rowSize) && (n->arr[blankRow + 1][blankCol] == twoByTwo))
		{
			// blank to right
			if (n->arr[blankRow][blankCol + 1] == blank && n->arr[blankRow + 1][blankCol] == n->arr[blankRow + 1][blankCol + 1]) {
				n->arr[blankRow][blankCol] = n->arr[blankRow + 1][blankCol];
				n->arr[blankRow][blankCol + 1] = n->arr[blankRow + 1][blankCol + 1];
				n->arr[blankRow + 2][blankCol] = blank;
				n->arr[blankRow + 2][blankCol + 1] = blank;
				moves += 2;
				return true;
			}
			// blank to left
			else if (n->arr[blankRow][blankCol - 1] == blank && n->arr[blankRow + 1][blankCol] == n->arr[blankRow + 1][blankCol - 1]) {
				n->arr[blankRow][blankCol] = n->arr[blankRow + 1][blankCol];
				n->arr[blankRow][blankCol - 1] = n->arr[blankRow + 1][blankCol - 1];
				n->arr[blankRow + 2][blankCol] = blank;
				n->arr[blankRow + 2][blankCol - 1] = blank;
				moves += 2;
				return true;
			}
		}
	}
	moves++;
	return false;
}

// Move blank spot(s) left
bool HRSearch::moveBlankLeft(node *n)
{
	bool moved = false;
	findNextBlank(n);

	if (blankCol - 1 >= 0)
	{
		//1x1 2 spaces
		/*if (blankCol - 2 >= 0 && find(oneByOne.begin(), oneByOne.end(), n->arr[blankRow][blankCol - 2]) != oneByOne.end() && n->arr[blankRow][blankCol - 1] == blank)
		{
			n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol - 2];
			n->arr[blankRow][blankCol - 2];
			moves += 2;
			return true;
		}*/
		// 1x1
		if (find(oneByOne.begin(), oneByOne.end(), n->arr[blankRow][blankCol - 1]) != oneByOne.end())
		{
			n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol - 1];
			n->arr[blankRow][blankCol - 1] = blank;
			moves++;
			return true;
		}
		// 1x2
		if ( (blankCol - 2 >= 0) && (find(oneByTwo.begin(), oneByTwo.end(), n->arr[blankRow][blankCol - 1]) != oneByTwo.end()) )
		{
				n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol - 1];
				n->arr[blankRow][blankCol - 2] = blank;
				moves++;
				return true;
		}
		// 1x2 2 moves
		if ( blankCol -  3 >= 0 && find(oneByTwo.begin(), oneByTwo.end(), n->arr[blankRow][blankCol - 2]) != oneByTwo.end() && n->arr[blankRow][blankCol - 1] == blank)
		{
			n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol - 2];
			n->arr[blankRow][blankCol - 1] = n->arr[blankRow][blankCol - 3];
			n->arr[blankRow][blankCol - 2] = blank;
			n->arr[blankRow][blankCol - 3] = blank;
			moves++;
			return true;
		}
		// 2x1
		if ((find(twoByOne.begin(), twoByOne.end(), n->arr[blankRow][blankCol - 1]) != twoByOne.end()))
		{
			// blank below and 2x1 works
			if (((n->arr[blankRow + 1][blankCol] == blank) && (n->arr[blankRow][blankCol - 1] == n->arr[blankRow + 1][blankCol - 1])))
			{
				n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol - 1];
				n->arr[blankRow + 1][blankCol] = n->arr[blankRow + 1][blankCol - 1];
				n->arr[blankRow][blankCol - 1] = blank;
				n->arr[blankRow + 1][blankCol - 1] = blank;
				moves += 2;
				return true;
			}
			// blank above and 2x1 works
			else if ((n->arr[blankRow - 1][blankCol] == blank) && (n->arr[blankRow][blankCol - 1] == n->arr[blankRow - 1][blankCol - 1]))
			{
				n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol - 1];
				n->arr[blankRow - 1][blankCol] = n->arr[blankRow - 1][blankCol - 1];
				n->arr[blankRow][blankCol - 1] = blank;
				n->arr[blankRow - 1][blankCol - 1] = blank;
				moves += 2;
				return true;
			}
		}
		// 2x2
		if ( (blankCol - 2 >= 0) && (n->arr[blankRow][blankCol - 1] == twoByTwo) )
		{
			if ((n->arr[blankRow + 1][blankCol] == blank) && (n->arr[blankRow + 1][blankCol - 1] == twoByTwo))
			{
				n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol - 1];
				n->arr[blankRow + 1][blankCol] = n->arr[blankRow + 1][blankCol - 1];
				n->arr[blankRow][blankCol - 2] = blank;
				n->arr[blankRow + 1][blankCol - 2] = blank;
				moves += 2;
				return true;
			}
			else if ((n->arr[blankRow - 1][blankCol] == blank) && (n->arr[blankRow - 1][blankCol - 1] == twoByTwo))
			{
				n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol - 1];
				n->arr[blankRow - 1][blankCol] = n->arr[blankRow - 1][blankCol - 1];
				n->arr[blankRow][blankCol - 2] = blank;
				n->arr[blankRow - 1][blankCol - 2] = blank;
				moves += 2;
				return true;
			}
		}
	}
	moves++;
	return false;
}

// Move blank spot(s) right
bool HRSearch::moveBlankRight(node *n)
{
	bool moved = false;
		findNextBlank(n);

		if (blankCol + 1 < colSize)
		{
			// 1x1 2 moves
			/*if ( blankCol + 2 < colSize && find(oneByOne.begin(), oneByOne.end(), n->arr[blankRow][blankCol + 2]) != oneByOne.end() && n->arr[blankRow][blankCol + 2] == blank)
			{
				n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol + 2];
				n->arr[blankRow][blankCol] = blank;
				moves += 2;
				return true;
			}*/
			// 1x1
			if (find(oneByOne.begin(), oneByOne.end(), n->arr[blankRow][blankCol + 1]) != oneByOne.end())
			{
				n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol + 1];
				n->arr[blankRow][blankCol + 1] = blank;
				moves++;
				return true;
			}
			// 1x2 2 moves
			if (blankCol + 3 < colSize && find(oneByTwo.begin(), oneByTwo.end(), n->arr[blankRow][blankCol + 2]) != oneByTwo.end() && n->arr[blankRow][blankCol + 1] == blank)
			{
				n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol + 2];
				n->arr[blankRow][blankCol + 1] = n->arr[blankRow][blankCol + 3];
				n->arr[blankRow][blankCol + 2] = blank;
				n->arr[blankRow][blankCol + 3] = blank;
				moves++;
				return true;
			}
			// 1x2
			if ((blankCol + 2 < colSize) && (find(oneByTwo.begin(), oneByTwo.end(), n->arr[blankRow][blankCol + 1]) != oneByTwo.end()) )
			{
				n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol + 1];
				n->arr[blankRow][blankCol + 2] = blank;
				moves++;
				return true;
			}
			// 2x1
			if (find(twoByOne.begin(), twoByOne.end(), n->arr[blankRow][blankCol + 1]) != twoByOne.end())
			{
				// blank below
				if (blankRow + 1 < rowSize && n->arr[blankRow + 1][blankCol] == blank && n->arr[blankRow][blankCol + 1] == n->arr[blankRow + 1][blankCol + 1])
				{
					n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol + 1];
					n->arr[blankRow + 1][blankCol] = n->arr[blankRow + 1][blankCol + 1];
					n->arr[blankRow][blankCol + 1] = blank;
					n->arr[blankRow + 1][blankCol + 1] = blank;
					moves += 2;
					return true;
				}
				// blank above
				else if (blankRow - 1 >= 0 && n->arr[blankRow - 1][blankCol] == blank && n->arr[blankRow][blankCol + 1] == n->arr[blankRow - 1][blankCol + 1]) {
					n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol + 1];
					n->arr[blankRow - 1][blankCol] = n->arr[blankRow - 1][blankCol + 1];
					n->arr[blankRow][blankCol + 1] = blank;
					n->arr[blankRow - 1][blankCol + 1] = blank;
					moves += 2;
					return true;
				}
			}
			// 2x2
			if ((blankCol + 2 < colSize) && (n->arr[blankRow][blankCol + 1] == twoByTwo))
			{
				if (n->arr[blankRow + 1][blankCol] == blank && (n->arr[blankRow + 1][blankCol + 1] == twoByTwo))
				{
					n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol + 1];
					n->arr[blankRow + 1][blankCol] = n->arr[blankRow + 1][blankCol + 1];
					n->arr[blankRow][blankCol + 2] = blank;
					n->arr[blankRow + 1][blankCol + 2] = blank;
					moves += 2;
					return true;
				}
				else if (n->arr[blankRow - 1][blankCol] == blank && (n->arr[blankRow - 1][blankCol + 1] == twoByTwo))
				{
					n->arr[blankRow][blankCol] = n->arr[blankRow][blankCol + 1];
					n->arr[blankRow - 1][blankCol] = n->arr[blankRow - 1][blankCol + 1];
					n->arr[blankRow][blankCol + 2] = blank;
					n->arr[blankRow - 1][blankCol + 2] = blank;
					moves += 2;
					return true;
				}
			}
		}
	moves++;
	return false;
}

// finds the next blank
void HRSearch::findNextBlank(node *n)
{
	do{
		blankCol++;

		if (blankCol % 4 == 0)
		{
			blankRow++;
			blankCol = 0;
		}

		if (blankRow >= 5)
			blankRow = 0;

	} while (n->arr[blankRow][blankCol] != blank);
}

// Tells if we have reached our target or not
bool HRSearch::targetFound(node n)
{
	if ( (n.arr[4][1] == twoByTwo) && (n.arr[4][2] == twoByTwo) && (n.arr[3][1] == twoByTwo) && (n.arr[3][2] == twoByTwo) )
		return true;

	return false;
}

// copy a 2D board from source node to target node
void HRSearch::copyBoard(node *source, node *target)
{
	for (int i = 0; i < rowSize; i++)
		for (int j = 0; j < colSize; j++)
			target->arr[i][j] = source->arr[i][j];
}

// Print the 2D array of a node
void HRSearch::printBoard(node n) 
{
	for (int i = 0; i < rowSize; i++)
	{
		for (int j = 0; j < colSize; j++)
		{
			if (n.arr[i][j] != blank)
				std:: cout << n.arr[i][j] << " ";
			else
				std::cout << "  ";
		}
		std::cout << endl;
	}
}

// Outputs the lists for pieces dimensions
void HRSearch::outputLists()
{
	std::cout << "One by one list: ";
	for (list<int>::iterator it = oneByOne.begin(); it != oneByOne.end(); it++)
		std::cout << *it << " ";

	std::cout << "\nOne by two list: ";
	for (list<int>::iterator it = oneByTwo.begin(); it != oneByTwo.end(); it++)
		std::cout << *it << " ";

	std::cout << "\nTwo by one list: ";
	for (list<int>::iterator it = twoByOne.begin(); it != twoByOne.end(); it++)
		std::cout << *it << " ";

	std::cout << "\nTwo by two: " << twoByTwo << endl;
}

// From Dr. Yu's 8-Puzzle program, for determining duplicate graphs
long HRSearch::getNum(node* n)
{
	long num = 0;

	for (int i = 0; i < rowSize; i++)
		for (int j = 0; j < colSize; j++)
			num = num * 2 + n->arr[i][j];

	n->num = num;

	return num;
}

// Opens input file, returns if open was valid
bool HRSearch::openFile(ifstream &infile, string filename)
{
	infile.open(filename);

	if (!infile.is_open())
	{
		cerr << "Error opening file.\n";
		return false;
	}
	return true;
}

// Makes a string representation of the 2d array
string HRSearch::stringPuzzle(node n)
{
	stringstream s;

	for (int i = 0; i < rowSize; i++)
	{
		for (int j = 0; j < colSize; j++)
		{
			if (n.arr[i][j] == blank)
			{
				s << "  ";
			}
			else {
				s << n.arr[i][j];
				s << " ";
			}
		}
		s << "\n";
	}
	s << "\n";

	return s.str();
}