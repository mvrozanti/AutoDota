// DotaItemMover.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<windows.h>
#include<iostream>
#include<thread>
#include<map>
#pragma comment(lib, "user32.lib")

using namespace std;
/* Globals */
int ScreenX = GetDeviceCaps(GetDC(0), HORZRES);
int ScreenY = GetDeviceCaps(GetDC(0), VERTRES);
BYTE* ScreenData = new BYTE[3 * ScreenX*ScreenY];
bool isPicking = false;

inline void ScreenCap() {
	HDC hdc = GetDC(GetDesktopWindow());
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, ScreenX, ScreenY);
	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 24;
	bmi.biWidth = ScreenX;
	bmi.biHeight = -ScreenY;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = ScreenX * ScreenY;
	SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, ScreenX, ScreenY, hdc, 0, 0, SRCCOPY);
	GetDIBits(hdc, hBitmap, 0, ScreenY, ScreenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdc);
}

inline int PosR(int x, int y) {
	return ScreenData[3 * ((y*ScreenX) + x) + 2];
}

inline int PosG(int x, int y) {
	return ScreenData[3 * ((y*ScreenX) + x) + 1];
}

inline int PosB(int x, int y) {
	return ScreenData[3 * ((y*ScreenX) + x)];
}

inline int* colorAt(int x, int y) {
	return new int[3]{ PosR(x,y), PosG(x,y), PosB(x,y) };
}

void clickAt(int x, int y) {
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	SetCursorPos(x, y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	SetCursorPos(cursorPos.x, cursorPos.y);
}

void tryPickMid() {
	int tries = 0;
	while (isPicking) {
		int time1 = time(0);
		ScreenCap();
		int time2 = time(0);
		//cout << time2 - time1 << endl;
		int* boardColor = colorAt(1680, 10);//(top bar when you're about to enter game)
		int* midColor = colorAt(276, 949);
		if (midColor[0] == 31 && midColor[1] == 60 && midColor[2] == 68) {
			clickAt(276, 949);
			isPicking = false;
		}
		else if (
			(boardColor[0] != 38 || boardColor[1] != 42 || boardColor[2] != 47)) {
			Sleep(10);
		}
		else if (tries++ == 1000) {
			isPicking = false;
		}
		delete[] midColor;
		delete[] boardColor;
		int time3 = time(0);
		cout << time3 - time1 << endl;
	}
}

map<int, int*> SLOT_POS = {
	//{ 0, new int[2]{ 1579,799 } },
	{ 4, new int[2] { 1499,1020} },
	{ 5, new int[2] { 1579,1020} },
	{ 6, new int[2] { 1657,1020} },
	{ 7, new int[2] { 1499,963 } },
	{ 8, new int[2] { 1579,963 } },
	{ 9, new int[2] { 1657,963 } }
};

int from = -1, to = -1;

void exec() {
	int* slotPos1 = SLOT_POS.at(from);
	int* slotPos2 = SLOT_POS.at(to);
	POINT saved;
	GetCursorPos(&saved);
	SetCursorPos(slotPos1[0], slotPos1[1]);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	SetCursorPos(slotPos2[0], slotPos2[1]);
	//Sleep(1);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	SetCursorPos(saved.x, saved.y);
}

void addToStack(int slot) {
	cout << "added to stack: " + slot << endl;
	if (from == -1 && to == -1) {
		from = slot;
		cout << "from=" + from << endl;;
	}
	else if (from != -1 && to == -1) {
		to = slot;
		cout << "from=" + from << endl;;
		if (from != to) {
			exec();
		}
		cout << "exec'd";
		from = to = -1;
	}
	else if (from != -1 && to != -1) {
		to = -1;
		from = slot;
	}
}

HHOOK hHook;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{
		PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
		cout << pKey->scanCode << " ";
		cout << pKey->vkCode << "     ";
		switch (pKey->vkCode) {
		case 100://4
			addToStack(4);
			break;
		case 101://5
			addToStack(5);
			break;
		case 102://6
			addToStack(6);
			break;
		case 103://7
			addToStack(7);
			break;
		case 104://8
			addToStack(8);
			break;
		case 105://9
			addToStack(9);
			break;
		case 110://del (try pick mid)
			/*POINT p;
			GetCursorPos(&p);
			ScreenCap();
			int* color = colorAt(p.x, p.y);
			cout << "CursorAt(" << p.x << "," << p.y << ") = ";
			cout << color[0] << "," << color[1] << "," << color[2] << endl;*/
			if (!isPicking) {
				isPicking = true;
				tryPickMid();
			}
			break;
		}
	}
	CallNextHookEx(hHook, nCode, wParam, lParam);
	return 0;
}

void startLogging() {
	RegisterHotKey(NULL, 0xB1AC7B1A, MOD_ALT, VK_F12);
	HMODULE hInstance = GetModuleHandle(NULL);
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, NULL);
	MSG msg;
	GetMessage(&msg, NULL, NULL, NULL);
	UnhookWindowsHookEx(hHook);
}

int main() {
	if ((GetKeyState(VK_NUMLOCK) & 0x0001) == 0) {
		keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
		keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	}
	thread t = thread(startLogging);
	t.join();
	return 0;
}

