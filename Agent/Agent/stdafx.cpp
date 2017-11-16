// stdafx.cpp : source file that includes just the standard includes
// FullScreenScrShot.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

string GetName()
{
	char buffer[80];
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	char* format = "%c";
	strftime(buffer, 80, format, timeinfo);
	string name(buffer);
	regex re(" ");
	regex re1(":");

	string name1 = regex_replace(name, re, "_");
	string name2 = regex_replace(name1, re1, "_");
	name2 += ".jpg";
	return name2;
}

string SaveScreenShot()
{
	//1. nessesary variables
	IDirect3D9* d3d = nullptr;
	IDirect3DSurface9* pSurface = nullptr;
	IDirect3DDevice9* g_pd3dDevice = nullptr;
	D3DPRESENT_PARAMETERS parameters = { 0 };
	//2. initializing
	int ScreenWidth;
	int ScreenHeight;
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	parameters.Windowed = TRUE;
	parameters.BackBufferCount = 1;
	parameters.BackBufferHeight = ScreenHeight;
	parameters.BackBufferWidth = ScreenWidth;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	parameters.hDeviceWindow = NULL;


	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parameters, &g_pd3dDevice);
	g_pd3dDevice->BeginScene();
	g_pd3dDevice->CreateOffscreenPlainSurface(ScreenWidth, ScreenHeight,
		D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL);
	//getting a screen
	g_pd3dDevice->GetFrontBufferData(0, pSurface);

	//getting name to open it further
	string strName =  GetName();
	wstring wName(strName.begin(), strName.end());
	const wchar_t* cw = wName.c_str();
	//saving surface to .jpg file
	D3DXSaveSurfaceToFileW(cw, D3DXIFF_JPG, pSurface, NULL, NULL);

	pSurface->Release();
	g_pd3dDevice->Release();
	d3d->Release();

	return strName;
}

void fillPTree(pt::ptree & root, string token, size_t sizeOfFile)
{
	root.put("token", token);//this value is for server to make difference between agent and viewer
	root.put("size", sizeOfFile);

	
}

