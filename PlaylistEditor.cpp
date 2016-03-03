#ifndef UNICODE
#define UNICODE
#endif

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>
#include <windowsx.h>
#include <Commctrl.h>
#include "EditorController.h"
#include "resource.h"

#define NUM_BUTTONS 10
#define BUTTON_ID 100
#define LIST_ID 10

//forward declarations
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK MyListProc (HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
void setupUIElements(HWND);
void paintWindow(HWND, HDC);

//globals
int cxClient, cyClient;
HWND hButtons[NUM_BUTTONS];
HWND hList;
wstring titleString;
HINSTANCE hInst;
HFONT hFontTitle, hFontText, hFontGroup;
HBITMAP hBitUp, hBitDown, hBitDel;
EditorController* editCtrl;

int WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;
	const wchar_t CLASS_NAME[] = L"PlaylistEditorClass";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Sansa Sport Clip Playlist Editor", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 785, 600,
								NULL, NULL, hInstance, NULL);

	if (hwnd == 0)
		return 0;
	
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	PAINTSTRUCT ps;
	HDC hdc;
	
	switch(uMsg)
	{
	case WM_DESTROY:
		{
			if (editCtrl)
			{
				delete editCtrl;
				editCtrl = 0;
			}
			DeleteObject(hFontTitle);
			DeleteObject(hFontText);
			DeleteObject(hFontGroup);
			for (int i = 0; i < NUM_BUTTONS; i++)
				DestroyWindow(hButtons[i]);
			PostQuitMessage(0);
			return 0;
		}

	case WM_CREATE:
		{
			editCtrl = new EditorController();
			hFontTitle = CreateFont(30,0,0,0,0,0,0,0,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,0,0,0,L"Arial");
			hFontText = CreateFont(18,0,0,0,0,0,0,0,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,0,0,0,L"Arial");
			hFontGroup = CreateFont(16,0,0,0,0,0,0,0,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,0,0,0,L"Arial");
			hBitUp = (HBITMAP) LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, 0);
			hBitDown = (HBITMAP) LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 0, 0, 0);
			hBitDel = (HBITMAP) LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 0, 0, 0);
			setupUIElements(hwnd);
			return 0;
		}

	case WM_PAINT:
		{	
			hdc = BeginPaint(hwnd, &ps);
			paintWindow(hwnd, hdc);
			EndPaint(hwnd, &ps);
			return 0;
		}

	case WM_SIZE:
		{
			cxClient = LOWORD (lParam);
			cyClient = HIWORD (lParam);
			MoveWindow(hList, 30, 120, cxClient-60, cyClient-130, TRUE);
			return 0;
		}

	case WM_SETFOCUS:
		{
			SetFocus(hList);
			return 0;
		}

	case WM_LBUTTONDOWN:
		{
			SendMessage(hList, LB_SETSEL, FALSE, -1);
			break;
		}

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case BUTTON_ID:		//new List
				{
					editCtrl->resetList();
					SendMessage(hList, LB_RESETCONTENT, 0, 0);
					InvalidateRect(hwnd, NULL, TRUE);
					break;
				}

			case BUTTON_ID+1:	//open List
				{
					editCtrl->openList();
					SendMessage(hList, LB_RESETCONTENT, 0, 0);
					while (editCtrl->hasNextString())
					{
						string s = editCtrl->getNextString();
						SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM) s.c_str());
					}
					InvalidateRect(hwnd, NULL, TRUE);
					break;
				}

			case BUTTON_ID+2:	//save List
				{
					editCtrl->saveList();
					break;
				}

			case BUTTON_ID+3:	//add Tracks
				{
					editCtrl->openFiles();

					int numSelected = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
					int* selected = NULL;
					if (numSelected != 0)
					{
						selected = new int[numSelected];
						SendMessage(hList, LB_GETSELITEMS, (WPARAM) numSelected, (LPARAM) selected);
					}
					SendMessage(hList, LB_RESETCONTENT, 0, 0);
					while (editCtrl->hasNextString())
					{
						string s = editCtrl->getNextString();
						SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM) s.c_str());
					}
					if (numSelected != 0)
					{
						for (int i = 0; i < numSelected; i++)
							SendMessage(hList, LB_SETSEL, TRUE, selected[i]);
						if (selected)
							delete[] selected;
					}
					InvalidateRect(hwnd, NULL, TRUE);
					break;
				}

			case BUTTON_ID+4:	//up
				{
					int numSelected = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
					if (numSelected == 0)
						break;
					int* selected = new int[numSelected];
					SendMessage(hList, LB_GETSELITEMS, (WPARAM) numSelected, (LPARAM) selected);

					if (selected[0] == 0)
						break;
					editCtrl->updateFiles(selected, numSelected, TRUE);
					SendMessage(hList, LB_RESETCONTENT, 0, 0);
					while (editCtrl->hasNextString())
					{
						string s = editCtrl->getNextString();
						SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM) s.c_str());
					}
					SendMessage(hList, LB_SETSEL, FALSE, -1);
					for (int i = 0; i < numSelected; i++)
						SendMessage(hList, LB_SETSEL, TRUE, selected[i]-1);

					delete[] selected;
					selected = 0;
					InvalidateRect(hwnd, NULL, TRUE);
					break;
				}

			case BUTTON_ID+5:	//down
				{
					int numSelected = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
					if (numSelected == 0)
						break;
					int* selected = new int[numSelected];
					SendMessage(hList, LB_GETSELITEMS, (WPARAM) numSelected, (LPARAM) selected);

					if (selected[numSelected-1] == SendMessage(hList, LB_GETCOUNT, 0, 0) - 1)
						break;
					editCtrl->updateFiles(selected, numSelected, FALSE);
					SendMessage(hList, LB_RESETCONTENT, 0, 0);
					while (editCtrl->hasNextString())
					{
						string s = editCtrl->getNextString();
						SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM) s.c_str());
					}
					SendMessage(hList, LB_SETSEL, FALSE, -1);
					for (int i = 0; i < numSelected; i++)
						SendMessage(hList, LB_SETSEL, TRUE, selected[i]+1);

					delete[] selected;
					selected = 0;
					InvalidateRect(hwnd, NULL, TRUE);
					break;
				}

			case BUTTON_ID+6:	//delete
				{
					int numSelected = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
					int* selected = new int[numSelected];
					SendMessage(hList, LB_GETSELITEMS, (WPARAM) numSelected, (LPARAM) selected);
					editCtrl->removeFiles(selected, numSelected);
					SendMessage(hList, LB_RESETCONTENT, 0, 0);
					while (editCtrl->hasNextString())
					{
						string s = editCtrl->getNextString();
						SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM) s.c_str());
					}
					SendMessage(hList, LB_SETSEL, FALSE, -1);

					delete[] selected;
					selected = 0;
					InvalidateRect(hwnd, NULL, TRUE);
					break;
				}
			}
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void setupUIElements(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);

	// Buttons
	hButtons[0] = CreateWindow(L"Button", L"New List", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 25, 65, 100, 30,
							hwnd, (HMENU) BUTTON_ID, hInst, NULL);
	hButtons[1] = CreateWindow(L"Button", L"Open List", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 145, 65, 100, 30,
							hwnd, (HMENU) (BUTTON_ID + 1), hInst, NULL); 
	hButtons[2] = CreateWindow(L"Button", L"Save List", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 265, 65, 100, 30,
							hwnd, (HMENU) (BUTTON_ID + 2), hInst, NULL); 
	hButtons[3] = CreateWindow(L"Button", L"Add Tracks", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 420, 65, 100, 30,
							hwnd, (HMENU) (BUTTON_ID + 3), hInst, NULL);
	hButtons[4] = CreateWindow(L"Button", 0, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP, 580, 65, 40, 40,
							hwnd, (HMENU) (BUTTON_ID + 4), hInst, NULL); 
	hButtons[5] = CreateWindow(L"Button", 0, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP, 640, 65, 40, 40,
							hwnd, (HMENU) (BUTTON_ID + 5), hInst, NULL); 
	hButtons[6] = CreateWindow(L"Button", 0, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP, 700, 65, 40, 40,
							hwnd, (HMENU) (BUTTON_ID + 6), hInst, NULL);
	hButtons[7] = CreateWindow(L"Button", L"File", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 5, 45, 380, 60,
							hwnd, (HMENU) (BUTTON_ID + 7), hInst, NULL);
	hButtons[8] = CreateWindow(L"Button", L"Music", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 400, 45, 140, 60,
							hwnd, (HMENU) (BUTTON_ID + 8), hInst, NULL);
	hButtons[9] = CreateWindow(L"Button", L"Edit List", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 560, 45, 200, 70,
							hwnd, (HMENU) (BUTTON_ID + 9), hInst, NULL);

	for (int i = 0; i < NUM_BUTTONS-3; i++)
		SendMessage(hButtons[i], WM_SETFONT, (WPARAM)hFontText, TRUE);
	SendMessage(hButtons[7], WM_SETFONT, (WPARAM)hFontGroup, TRUE);
	SendMessage(hButtons[8], WM_SETFONT, (WPARAM)hFontGroup, TRUE);
	SendMessage(hButtons[9], WM_SETFONT, (WPARAM)hFontGroup, TRUE);

	SendMessage(hButtons[4], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitUp);
	SendMessage(hButtons[5], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitDown);
	SendMessage(hButtons[6], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitDel);

	// Listbox
	hList =	CreateWindow(L"Listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_MULTIPLESEL | LBS_STANDARD & ~LBS_SORT, 30, 120, rc.right-60, rc.bottom-130, 
				hwnd, (HMENU) LIST_ID, hInst, NULL);
	SendMessage(hList, WM_SETFONT, (WPARAM)hFontText, TRUE);
	SetWindowSubclass(hList, MyListProc, 0, 0);

	// Strings
	titleString = L"--== Sport Clip Playlist Editor ==--";
}

void paintWindow(HWND hwnd, HDC hdc)
{
	RECT rc;
	GetClientRect(hwnd, &rc);

	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, hFontTitle);
	TextOut(hdc, rc.right/2-5*titleString.length(), 10, titleString.c_str(), titleString.length());

	SelectObject(hdc, hFontText);
}

LRESULT CALLBACK MyListProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (msg)
    {
    case WM_LBUTTONDOWN:
		{
			if (hwnd == hList)
			{
				POINT pt;
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				ClientToScreen(hList, &pt);

				int index = LBItemFromPt(hList, pt, FALSE);
				if (index == -1)
				{
					SendMessage(hList, LB_SETSEL, FALSE, -1);
					return TRUE;
				}
			}
		}
    } 
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}