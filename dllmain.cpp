// dllmain.cpp : D��finit le point d'entr��e de l'application DLL.
// MHWDLLMod 005
#include <fstream>
#include <queue>
#include <functional>

#include <random>
#include <iostream>
#include <cmath>

#include <windows.h>

#include "minhook/MinHook.h"
#include "json/json.hpp"
#include "loader.h"
#include "ghidra_export.h"
#include "util.h"
#include <thread>

#include "Base.h"
#include "Component.h"
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
using json = nlohmann::json;

json config = json::object();
json set = json::object();
bool JoyL = false;
int joyl_ = 0;
float KeyA = 1;
float KeyY = 1;
float KeyRT = 1.0;
int isspr = 0;
int rd = 0;
int sp = 0;
int spc = 1;
int spq = 1;
int spu = 0;
int TK, DS, CC;
static void* (*spiritBlade_LvUP)(void*) = (void* (*)(void*))0x142122570;//0x1421211F0;
//static void* (*effects)(void*, int, int) = (void* (*)(void*, int, int))0x140B01880;
//142123DBF 14061770E
static void* (*spiritBlade_Refresh)(void*) = (void* (*)(void*))0x142123DBF;

void showMessage(std::string message) {
	MH::Chat::ShowGameMessage(*(undefined**)MH::Chat::MainPtr, (undefined*)&message[0], -1, -1, 0);
}

struct keyboardkeys
{
	int keyset_Y = VK_LBUTTON;
	int keyset_X = 69;
	int keyset_B = VK_RBUTTON;
	int keyset_A = VK_SPACE;
	int keyset_RT = 17;
	int keyset_RB = VK_LSHIFT;
	int keyset_LT = 67;
	int keyset_LU = 87;
	int keyset_LD = 83;
	int keyset_LL = 65;
	int keyset_LR = 68;
	float Y = 0;
	float X = 0;
	float B = 0;
	float A = 0;
	float RT = 0;
	float RB = 0;
	float LT = 0;
	float LU = 0;
	float LD = 0;
	float LL = 0;
	float LR = 0;
} Keys;

void init_json() {
	int X = config["X"];
	int Y = config["Y"];
	int A = config["A"];
	int B = config["B"];
	int RT = config["RT"];
	int RB = config["RB"];
	int LT = config["LT"];
	int LU = config["LU"];
	int LD = config["LD"];
	int LL = config["LL"];
	int LR = config["LR"];
	TK = set["TK"];
	DS = set["DS"];
	CC = set["CC"];
	Keys.keyset_X = X;
	Keys.keyset_Y = Y;
	Keys.keyset_A = A;
	Keys.keyset_B = B;
	Keys.keyset_RT = RT;
	Keys.keyset_RB = RB;
	Keys.keyset_LT = LT;
	Keys.keyset_LU = LU;
	Keys.keyset_LD = LD;
	Keys.keyset_LL = LL;
	Keys.keyset_LR = LR;
}

bool dirExists(const std::string& dirName_in) {
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false; //something is wrong with your path!
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
}

void load_json() {
	config["Y"] = VK_LBUTTON;
	config["A"] = VK_SPACE;
	config["B"] = VK_RBUTTON;
	config["X"] = 69;
	config["RT"] = 17;
	config["RB"] = VK_LSHIFT;
	config["LT"] = 67;
	config["LU"] = 87;
	config["LD"] = 83;
	config["LL"] = 65;
	config["LR"] = 68;
	set["TK"] = 0;
	set["DS"] = 0;
	set["CC"] = 1;
RESTART:
	ifstream i(".\\nativePC\\plugins\\iai\\key_config.json");
	ifstream f(".\\nativePC\\plugins\\iai\\config.json");
	if (!i.is_open() || i.eof()) {
		if (!dirExists(".\\nativePC\\plugins\\iai"))
			CreateDirectory(".\\nativePC\\plugins\\iai", NULL);
		ofstream myfile(".\\nativePC\\plugins\\iai\\key_config.json", fstream::out);
		//myfile << "{\n\"Y\":1,\n\"B\":2,\n\"A\":32,\n\"RT\":6,\n\"RB\":160,\n\"LT\":67\n}";
		myfile << config;
		myfile.close();
		goto RESTART;
	}
	if (!i.fail())
	{
		config.clear();
		i >> config;
		i.close();
	}
	if (!f.is_open() || f.eof()) {
		ofstream myfile(".\\nativePC\\plugins\\iai\\config.json", fstream::out);
		myfile << set;
		myfile.close();
	}
	if (!f.fail())
	{
		set.clear();
		f >> set;
		f.close();
	}
	else { return; }

	init_json();
	return;
}

/*void GetNowKey()
{
	JOYINFO joyinfo;
	JOYINFOEX joyinfoex;
	joyinfoex.dwSize = sizeof(JOYINFOEX);
	joyinfoex.dwFlags = JOY_RETURNALL;

	for (;;)
	{
		UINT joy_nums;
		joy_nums = joyGetNumDevs();
		if (joy_nums > 0)
		{
			MMRESULT joy_ret = joyGetPosEx(JOYSTICKID1, &joyinfoex);
			if (joy_ret == JOYERR_NOERROR)
			{
				cout << joyinfoex.dwButtons << endl;
				if (joyinfoex.dwButtons == 1)
					Keys.A = Keys.A + 0.0166;

				if (joyinfoex.dwButtons == 2)
					Keys.B = Keys.B + 0.0166;

				if (joyinfoex.dwButtons == 8)
					Keys.Y = Keys.Y + 0.0166;

				if (joyinfoex.dwButtons == 4)
					Keys.X = Keys.X + 0.0166;

				if (joyinfoex.dwButtons == 32)
					Keys.RB = Keys.RB + 0.0166;

				if (joyinfoex.dwZpos < 32767)
					Keys.RT = Keys.RT + 0.0166;

				if (joyinfoex.dwZpos > 32767)
					Keys.LT = Keys.LT + 0.0166;
			}
		}

		if (GetAsyncKeyState(Keys.keyset_Y) < 0)
			Keys.Y = Keys.Y + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_X) < 0)
			Keys.X = Keys.X + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_B) < 0)
			Keys.B = Keys.B + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_A) < 0)
			Keys.A = Keys.A + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LT) < 0)
			Keys.LT = Keys.LT + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_RB) < 0)
			Keys.RB = Keys.RB + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_RT) < 0)
			Keys.RT = Keys.RT + 0.0166;

		if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(Keys.keyset_Y) == 0)
			Keys.Y = 0;

		if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(Keys.keyset_X) == 0)
			Keys.X = 0;

		if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(Keys.keyset_B) == 0)
			Keys.B = 0;

		if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(Keys.keyset_A) == 0)
			Keys.A = 0;

		if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(Keys.keyset_RB) == 0)
			Keys.RB = 0;

		if ((joyinfoex.dwZpos == 32767 || joyinfoex.dwZpos == 0) && GetAsyncKeyState(Keys.keyset_RT) == 0)
			Keys.RT = 0;

		if ((joyinfoex.dwZpos == 32767 || joyinfoex.dwZpos == 0) && GetAsyncKeyState(Keys.keyset_LT) == 0)
			Keys.LT = 0;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	// return;
}*/

//�򵥻�ȡ����
void GetNowKey()
{
	Sleep(10000);
	void* Gamepad = *(undefined**)MH::GamePad::XboxPadPtr;
	for (;;) {
		Gamepad = *(undefined**)MH::GamePad::XboxPadPtr;
		if (Gamepad == nullptr)continue;
		if (*offsetPtr<float>(Gamepad, 0xC98) > 0)
			Keys.A = Keys.A + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC94) > 0)
			Keys.B = Keys.B + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC90) > 0)
			Keys.Y = Keys.Y + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC9C) > 0)
			Keys.X = Keys.X + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC84) > 0)
			Keys.RB = Keys.RB + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC8C) > 0)
			Keys.RT = Keys.RT + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC88) > 0)
			Keys.LT = Keys.LT + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xCA0) > 0)
			Keys.LU = *offsetPtr<float>(Gamepad, 0xCA0);

		if (*offsetPtr<float>(Gamepad, 0xCA8) > 0)
			Keys.LD = *offsetPtr<float>(Gamepad, 0xCA8);

		if (*offsetPtr<float>(Gamepad, 0xCAC) > 0)
			Keys.LL = *offsetPtr<float>(Gamepad, 0xCAC);

		if (*offsetPtr<float>(Gamepad, 0xCA4) > 0)
			Keys.LR = *offsetPtr<float>(Gamepad, 0xCA4);

		if (GetAsyncKeyState(Keys.keyset_X) < 0)
			Keys.X = Keys.X + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_Y) < 0)
			Keys.Y = Keys.Y + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_B) < 0)
			Keys.B = Keys.B + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_A) < 0)
			Keys.A = Keys.A + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LT) < 0)
			Keys.LT = Keys.LT + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_RB) < 0)
			Keys.RB = Keys.RB + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_RT) < 0)
			Keys.RT = Keys.RT + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LU) < 0)
			Keys.LU = Keys.LU + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LD) < 0)
			Keys.LD = Keys.LD + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LL) < 0)
			Keys.LL = Keys.LL + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LR) < 0)
			Keys.LR = Keys.LR + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC90) == 0 && GetAsyncKeyState(Keys.keyset_Y) == 0)
			Keys.Y = 0;

		if (*offsetPtr<float>(Gamepad, 0xC9C) == 0 && GetAsyncKeyState(Keys.keyset_X) == 0)
			Keys.X = 0;

		if (*offsetPtr<float>(Gamepad, 0xC94) == 0 && GetAsyncKeyState(Keys.keyset_B) == 0)
			Keys.B = 0;

		if (*offsetPtr<float>(Gamepad, 0xC98) == 0 && GetAsyncKeyState(Keys.keyset_A) == 0)
			Keys.A = 0;

		if (*offsetPtr<float>(Gamepad, 0xC84) == 0 && GetAsyncKeyState(Keys.keyset_RB) == 0)
			Keys.RB = 0;

		if (*offsetPtr<float>(Gamepad, 0xC8C) == 0 && GetAsyncKeyState(Keys.keyset_RT) == 0)
			Keys.RT = 0;

		if (*offsetPtr<float>(Gamepad, 0xC88) == 0 && GetAsyncKeyState(Keys.keyset_LT) == 0)
			Keys.LT = 0;

		if (*offsetPtr<float>(Gamepad, 0xCA0) == 0 && GetAsyncKeyState(Keys.keyset_LU) == 0)
			Keys.LU = 0;

		if (*offsetPtr<float>(Gamepad, 0xCA8) == 0 && GetAsyncKeyState(Keys.keyset_LD) == 0)
			Keys.LD = 0;

		if (*offsetPtr<float>(Gamepad, 0xCAC) == 0 && GetAsyncKeyState(Keys.keyset_LL) == 0)
			Keys.LL = 0;

		if (*offsetPtr<float>(Gamepad, 0xCA4) == 0 && GetAsyncKeyState(Keys.keyset_LR) == 0)
			Keys.LR = 0;

		if (Keys.LU + Keys.LD + Keys.LL + Keys.LR > 0)
			JoyL = true;
		else
			JoyL = false;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	//return;
}
//ɾ����Ӻϵ��У�����ͨ����Ҳ�е���.	
void asm_edit() {
	Sleep(10000);
	DWORD pid;
	HWND hwnd = FindWindow(NULL, "MONSTER HUNTER: WORLD(421810)");
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	DWORD64 readByte = NULL;
	
	//��ӺϿ���
	char ASM_iai_not_decrease_blade_lv[] = { 0xEB };
	WriteProcessMemory(hprocess, (LPVOID)0X142122F60, ASM_iai_not_decrease_blade_lv, sizeof(ASM_iai_not_decrease_blade_lv), NULL);

	//�������������в�
	char not_cleat_gauge[] = { 0x90,0x90,0x90,0x90,0x90,0x90 };
	WriteProcessMemory(hprocess, (LPVOID)0x14212259B, not_cleat_gauge, sizeof(not_cleat_gauge), NULL);

	//����
	char blade_efx[] = { 0X0F, 0X99, 0XC3 };
	WriteProcessMemory(hprocess, (LPVOID)0x142123C1B, blade_efx, sizeof(blade_efx), NULL);

	return;
}

void fsm_derive(int input1, int input2) {
	void* PlayerObj = *(undefined**)MH::Player::PlayerBasePlot;
	void* PlayerBase = *offsetPtr<void*>(PlayerObj, 0x50);
	*offsetPtr<int>(PlayerBase, 0x6284) = input1;
	*offsetPtr<int>(PlayerBase, 0x6288) = input2;
	return;
}

void mian_loop() {
	//�ȴ�ʮ���ӱ����ָ��
	Sleep(10000);

	//�ֱ������ַ
	//void* key = *((undefined**)0x145224a98);

	//��һ�ַ
	void* PlayerObject = *(undefined**)MH::Player::PlayerBasePlot;

	//���뻺��
	int input[2] = { 0,0 };

	//�Ӻϳɹ����λ
	bool iai_suc = false;

	while (1) {
		//�߳�ÿ������60��,ģ��60֡ˢ��
		//Sleep(16);
		std::this_thread::sleep_for(std::chrono::milliseconds(16));

		//��ȡ���̰���
		//GetNowKey();

		//��ȡ��ַ,��Ϊ��ָ�������¿�ʼѭ��,������ʵ��Ƿ���ַ
		if (PlayerObject == nullptr) continue;
		void* PlayerBase = *offsetPtr<void*>(PlayerObject, 0x50);
		if (PlayerBase == nullptr) continue;
		void* MutekiPtr = *offsetPtr<void*>(PlayerBase, 0x6a8);
		if (MutekiPtr == nullptr) continue;
		void* playeroff = *offsetPtr<void*>(PlayerBase, 0x76B0);
		if (playeroff == nullptr) continue;
		void* healthoff = *offsetPtr<void*>(PlayerBase, 0x7630);
		if (healthoff == nullptr) continue;
		void* wepoff = *offsetPtr<void*>(PlayerBase, 0xc0);
		if (wepoff == nullptr) continue;
		void* actoff = *offsetPtr<void*>(PlayerBase, 0x468);
		if (actoff == nullptr) continue;
		void* Cameraangle = *offsetPtr<void*>(PlayerBase, 0x7DC8);
		if (Cameraangle == nullptr) continue;
		void* Playangle = *offsetPtr<void*>(PlayerBase, 0x174);
		if (Playangle == nullptr) continue;
		wepoff = *offsetPtr<void*>(wepoff, 0x8);
		wepoff = *offsetPtr<void*>(wepoff, 0x78);

		int angle = 0;
		if ((Cameraangle > 0 && Playangle > 0) || (Cameraangle < 0 && Playangle < 0)) angle = 1;
		else if ((Cameraangle < 0 && Playangle > 0) || (Cameraangle > 0 && Playangle < 0)) angle = 0;

		//ǰ
		if (Keys.LU != 0.0 && Keys.LU >= Keys.LL && Keys.LU >= Keys.LR) {
			if (angle) joyl_ = 1;
			else joyl_ = 4;
		}
		//��
		else if (Keys.LD != 0.0 && Keys.LD >= Keys.LL && Keys.LD >= Keys.LR) {
			if (angle) joyl_ = 4;
			else joyl_ = 1;
		}
		//��
		else if (Keys.LR > Keys.LU && Keys.LR > Keys.LD)
			joyl_ = 3;
		//��
		else if (Keys.LL > Keys.LU && Keys.LL > Keys.LD)
			joyl_ = 2;
		//��
		else
			joyl_ = 0;

		DWORD pid;
		HWND hwnd = FindWindow(NULL, "MONSTER HUNTER: WORLD(421810)");
		GetWindowThreadProcessId(hwnd, &pid);
		HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		DWORD64 readByte = NULL;
		char CatCar_not_decrease_blade[] = { 0x90,0x90,0x90,0x90,0x90,0x90,0x90 };
		char CatCar_not_decrease_hp[] = { 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90 };
		char decrease_blade[] = { 0x4C,0x89,0x89,0x68,0x23,0x00,0x00 };
		char decrease_bladelv[] = { 0x4C,0x89,0x89,0x70,0x23,0x00,0x00 };
		char decrease_hp[] = { 0xF3,0x0F,0x11,0x8F,0x28,0x76,0x00,0x00 }; 

		//è��
		if (CC) {
			if (*offsetPtr<float>(healthoff, 0x64) <= 0.1f) {
				//������
				if (*offsetPtr<int>(wepoff, 0x2e8) == 0x3) {
					WriteProcessMemory(hprocess, (LPVOID)0x1421245B4, CatCar_not_decrease_blade, sizeof(CatCar_not_decrease_blade), NULL);
					WriteProcessMemory(hprocess, (LPVOID)0x1421245BB, CatCar_not_decrease_blade, sizeof(CatCar_not_decrease_blade), NULL);
				}
				//����Ѫ����
				WriteProcessMemory(hprocess, (LPVOID)0x141F6F64D, CatCar_not_decrease_hp, sizeof(CatCar_not_decrease_hp), NULL);
			}
			else {
				WriteProcessMemory(hprocess, (LPVOID)0x141F6F64D, decrease_hp, sizeof(decrease_hp), NULL);
				WriteProcessMemory(hprocess, (LPVOID)0x1421245B4, decrease_blade, sizeof(decrease_blade), NULL);
				WriteProcessMemory(hprocess, (LPVOID)0x1421245BB, decrease_bladelv, sizeof(decrease_bladelv), NULL);
			}
		}
		//Ѫ������0.1
		if (*offsetPtr<float>(healthoff, 0x64) > 0.1f) {
			//����
			if (*offsetPtr<int>(PlayerBase, 0x6278) == 0x298)
				if (*offsetPtr<float>(actoff, 0x10c) <= 0.1f)
					if (JoyL) {
						rd = 1;
						fsm_derive(1, 0x29C); 
					}
			if (rd && *offsetPtr<int>(PlayerBase, 0x6278) == 0x29C && *offsetPtr<float>(actoff, 0x10c) >= 25.0f){
				fsm_derive(1, 0x0F);
				rd = 0;
			}
			//�ֶ�ĥ��
			if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC1AD || *offsetPtr<int>(actoff, 0xe9c4) == 0xC046) {
				if (spq != 0) {
					*offsetPtr<int>(playeroff, 0x20F8) += 30 * (4 - sp);
					spq = 0;
					sp = 0;
				}
			}
			else {
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC1AC) {
					if (*offsetPtr<float>(actoff, 0x10c) <= 15.0f && spc != 0) {
						*offsetPtr<int>(playeroff, 0x20F8) += 30;
						sp++;
						spc = 0;
					}
					if (*offsetPtr<float>(actoff, 0x10c) > 15.0f) spc = 1;
				}
				else if (*offsetPtr<int>(actoff, 0xe9c4) != 0xC1AC) {
					sp = 0;
					spc = 1;
					spq = 1;
					//�޸�����ĥ������
					if (*offsetPtr<int>(PlayerBase, 0x6278) == 0x295){
						if (*offsetPtr<int>(playeroff, 0x20F8) - spu == 30) {
							*offsetPtr<int>(playeroff, 0x20F8) += 120;
						}
					}
					spu = *offsetPtr<int>(playeroff, 0x20F8);
				}
			}
			//Ϊ̫������Ч
			if (*offsetPtr<int>(wepoff, 0x2e8) == 0x3) {
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC132) {
					if (KeyA <= 0) {
						if (*offsetPtr<float>(actoff, 0x10c) >= 100.0f) {
							if (Keys.A > 0) {
								//�����÷���ȡ���ɵ�
								if (DS) {
									*offsetPtr<int>(PlayerBase, 0x76a8) = 1;
									fsm_derive(3, 0x12);
								}
								else fsm_derive(1, 0x35);
								KeyA = 1;
							}
						}
					}
					/*if (*offsetPtr<float>(actoff, 0x10c) >= 80.0f) {
						if (KeyRT <= 0) {
							if (Keys.RT > 0.0) {
								//����RT˲��;Ӻϳ���(�޸���Ϸ����Ӻϴ���5֡�ӳٵ�����)
								fsm_derive(3, 0x66);
								KeyRT = 1;
							}
						}
					}
					KeyRT = Keys.RT;*/
					KeyA = Keys.A;
				}
				if (isspr) {
					if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC06B ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC077 ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC065 ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC153 ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC067 ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC07F ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC080 ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC165)
						if (*offsetPtr<float>(actoff, 0x10c) <= 1.0f) {
							//�޸������ȡ�����״̬����
							*offsetPtr<int>(PlayerBase, 0x76a8) = 1;
						}
					isspr = 0;
				}
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC06D) {
					isspr = 1;
					if (KeyA <= 0) {
						if (*offsetPtr<float>(actoff, 0x10c) >= 48.0f) {
							if (Keys.A > 0) {
								for (int A_count = 0; A_count < 8; A_count++) {
									//ѭ������RT������,����Ϊ���µ���RT+B��ϼ�
									if (Keys.RT > 0.0) {
										break;
									}
									//ÿ��ѭ����һ֡�ӳ�
									if (A_count == 7) {
										//�����÷���ȡ�������
										//ǰ����
										if (joyl_ == 1) fsm_derive(3, 0x13);
										//�󷭹�
										else if (joyl_ == 2) fsm_derive(3, 0x14);
										//�ҷ���
										else if (joyl_ == 3) fsm_derive(3, 0x15);
										//�󷭹�
										else if (joyl_ == 4) fsm_derive(3, 0x16);
										//����
										else fsm_derive(3, 0x13);
										KeyA = 1;
									}
									std::this_thread::sleep_for(std::chrono::milliseconds(16));
								}
							}
						}
					}
					KeyA = Keys.A;
				}
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC0AC) {
					if (*offsetPtr<float>(actoff, 0x10c) <= 120.0f) {
						if (Keys.RB > 0 || Keys.X > 0) {
							//���������ɵ�
							*offsetPtr<int>(PlayerBase, 0x76a8) = 0;
							fsm_derive(1, 0xD1);
						}
					}
				}
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC08C) {
					if (*offsetPtr<float>(actoff, 0x10c) >= 80.0f) {
						if (Keys.RB > 0 || Keys.X > 0) {
							//���������ɵ�
							if (JoyL)
								fsm_derive(3, 9);
							else
								fsm_derive(3, 6);
						}
					}
				}

				//��Ӻ���������
				if (*offsetPtr<int>(actoff, 0xe9c4) >= 49460 && *offsetPtr<int>(actoff, 0xe9c4) <= 49463) {
					if (iai_suc && *offsetPtr<float>(actoff, 0x10c) < 50.0f) {
						*offsetPtr<float>(PlayerBase, 0xE168) = 10.0f;
					}

					//�ھӺ�֡������60֡�Ϳ�ʼ��¼����
					//ͨ�����뻺�������Ԥ�����Ч��
					if (*offsetPtr<float>(actoff, 0x10c) >= 60.0f) {
						if (input[0] == 0 && input[1] == 0) {
							//��Ӻϳɹ�����
							if (iai_suc) {
								//��RT������,����ѭ��
								if (Keys.RT > 0.0) {
									for (int RT_count = 0; RT_count < 8; RT_count++) {
										//ѭ������A������,����Ϊ���µ���RT+A��ϼ�,���ɵ����뻺��
										if (Keys.A > 0) {
											input[0] = 3;
											input[1] = 0x62;
											break;
										}
										if (Keys.Y > 0) {
											break;
										}
										//ÿ��ѭ����һ֡�ӳ�
										std::this_thread::sleep_for(std::chrono::milliseconds(16));

										//��ѭ��5��Ҳû�а���������ϼ�������Ϊ���µ���RT��ͬʱ���㹻������������ն3���뻺��
										//����ϼ��ж���ʽ������Ϸ����,�����RT��5֡�ӳ�
										if (KeyRT <= 0.0 && RT_count == 7 && *offsetPtr<float>(playeroff, 0x2368) >= 0.2f) {
											input[0] = 3;
											input[1] = 0x44;
											KeyRT = 1.0;
										}
									}
								}
								//����A������,����ѭ��
								else if (Keys.A > 0) {
									for (int A_count = 0; A_count < 8; A_count++) {
										//ѭ������RT������,����Ϊ���µ���RT+A��ϼ�,���ɵ����뻺��
										if (Keys.RT > 0.0) {
											input[0] = 3;
											input[1] = 0x62;
											break;
										}
										//ÿ��ѭ����һ֡�ӳ�
										std::this_thread::sleep_for(std::chrono::milliseconds(16));

										//��ѭ��5��Ҳû�а���������ϼ�������Ϊ���µ���A�����������뻺��
										//����ϼ��ж���ʽ������Ϸ����,�����A��5֡�ӳ�
										if (KeyA <= 0 && A_count == 7) {
											input[0] = 3;
											//ǰ����
											if (joyl_ == 1) input[1] = 0x13;
											//�󷭹�
											else if (joyl_ == 2) input[1] = 0x14;
											//�ҷ���
											else if (joyl_ == 3) input[1] = 0x15;
											//�󷭹�
											else if (joyl_ == 4) input[1] = 0x16;
											//����
											else input[1] = 0x13;
											KeyA = 1;
										}
									}
								}
								//����Y������,����ѭ��
								else if (Keys.Y > 0) {
									for (int Y_count = 0; Y_count < 8; Y_count++) {
										//ѭ������RT������,����Ϊ���µ���RT+Y��ϼ�
										if (Keys.RT > 0.0) {
											break;
										}
										//ÿ��ѭ����һ֡�ӳ�
										std::this_thread::sleep_for(std::chrono::milliseconds(16));

										//��ѭ��5��Ҳû�а���������ϼ�������Ϊ���µ���Y����̤��ն���뻺��
										//����ϼ��ж���ʽ������Ϸ����,�����Y��5֡�ӳ�
										if (KeyY <= 0 && Y_count == 7) {
											if (JoyL) {
												input[0] = 3;
												input[1] = 0x55;
											}
											else {
												input[0] = 3;
												input[1] = 0x6A;
											}
											KeyY = 1;
											break;
										}
									}
								}
							}
						}

						//����Ҫ�Ӻϳɹ�������
						//������RB,�򽫿����ɵ����뻺��
						if (TK)
							if (Keys.RB > 0 || Keys.X > 0) {
								input[0] = 3;
								input[1] = 0x6B;
							}
					}
					KeyY = Keys.Y;
					KeyA = Keys.A;
					KeyRT = Keys.RT;
				}
				//������������ʽ,����֡������70֡,�����������е���ʽ,��������
				//����ʽ������60֡�Ժ�ʼ������������,����70֡�Ժ��������,������Ϸ�����Ԥ���뷽ʽ
				if (*offsetPtr<float>(actoff, 0x10c) >= 70.0f) {
					if (input[0] != 0 && input[1] != 0) {
						fsm_derive(input[0], input[1]);
						input[0] = 0; input[1] = 0;
					}
				}

				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC155) {
					*offsetPtr<int>(PlayerBase, 0x76a8) = 0;
					if (*offsetPtr<float>(actoff, 0x10c) >= 105.0f) {
						fsm_derive(1, 0);
					}
				}
				//������
				else { input[0] = 0; input[1] = 0; }

				//��Ӻϵ��˺�����
				//-1 = ���˺�,0xE=�������д�Ӻ�,0xD=����,0xC=һ��,���ﱾ��������,������������
				if (!iai_suc && *offsetPtr<int>(actoff, 0xe9c4) >= 49460 && *offsetPtr<int>(actoff, 0xe9c4) <= 49463 && *offsetPtr<int>(playeroff, 0x2d24) != 0xFFFFFFFF) {
					*offsetPtr<int>(playeroff, 0x2d24) = 0xFFFFFFFF;
				}
				if (iai_suc)*offsetPtr<int>(playeroff, 0x2d24) = 0xe;

				//��ӺϿ���
				if (*offsetPtr<BYTE>(playeroff, 0x2CED) == 1 && *offsetPtr<int>(actoff, 0xe9c4) >= 49460 && *offsetPtr<int>(actoff, 0xe9c4) <= 49463) {
					if (*offsetPtr<float>(playeroff, 0x2d10) != 0) {
						*offsetPtr<int>(playeroff, 0x2cec) = 1;
						spiritBlade_LvUP(playeroff);
					}
				}

				//��Ӻϵĳɹ���־λ�ж�
				if (iai_suc == false && *offsetPtr<BYTE>(playeroff, 0x2CED) == 1 && *offsetPtr<int>(actoff, 0xe9c4) >= 49460 && *offsetPtr<int>(actoff, 0xe9c4) <= 49463)
					iai_suc = true;
				else if (iai_suc == true && *offsetPtr<BYTE>(playeroff, 0x2CED) == 0 && (*offsetPtr<int>(actoff, 0xe9c4) < 49460 || *offsetPtr<int>(actoff, 0xe9c4) > 49463))
					iai_suc = false;
			}
		}
	}
}
__declspec(dllexport) extern bool Load()
{
	load_json();
	thread th1(asm_edit);
	th1.detach();
	thread th2(mian_loop);
	th2.detach();
	thread th3(GetNowKey);
	th3.detach();
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


