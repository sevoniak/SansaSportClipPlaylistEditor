#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include "LoadData.h"
using namespace std;

class EditorController
{
private:
	LoadData* ld;
	string* masterList;
	int numStrings;
	int numReturned;
	bool pcMode;

	void prepDefDirStr(char* str, char* name, int len);
	string populateContentsString(const char* str, bool discard);
	void addToMasterList(LoadData* data = 0);
	void resetMasterList();

public:
	EditorController(void);
	~EditorController(void);

	void openList();
	void openFiles();
	void saveList();
	void resetList();
	void removeFiles(int* arr, int size);
	void updateFiles(int* arr, int size, bool dir);

	bool hasNextString();
	string getNextString();
};

