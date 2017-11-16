// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <unordered_map>

#include "d3d9.h"
#include "d3dx9tex.h"
#include <Windows.h>
#include "time.h"
#include "regex"



#include <iostream>
using namespace std;
using boost::asio::ip::tcp;
namespace pt = boost::property_tree;

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

string GetName();
string SaveScreenShot();
void fillPTree(pt::ptree&, string, size_t);



