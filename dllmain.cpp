// dllmain.cpp : Définit le point d'entrée de l'application DLL.
// MHWDLLMod 005
#include <fstream>

#include <random>
#include <iostream>

#include <windows.h>

#include "json/json.hpp"
#include "loader.h"
#include "ghidra_export.h"
#include "util.h"
#include <thread>

#include "Base.h"
#include "ControlProgram.h"
#define _USE_MATH_DEFINES
#include <string>
#include <conio.h>
#include <math.h>
#include "AOB memscan.h"
#include <MMSystem.h>
#pragma comment(lib,"Winmm.lib")

#pragma warning(disable:4996)

using namespace std;
using namespace loader;
using namespace nlohmann;


void mian_loop() {
	//等待十秒钟避免空指针
	Sleep(10000);

	//手柄输入基址
	//void* key = *((undefined**)0x145224a98);

	//玩家基址
	void* PlayerObject = *(undefined**)MH::Player::PlayerBasePlot;

	//输入缓存
	int input[2] = { 0,0 };

	DWORD pid;
	HWND hwnd = FindWindow(NULL, "MONSTER HUNTER: WORLD(421810)");
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);


	while (1) {
		//线程每秒运行60次,模拟60帧刷新
		//Sleep(16);
		std::this_thread::sleep_for(std::chrono::milliseconds(16));

		//获取键盘按键
		//GetNowKey();

		//获取基址,若为空指针则重新开始循环,避免访问到非法地址
		if (PlayerObject == nullptr) continue;
		void* PlayerBase = *offsetPtr<void*>(PlayerObject, 0x50);
		if (PlayerBase == nullptr) continue;
		void* actoff = *offsetPtr<void*>(PlayerBase, 0x468);
		if (actoff == nullptr) continue;


		int lmtID = *offsetPtr<int>(actoff, 0xE9C4);
		// GetAddressData(0x1451238C8, 'int') + GetAddressData(pointer:Player() + 0x10, 'int') * 0xf8 + 0x9c
		float* speedMultiplier = (float*)(*(int*)0x1451238C8 + *offsetPtr<int>(PlayerBase, 0x10) * 0xf8 + 0x9c);
		float wildSpeedMultiplier = 1.6923;
		//LOG(INFO) << "lmtID: " << lmtID;
		if (lmtID == 49322 || lmtID == 49509 || lmtID == 49516)
		{
			if (*speedMultiplier < wildSpeedMultiplier)
			{
				*speedMultiplier = wildSpeedMultiplier;
			}
		} else if (*speedMultiplier > 1)
		{
			*speedMultiplier = 1;
		}
	}

}
__declspec(dllexport) extern bool Load()
{
	thread th2(mian_loop);
	th2.detach();
	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		return Load();

	return TRUE;
}


