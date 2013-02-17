#include "common.hpp"
#include "sound.hpp"

#include "math.hpp"

extern D3DPRESENT_PARAMETERS g_dev_params;

static const int g_num_octaves = 5;
static const int g_note_offset = 12 + 3;
static const int g_sample_rate = 44100;
static const int g_total_samples = g_sample_rate * 2;
static short g_samples[g_num_octaves * 12][g_total_samples];

static const WAVEFORMATEX g_format = {WAVE_FORMAT_PCM, 1, g_sample_rate, g_sample_rate * sizeof(short), sizeof(short), sizeof(short) * 8};       
static const DSBUFFERDESC g_primary_buffer_desc = {sizeof(DSBUFFERDESC), DSBCAPS_PRIMARYBUFFER};
static const DSBUFFERDESC g_channel_buffer_desc = {sizeof(DSBUFFERDESC), DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS, g_total_samples * sizeof(short), NULL, (LPWAVEFORMATEX)&g_format, NULL};
LPDIRECTSOUND g_dsound;
LPDIRECTSOUNDBUFFER g_primary_buffer;

static const int g_num_channels = 16;
static LPDIRECTSOUNDBUFFER g_channels[g_num_channels];
static int g_cur_channel_idx = 0;

static FILETIME g_last_modify_time = {0};
static SHELLEXECUTEINFO g_exec_info;

void InitSound()
{
	DirectSoundCreate(0, &g_dsound, 0);
	g_dsound->SetCooperativeLevel(g_dev_params.hDeviceWindow, DSSCL_PRIORITY);
	g_dsound->CreateSoundBuffer(&g_primary_buffer_desc, &g_primary_buffer, NULL);
	g_primary_buffer->SetFormat(&g_format);

	for (int i = 0; i != g_num_channels; ++i)
	{
		g_dsound->CreateSoundBuffer(&g_channel_buffer_desc, &g_channels[i], NULL);
	}

	UpdateSamples();
}

void UpdateSamples()
{
	HANDLE h_file = CreateFile("syn.cpp", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_file == INVALID_HANDLE_VALUE)
	{
		SetWindowText(g_dev_params.hDeviceWindow, "PIANO - can't find syn.cpp");
		return;
	}

	FILETIME write_time = {0};
	GetFileTime(h_file, NULL, NULL, &write_time);
	CloseHandle(h_file);
	if (g_last_modify_time.dwHighDateTime == write_time.dwHighDateTime &&
		g_last_modify_time.dwLowDateTime == write_time.dwLowDateTime) {
		return;
	}
	g_last_modify_time = write_time;

	SetWindowText(g_dev_params.hDeviceWindow, "PIANO - compiling...");
	g_exec_info.cbSize = sizeof(SHELLEXECUTEINFO);
	g_exec_info.fMask = SEE_MASK_NOCLOSEPROCESS;
	g_exec_info.lpFile = "cmd";
	g_exec_info.lpParameters = "/c call \"%VS90COMNTOOLS%vsvars32.bat\" & cd gen & cl gen.cpp /LD";
	g_exec_info.nShow = SW_HIDE;
	ShellExecuteEx(&g_exec_info);
	WaitForSingleObject(g_exec_info.hProcess, INFINITE);
	DWORD ret_code = 0;
	GetExitCodeProcess(g_exec_info.hProcess, &ret_code);
	if (ret_code != 0)
	{
		SetWindowText(g_dev_params.hDeviceWindow, "PIANO - compile gen.cpp failed");
		return;
	}

	HMODULE h_dll = LoadLibrary("gen/gen.dll");
	if (h_dll == NULL)
	{  
		SetWindowText(g_dev_params.hDeviceWindow, "PIANO - load gen.dll failed");
		return;
	}

	typedef float (__cdecl *gen_func)(float, float);
	gen_func func = (gen_func)GetProcAddress(h_dll, "gen");
	if (func == NULL)
	{
		SetWindowText(g_dev_params.hDeviceWindow, "PIANO - load gen.dll failed");
		FreeLibrary(h_dll);
		return;
	}

	SetWindowText(g_dev_params.hDeviceWindow, "PIANO - generating...");

	for (int i = 0; i != g_num_octaves * 12; ++i)
	{
		int note = i + g_note_offset;
		const float A2_frequency = 27.5f;
		float freq = A2_frequency * pow(2, note / 12.0f);

		for (int j = 0; j != g_total_samples; ++j)
		{
			float time = (float)j / g_sample_rate;
			float out = func(freq * 2 * PI, time);

			out = min(out, 1.0f);
			out = max(out, -1.0f);
			g_samples[i][j] = (short)(out * 0x7FFF);
		}
	}

	SetWindowText(g_dev_params.hDeviceWindow, "PIANO - ready");

	FreeLibrary(h_dll);
}

void PlaySample(int i)
{
	short* data = NULL;
	unsigned long length = 0;
	g_channels[g_cur_channel_idx]->Lock(0, g_total_samples * sizeof(short), (void**)&data, &length, NULL, NULL, 0);

#ifndef RELEASE
	memcpy(data, &g_samples[i], g_total_samples * sizeof(short));
#else
	for (int j = 0; j != g_total_samples; ++j) data[j] = g_samples[i][j];
#endif

	g_channels[g_cur_channel_idx]->Unlock(data, length, NULL, 0);

	g_channels[g_cur_channel_idx]->SetCurrentPosition(0);
	g_channels[g_cur_channel_idx]->Play(0, 0, 0);

	g_cur_channel_idx = (g_cur_channel_idx + 1) % g_num_channels;
}
