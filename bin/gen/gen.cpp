#include <cmath>

extern "C" __declspec(dllexport) float __cdecl gen(float w, float t)
{
	#include "../syn.cpp"
}
