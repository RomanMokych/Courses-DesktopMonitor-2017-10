// FullScreenScrShot.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "FullScreenScrShot.h"
#include "d3d9.h"
#include "d3dx9tex.h"
#include "string"
#include "regex"
#include "time.h"
#include <conio.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FULLSCREENSCRSHOT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FULLSCREENSCRSHOT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//-----------------------------
//
//-----------------------------
HWND hScrShot, hTime, hCount;
IDirect3D9* d3d = nullptr;
IDirect3DDevice9* g_pd3dDevice = nullptr;
D3DPRESENT_PARAMETERS parameters = {0};
int ScreenWidth;
int ScreenHeight;
//-----------------------------

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FULLSCREENSCRSHOT));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDI_FULLSCREENSCRSHOT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		400, 300, 300, 200, NULL, NULL, hInstance, NULL);
	//Nút chụp màn hình
	hScrShot = CreateWindowEx(0, L"BUTTON", L"Start screen Shot",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		10, 70, 150, 30, hWnd, (HMENU)IDC_SCRSHOT, hInst, NULL);
	CreateWindowEx(0, L"STATIC", L"Screenshot every : ", WS_CHILD | WS_VISIBLE, 10, 10, 120, 20, hWnd, NULL, hInst, NULL);
	hTime = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 130, 10, 40, 20, hWnd, NULL, hInst, NULL);
	hCount = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 130, 40, 40, 20, hWnd, NULL, hInst, NULL);
	CreateWindowEx(0, L"STATIC", L"time(s)", WS_CHILD | WS_VISIBLE, 175, 40, 50, 20, hWnd, NULL, hInst, NULL);

	//Khởi tạo
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	parameters.Windowed = TRUE;
	parameters.BackBufferCount = 1;
	parameters.BackBufferHeight = ScreenHeight;
	parameters.BackBufferWidth = ScreenWidth;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	parameters.hDeviceWindow = NULL;

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

string GetName()
{
	string date, name;

	time_t rawtime;
	struct tm * timeinfo;
	timeinfo = new tm;
	time(&rawtime);
	localtime_s(timeinfo, &rawtime);
	char c[50];
	asctime_s(c, timeinfo);
	date.append(c);
	
	regex word("[[:s:]]+");
	const string format = "_";
	name = "scrshot/";
	name += regex_replace(date, word, format, regex_constants::format_default);
	for (int i = 0; i < name.length(); i++)
		if (name[i] == ':')
			name[i] = '-';
	name += ".bmp";

	return name;
}


int ScreenShot(HWND hwnd, int s, int n)
{
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	int index = 1;
	liDueTime.QuadPart = s * -10000000;
	ShowWindow(hwnd, SW_HIDE);
	while (index <= n)
	{

		// Create a waitable timer.
		hTimer = CreateWaitableTimer(NULL, TRUE, L"WaitableTimer");
		if (NULL == hTimer)
		{
			MessageBox(hwnd, L"CreateWaitableTimer failed", L"Error", MB_OK | MB_ICONERROR);
			return 1;
		}

		// Set a timer to wait for s seconds.
		if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
		{
			MessageBox(hwnd, L"SetWaitableTimer failed", L"Error", MB_OK | MB_ICONERROR);
			return 2;
		}

		// Wait for the timer.
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			MessageBox(hwnd, L"WaitForSingleObject failed", L"Error", MB_OK | MB_ICONERROR);
		else
		{
			IDirect3DSurface9* pSurface = nullptr;
			d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parameters, &g_pd3dDevice);
			g_pd3dDevice->BeginScene();
			g_pd3dDevice->CreateOffscreenPlainSurface(ScreenWidth, ScreenHeight,
				D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL);
			g_pd3dDevice->GetFrontBufferData(0, pSurface);

			string strName = GetName();
			wstring wName(strName.begin(), strName.end());
			const wchar_t* cw = wName.c_str();
			D3DXSaveSurfaceToFileW(cw, D3DXIFF_BMP, pSurface, NULL, NULL);
			pSurface->Release();
			Beep(600, 200);
			_getch();
		
		}
		index++;
	}
	ShowWindow(hwnd, SW_SHOW);
	return 0;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_SCRSHOT:
		{
			int lTime = GetWindowTextLength(hTime);
			int lCount = GetWindowTextLength(hCount);
			if (lTime > 0 && lCount > 0)
			{
				WCHAR* time = new WCHAR(lTime);
				WCHAR* count = new WCHAR(lCount);
				GetWindowText(hTime, time, lTime+1);
				GetWindowText(hCount, count, lCount+1);
				int iTime = _wtoi(time);
				int iCount = _wtoi(count);
				ScreenShot(hWnd, iTime, iCount);
			}
		}
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			g_pd3dDevice->Release();
			d3d->Release();
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
