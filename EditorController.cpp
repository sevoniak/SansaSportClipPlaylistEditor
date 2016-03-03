#include "EditorController.h"


EditorController::EditorController(void)
{
	ld = NULL;
	masterList = NULL;
	numStrings = 0;
	numReturned = 0;
	pcMode = false;
}


EditorController::~EditorController(void)
{
	if (ld)
	{
		delete ld;
		ld = NULL;
	}

	if (masterList)
	{
		delete[] masterList;
		masterList = NULL;
	}
}

void EditorController::openList()
{
	char filenameFull[MY_MAX_LENGTH];
	char filename[256];

	prepDefDirStr(filenameFull, NULL, 0);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = filenameFull;
	ofn.nMaxFile = sizeof(filenameFull);
	ofn.lpstrFileTitle = filename;
	ofn.nMaxFileTitle = sizeof(filename);
	ofn.lpstrFilter = "m3u playlistss (*.m3u)\0*.M3U\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_EXPLORER;

	if (GetOpenFileName(&ofn))
	{
		string listContents = populateContentsString(filenameFull, false);
		if (ld)
			delete ld;
		ld = new LoadData(listContents.c_str(), 0, !pcMode);
		resetMasterList();
		addToMasterList();
	}
}

void EditorController::openFiles()
{
	char filenameFull[MY_MAX_LENGTH];

	prepDefDirStr(filenameFull, NULL, 0);
	
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = filenameFull;
	ofn.nMaxFile = sizeof(filenameFull);
	ofn.lpstrFilter = "mp3 music files (*.mp3)\0*.MP3\0m3u playlists (*.m3u)\0*.M3U\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;

	if (GetOpenFileName(&ofn))
	{
		if (ld)
			delete ld;
		ld = new LoadData(filenameFull, ofn.nFileOffset, !pcMode);
		string ext = ld->getExt();
		if (ext == "mp3")
			addToMasterList();
		if (ext == "m3u")
		{
			while (ld->hasNextString())
			{
				stringstream ss;
				string fileName = ld->getNextString();
				string fileContents = populateContentsString(fileName.c_str(), false);
				LoadData* subData = new LoadData(fileContents.c_str(), 0, pcMode);
				addToMasterList(subData);
			}
		}
	}
}

void EditorController::saveList()
{
	char filenameFull[MY_MAX_PATH];
	char filename[256];

	prepDefDirStr(filenameFull, "\\myList.m3u", 11);
	
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = filenameFull;
	ofn.nMaxFile = sizeof(filenameFull);
	ofn.lpstrFileTitle = filename;
	ofn.nMaxFileTitle = sizeof(filename);
	ofn.lpstrFilter = "m3u playlists (*.m3u)\0*.M3U\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
	{
		ofstream ofs(filenameFull);
		if (ofs)
		{
			ofs << "#EXTM3U" << endl;
			while (hasNextString())
				ofs << getNextString() << endl;
			ofs.close();
		}
	}
}

void EditorController::resetMasterList()
{
	if (masterList)
		delete[] masterList;
	masterList = NULL;
	numStrings = 0;
	numReturned = 0;
}

void EditorController::prepDefDirStr(char* str, char* name, int len)
{
	char currentDir[MY_MAX_PATH] = "\0";
	GetCurrentDirectory(MY_MAX_PATH, currentDir);

	for (int i = 0; i < MY_MAX_PATH; i++)
	{
		if (currentDir[i] != '\0')
			str[i] = currentDir[i];
		else
		{
			for (int j = 0; j < len; j++)
				str[i+j] = name[j];
			str[i+len] = '\0';
			break;
		}
	}
}

string EditorController::populateContentsString(const char* str, bool discard)
{
	stringstream ss;
	string buffer;

	ifstream fin(str);
	if (fin)
	{
		while(getline(fin, buffer).good())
		{
			if (buffer.c_str()[0] != '#')
			{
				if (discard)
				{
					size_t pos = buffer.find_last_of('\\');
					string substr = buffer.substr(pos+1);
					ss << substr << '\0';
				}
				else
					ss << buffer << '\0'; 
			}
		}
		ss << '\0';
	}
	return ss.str();
}

bool EditorController::hasNextString() 
{ 
	if (numReturned < numStrings)
		return true; 
	numReturned = 0;
	return false;
}

string EditorController::getNextString() 
{ 
	string s = masterList[numReturned]; 
	numReturned++;
	return s;
}

void EditorController::addToMasterList(LoadData* data)
{
	if (data == 0)
		data = ld;
	int newNum = data->getNumStrings();
	string* temp = new string[numStrings + newNum];
	for (int i = 0; i < numStrings; i++)
		temp[i] = masterList[i];
	int i = 0;
	while (data->hasNextString())
	{
		temp[numStrings + i] = data->getNextString();
		i++;
	}
	if (masterList)
		delete[] masterList;
	masterList = temp;
	numStrings += newNum;
}

void EditorController::resetList()
{
	if (ld)
		delete ld;
	ld = NULL;
	resetMasterList();
}

void EditorController::removeFiles(int* arr, int size)
{
	string* temp = new string[numStrings - size];
	for (int i = 0, j = 0; i < numStrings || j < size; i++)
	{
		if (i == arr[j])
			j++;
		else
			temp[i-j] = masterList[i];
	}
	if (masterList)
		delete[] masterList;
	masterList = temp;
	numStrings = numStrings - size;
	numReturned = 0;
}

void EditorController::updateFiles(int* arr, int size, bool dir)
{
	string* temp = new string[numStrings];
	string* toMove = new string[size];
	string* stay = new string[numStrings-size];

	for (int i = 0, j = 0, k = 0; i < numStrings; i++)
	{
		if (i == arr[j])
		{
			toMove[k] = masterList[i];
			k++;
			j++;
		}
		else
			stay[i-k] = masterList[i];
	}

	bool done = false;
	if (dir)
		for (int i = 0, j = 0; i < numStrings || j < size; i++)
		{
			if (!done && i+1 == arr[j])
			{
				temp[i] = toMove[j];
				j++;
				if (j == size)
					done = true;
			}
			else
				temp[i] = stay[i-j];
		}
	else
		for (int i = 0, j = 0; i < numStrings || j < size; i++)
		{
			if (!done && i == arr[j]+1)
			{
				temp[i] = toMove[j];
				j++;
				if (j == size)
					done = true;
			}
			else
				temp[i] = stay[i-j];
		}
	if (masterList)
		delete[] masterList;
	masterList = temp;
	numReturned = 0;
}