//
// Author:		James Callahan
// Class:		CS 438
// File:		CS438 Homework 2 - Callahan.cpp
// Assignment:	Homework 2
// Due Date:	6 February 2018
//
/*
	Performance Report:
		BFS:
			Currently my program will solve for all of the files in a timely manner when run without debugging

		DFS:
			DFS finds all soultions within a reasonable time. The longest one took a little over 20 seconds

		I count movements across two spaces as two instead of one, so the numbers seem off on some of the solutions.

		All the output files are included with the project, along with the ones from BFS that took too long.
*/

#include "stdafx.h"
#include "HRSearch.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	string filename = "", alg = "";
	
	cout << "Please enter the name of the first puzzle.\n";
	cout << "Enter Q or Quit to terminate the program\n";
	cin >> filename;
	filename.append(".in");

	while (filename != "q.in" && filename != "Q.in" && filename != "quit.in" && filename != "Quit.in") {
		cout << "\tSelect a search method:\n";
		cout << "\t1. Depth-first Search\n";
		cout << "\t2. Breadth-first Search\n";
		cin >> alg;

		HRSearch *h = new HRSearch(filename);
		cin.clear();
		cout.clear();

		cout << "\nWait a moment while the data is collected and cleaned up.\n\n\n";

		if (alg == "1")
			h->DFS();
		else
			h->BFS();

		delete h;

		alg = "";
		filename = "";

		printf("Please enter the name of the next puzzle.\n");
		printf("Enter Q or Quit to terminate the program.\n");
		cin >> filename;
		filename.append(".in");

		
	}

    return 0;
}

