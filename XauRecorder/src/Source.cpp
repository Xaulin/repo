#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HWND window;
HWND button;
HWND button2;
HWND stat;
HWND edit;
HWND prgb;

int NUMPTS;
short* waveIn = 0;
const int sampleRate = 44000;
bool mode = false;
bool mode2 = false;

HWAVEIN hWaveIn;
WAVEHDR WaveInHdr;
HWAVEOUT hWaveOut;
WAVEFORMATEX pFormat;

int counter = 0;
int lastTime = 0;

LRESULT WINAPI wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam){
	switch (msg){
	case WM_CREATE:{
		pFormat.wFormatTag = WAVE_FORMAT_PCM;
		pFormat.nChannels = 1;
		pFormat.nSamplesPerSec = sampleRate;
		pFormat.nAvgBytesPerSec = 2 * sampleRate;
		pFormat.nBlockAlign = 2;
		pFormat.wBitsPerSample = 16;
		pFormat.cbSize = 0;
		MMRESULT result = waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat, 0, 0, WAVE_FORMAT_DIRECT);
		if (result){
			char fault[256];
			waveInGetErrorTextA(result, fault, 256);
			MessageBoxA(NULL, fault, "Failed to open waveform input device.", MB_OK | MB_ICONEXCLAMATION);
			exit(-1);
		}
		break;
	}
	case WM_COMMAND:
		if ((HWND)lparam == button){
			if (mode)
				counter = NUMPTS / sampleRate * 10;
			else{
				char txt[100];
				GetWindowTextA(edit, txt, 100);
				int a = atoi(txt);
				NUMPTS = 44000 * a;

				if (waveIn)
					delete[]waveIn;
				waveIn = new short[NUMPTS];
				WaveInHdr.lpData = (LPSTR)waveIn;
				WaveInHdr.dwBufferLength = 2 * NUMPTS;
				WaveInHdr.dwBytesRecorded = 0;
				WaveInHdr.dwUser = 0;
				WaveInHdr.dwFlags = 0;
				WaveInHdr.dwLoops = 0;
				waveInPrepareHeader(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));

				waveInAddBuffer(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));
				waveInStart(hWaveIn);

				mode = true;
				lastTime = 0;
				EnableWindow(button2, FALSE);
				SendMessage(prgb, PBM_SETRANGE, 0, (LPARAM)MAKELONG(0, (a - 1) * 10));
				SetWindowText(button, L"stop");
				SetTimer(window, 1, 100, 0);
			}
		}
		else if ((HWND)lparam == button2){
			if (mode2)
				counter = NUMPTS / sampleRate * 10;
			else{
				waveOutOpen(&hWaveOut, WAVE_MAPPER, &pFormat, 0, 0, WAVE_FORMAT_DIRECT);
				waveOutWrite(hWaveOut, &WaveInHdr, sizeof(WaveInHdr));

				mode2 = true;
				EnableWindow(button, FALSE);
				SendMessage(prgb, PBM_SETRANGE, 0, (LPARAM)MAKELONG(0, lastTime));
				SetWindowText(button2, L"stop");
				SetTimer(window, 1, 100, 0);
			}
		}
		break;
	case WM_TIMER:
		if (counter <= (mode2 ? lastTime : NUMPTS / sampleRate * 10)){
			++counter;
			if (mode)
				++lastTime;
			SendMessage(prgb, PBM_SETPOS, (WPARAM)counter, 0);
		}
		else{
			counter = 0;
			if (mode){
				waveInStop(hWaveIn);
				EnableWindow(button2, TRUE);
				SetWindowText(button, L"start");
				mode = false;
			}
			else if (mode2){
				waveOutReset(hWaveOut);
				waveOutPause(hWaveOut);
				EnableWindow(button, TRUE);
				SetWindowText(button2, L"play");
				mode2 = false;
			}

			KillTimer(window, 1);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(window);
		DestroyWindow(button);
		DestroyWindow(button2);
		DestroyWindow(stat);
		DestroyWindow(edit);
		DestroyWindow(prgb);

		waveOutUnprepareHeader(hWaveOut, &WaveInHdr, sizeof(WAVEHDR));
		waveInUnprepareHeader(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));
		waveInClose(hWaveIn);
		waveOutClose(hWaveOut);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

INT WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, INT){
	WNDCLASS w = { 0 };
	w.lpszClassName = L"clstst";
	w.hInstance = hinst;
	w.lpfnWndProc = wndproc;
	w.hCursor = LoadCursor(0, IDC_ARROW);
	w.hbrBackground = (HBRUSH)6;
	w.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClass(&w);

	window = CreateWindow(L"clstst", L"Recorder (by Xau)", WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 200, 75, 0, 0, 0, 0);
	button = CreateWindow(L"BUTTON", L"start", WS_VISIBLE | WS_CHILD, 0, 0, 50, 20, window, 0, 0, 0);
	button2 = CreateWindow(L"BUTTON", L"play", WS_DISABLED | WS_VISIBLE | WS_CHILD, 50, 0, 50, 20, window, 0, 0, 0);
	stat = CreateWindow(L"STATIC", L"time(s):", WS_VISIBLE | WS_CHILD, 110, 3, 50, 17, window, 0, 0, 0);
	edit = CreateWindow(L"EDIT", L"10", WS_VISIBLE | WS_CHILD | WS_BORDER, 145, 0, 50, 20, window, 0, 0, 0);
	prgb = CreateWindow(PROGRESS_CLASS, 0, WS_VISIBLE | WS_CHILD | PBS_SMOOTH, 0, 20, 184, 15, window, 0, 0, 0);
	HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	SendMessage(button, WM_SETFONT, WPARAM(font), TRUE);
	SendMessage(button2, WM_SETFONT, WPARAM(font), TRUE);
	SendMessage(stat, WM_SETFONT, WPARAM(font), TRUE);
	SendMessage(edit, WM_SETFONT, WPARAM(font), TRUE);

	ShowWindow(window, TRUE);
	MSG msg;
	GetMessage(&msg, 0, 0, 0);
	while (msg.message != WM_QUIT){
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	UnregisterClass(L"clstst", hinst);
	return msg.wParam;
}