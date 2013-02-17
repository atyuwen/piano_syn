#include "common.hpp"

#ifdef RELEASE

extern "C" int __CRTDECL _purecall(void)
{
	return 0;
}

void* __CRTDECL operator new (unsigned int count)
{
	return (void*)GlobalAlloc(GMEM_FIXED, count);
}

void __CRTDECL operator delete (void* ptr)
{
	GlobalFree(ptr);
}

#endif