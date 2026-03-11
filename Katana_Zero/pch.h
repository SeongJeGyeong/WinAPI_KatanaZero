#pragma once

#include <iostream>
#include <windows.h>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>	// std::function
#include <algorithm>	// std::find_if, std::sort
#include <stack>
#include <queue>

// C++ 20
#include <format>		// std::format("x:{0}, y:{1}"
#include <numbers>  
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

// transparent Blt
#pragma comment(lib, "msimg32.lib")

// Json
#include <fstream>
#include "Json/nlohmann/json.hpp"
using json = nlohmann::json;

// 사운드
//#include <MMSystem.h>
//#include <dsound.h>
//#pragma comment (lib, "winmm.lib")
//#pragma comment (lib, "dsound.lib")

#include <xaudio2.h>
#include <mmreg.h>
#pragma comment(lib, "xaudio2.lib")

//#pragma comment(lib, "avformat.lib")
//#pragma comment(lib, "avcodec.lib")
//#pragma comment(lib, "swresample.lib")
//#pragma comment(lib, "swscale.lib")
//#pragma comment(lib, "avutil.lib")
//
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavutil/opt.h>
//#include <libavutil/imgutils.h>
//#include <libswscale/swscale.h>

using namespace std;

#define SAFE_DELETE(ptr)		\
	if (ptr)					\
	{							\
		delete ptr;				\
		ptr = nullptr;			\
	}

// 메모리 릭. json 라이브러리 뒤쪽에서 해야지 new 키워드랑 충돌하지 않는다.
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>

#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif

// 공용 선언들 
#include "Utils/Defines.h"