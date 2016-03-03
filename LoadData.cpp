#include "LoadData.h"


LoadData::LoadData(const char* filenameFull, int offset, bool discard)
{
	numDataItems = 0;
	numDataReturned = 0;
	filenames = NULL;

	init(filenameFull, offset, discard);
}

LoadData::~LoadData(void)
{
	if (filenames)
	{
		delete[] filenames;
		filenames = NULL;
	}
}

void LoadData::init(const char* filenameFull, int offset, bool discard)
{
	char path[MY_MAX_PATH];

	if (offset > 0)
	{
		int disc = discard ? 2 : 0;

		for (int i = disc; i < offset; i++)
			path[i-disc] = filenameFull[i];
		path[offset-disc-1] = L'\\';
		path[offset-disc] = L'\0';
		dir = path;
	}
	else
		dir = "";

	const char* str = &(filenameFull[offset]);
	numDataItems = countItems(str);
	if (numDataItems > 0)
	{
		filenames = new string[numDataItems];

		int i = 0;
		for (int j = 0; j < numDataItems; j++)
		{
			stringstream name;
			while (str[i] != '\0')
			{
				name << str[i];
				i++;
			}
			filenames[j] = name.str();
			i++;
		}
		ext = filenames[0].substr(filenames[0].length() - 3);
	}
}

int LoadData::countItems(const char* str)
{
	int n = 0;
	bool foundOne = false;
	for (int i = 0; i < MY_MAX_LENGTH; i++)
	{
		if (str[i] == '\0')
		{
			if (foundOne)
				return n;
			else
			{
				n++;
				foundOne = true;
			}
		}
		else
			foundOne = false;
	}
	return -1;
}

string LoadData::getDir() { return dir; }

string LoadData::getExt() { return ext; }

bool LoadData::hasNextString() 
{ 
	if (numDataReturned < numDataItems)
		return true; 
	numDataReturned = 0;
	return false;
}

string LoadData::getNextString()
{
	stringstream ret;
	if (dir != "")
		ret << dir;
	ret << filenames[numDataReturned++];
	return ret.str();
}

int LoadData::getNumStrings() { return numDataItems; }
