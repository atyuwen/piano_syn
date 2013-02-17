#include "common.hpp"
#include "piano.hpp"

const int g_width  = 1280;
const int g_height = 256;

D3DPRESENT_PARAMETERS g_dev_params = {
	g_width, g_height, D3DFMT_A8R8G8B8, 0, D3DMULTISAMPLE_NONE, 0, D3DSWAPEFFECT_DISCARD, NULL, true, true, D3DFMT_D24S8, 0, 0, D3DPRESENT_INTERVAL_IMMEDIATE,
};

int g_keys_pressed[8] = {0};
int g_key_idx = 0;

static void OnKey(int scan_code, bool up)
{
	int key = -1;

	// ZXCVBNM<>?
	if (0x2c <= scan_code && scan_code <= 0x35) key = scan_code - 0x2c + 0 * 7;
	
	// ASDFGHJKL:
	if (0x1e <= scan_code && scan_code <= 0x27) key = scan_code - 0x1e + 1 * 7;

	// QWERTYUIOP
	if (0x10 <= scan_code && scan_code <= 0x19) key = scan_code - 0x10 + 2 * 7;

	// 1234567890
	if (0x02 <= scan_code && scan_code <= 0x0b) key = scan_code - 0x02 + 3 * 7;

	// Arrow keys and Num Pad
	const unsigned short reverse_map[] = {
		0x014b, 0x0150, 0x014d, 0x0148,  // left down right up
		0x52, 0x53, 0x011c, 0x4f, 0x50, 0x51, 0x4b, 0x4c, 0x4d, 0x47, 0x48, 0x49,  // 0 . Enter 123456789
		0x4e, 0x0145, 0x0135, 0x37, 0x4a,  // + NumLock / * -
	};
	for (int i = 0; i != sizeof(reverse_map) / sizeof(unsigned short); ++i)
	{
		if (reverse_map[i] == scan_code) key = i + 2 * 7;
	}

	if (key != -1)
	{
		int q = key / 7, r = key % 7;
		int key_code = 2 * key + 100;
		if (!up)
		{
			int note = q * 12 + (r > 2 ? 2 * r - 1 : 2 * r);
			if ((GetKeyState(VK_CONTROL) & 0x80) != 0 && r != 2 && r != 6)
			{
				note += 1;
				key_code += 1;
			}
			PianoPlay(note);
			g_keys_pressed[g_key_idx] = key_code;
			g_key_idx = (g_key_idx + 1) % 8;
		}
		else
		{
			for (int i = 0; i != 8; ++i)
			{
				if (g_keys_pressed[i] == key_code) g_keys_pressed[i] = 0;
				if (g_keys_pressed[i] == key_code + 1) g_keys_pressed[i] = 0;
			}
		}
	}
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	bool up = false;
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		up = true;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (up || (lparam & (1 << 30)) == 0)
		{
			int scan_code = (lparam & (0x1FF << 16)) >> 16;
			OnKey(scan_code, up);
		}
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

static HWND InitWindow()
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "piano_wnd";
	RegisterClass(&wc);

	RECT rect = {0, 0, g_width, g_height};
	DWORD wnd_style = WS_OVERLAPPEDWINDOW & (~(WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX));
	AdjustWindowRect(&rect, wnd_style, FALSE);

	return CreateWindowEx(WS_EX_TOPMOST, "piano_wnd", "PIANO", wnd_style | WS_VISIBLE,
		(GetSystemMetrics(SM_CXSCREEN) - rect.right + rect.left) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - rect.bottom + rect.top) / 2,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, GetModuleHandle(NULL), NULL);
}

int EntryPoint()
{
	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
		return 0;
	}

	g_dev_params.hDeviceWindow = InitWindow();
	IDirect3DDevice9* device = NULL;
	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_dev_params.hDeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_dev_params, &device) != S_OK)
	{
		return 0;
	}

	PianoInit(device);

	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		PianoUpdate(device, GetTickCount());
		Sleep(10);
	}

#ifndef RELEASE
	PianoDestroy(device);
	device->Release();
	d3d->Release();
	DestroyWindow(g_dev_params.hDeviceWindow);
	return 0;
#else
	ExitProcess(0);
#endif
}

#ifndef RELEASE
int WINAPI WinMain(HINSTANCE _0, HINSTANCE _1, LPSTR _2, int _3)
{
#ifdef ENABLE_LOG
	AllocConsole();
	SetConsoleTitle("PIANO - console");

	#pragma warning(push)
	#pragma warning(disable: 4996)
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONIN$", "r+t", stdin);
	#pragma warning(pop)
#endif

	return EntryPoint();
}
#endif
