// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include "d3d9.h"
#include "d3dx9tex.h"
#include <Windows.h>
#include "time.h"
#include "regex"

#include <iostream>
using namespace std;

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

string GetName();
void ScrShot();



