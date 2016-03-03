#pragma once
#include <sstream>
using namespace std;

#define MY_MAX_PATH 260
#define MY_MAX_LENGTH 65534

class LoadData
{
private:
	int numDataItems;
	int numDataReturned;
	string dir;
	string ext;
	string* filenames;

	void init(const char* filenameFull, int offset, bool discard);
	int countItems(const char* str);
	int countItems(wchar_t* str);

public:
	LoadData(const char* filenameFull, int offset, bool discard);
	~LoadData(void);

	bool hasNextString();
	string getDir();
	string getExt();
	string getNextString();
	int getNumStrings();
};

