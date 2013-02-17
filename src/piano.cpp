#include "common.hpp"
#include "piano.hpp"

#define COMPILE_SHADER_FROM_FILE	0
#define WRITE_SHADER_SOURCE			0
#define WRITE_SHADER_BINARY			1

#if COMPILE_SHADER_FROM_FILE && (WRITE_SHADER_SOURCE || WRITE_SHADER_BINARY)
#include <fstream>
#else
#include "shader.hpp"
#endif

#include "sound.hpp"

static const float quad_vertices[] = {
	 1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	 1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

static IDirect3DVertexShader9* g_vertex_shader = NULL;
static IDirect3DPixelShader9* g_pixel_shader = NULL;

extern int g_keys_pressed[8];
extern D3DPRESENT_PARAMETERS g_dev_params;

void PianoInit(IDirect3DDevice9* device)
{
#if COMPILE_SHADER_FROM_FILE
	ID3DXBuffer *vs_buffer = NULL;
	ID3DXBuffer *ps_buffer = NULL;
	ID3DXBuffer *ps_error = NULL;
	SetWindowText(g_dev_params.hDeviceWindow, "PIANO - compiling shaders");
	D3DXCompileShaderFromFile("fx/vs.hlsl", NULL, NULL, "vs_main", "vs_3_0", D3DXSHADER_USE_LEGACY_D3DX9_31_DLL | D3DXSHADER_PREFER_FLOW_CONTROL, &vs_buffer, NULL, NULL);
	D3DXCompileShaderFromFile("fx/ps.hlsl", NULL, NULL, "ps_main", "ps_3_0", D3DXSHADER_USE_LEGACY_D3DX9_31_DLL | D3DXSHADER_PREFER_FLOW_CONTROL, &ps_buffer, &ps_error, NULL);

	#if WRITE_SHADER_BINARY
	{
		std::ofstream ofs("src/shader.hpp");
		if (ps_error != NULL)
		{
			ofs << (char *)ps_error->GetBufferPointer();
			ofs.close();
			ExitProcess(0);
		}

		const char* hex_digits = "0123456789ABCDEF";

		ofs << "unsigned int vs_shader[] = {";
		unsigned int *vs_codes = (unsigned int*)vs_buffer->GetBufferPointer();
		int count = 0;
		for (int i = 0; i != vs_buffer->GetBufferSize() / 4; ++i)
		{
			if ((vs_codes[i] & 0xFFFF) == D3DSIO_COMMENT)
			{
				int comment_length = vs_codes[i] >> 16;
				i += comment_length;
				continue;
			}
			if (count++ % 16 == 0) ofs << std::endl;

			ofs << "0x";
			for (int j = 28; j >= 0; j -= 4)
			{
				ofs << hex_digits[(vs_codes[i] >> j) & 0xF];
			}
			ofs << ",";
		}
		ofs << "};" << std::endl;
		ofs << "unsigned int ps_shader[] = {";
		unsigned int *ps_codes = (unsigned int*)ps_buffer->GetBufferPointer();
		count = 0;
		for (int i = 0; i != ps_buffer->GetBufferSize() / 4; ++i)
		{
			if ((ps_codes[i] & 0xFFFF) == D3DSIO_COMMENT)
			{
				int comment_length = ps_codes[i] >> 16;
				i += comment_length;
				continue;
			}
			if (count++ % 16 == 0) ofs << std::endl;

			ofs << "0x";
			for (int j = 28; j >= 0; j -= 4)
			{
				ofs << hex_digits[(ps_codes[i] >> j) & 0xF];
			}
			ofs << ",";
		}
		ofs << "};" << std::endl;
	}
	#elif  WRITE_SHADER_SOURCE
	{

	}
	#endif
		
	device->CreateVertexShader((DWORD*)vs_buffer->GetBufferPointer(), &g_vertex_shader);
	device->CreatePixelShader((DWORD*)ps_buffer->GetBufferPointer(), &g_pixel_shader);

#else
	#if WRITE_SHADER_SOURCE
	{

	}
	#else
	{
		device->CreateVertexShader((DWORD*)vs_shader, &g_vertex_shader);
		device->CreatePixelShader((DWORD*)ps_shader, &g_pixel_shader);
	}
	#endif

#endif  // COMPILE_SHADER_FROM_FILE

	InitSound();
}

void PianoUpdate(IDirect3DDevice9* device, long time)
{
	UpdateSamples();

	device->BeginScene();
	device->SetVertexShader(g_vertex_shader);
	device->SetPixelShader(g_pixel_shader);

	float key_code[8];
	for (int i = 0; i != 8; ++i) key_code[i] = (float)g_keys_pressed[i];
	device->SetPixelShaderConstantF(0, key_code, 2);

	device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad_vertices, sizeof(float) * 5);
	device->EndScene();
	device->Present(NULL, NULL, NULL, NULL);
}

void PianoDestroy(IDirect3DDevice9* device)
{
	g_vertex_shader->Release();
	g_pixel_shader->Release();
}

void PianoPlay(int note)
{
	PlaySample(note);
}
