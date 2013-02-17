#ifndef _COMMON_HPP_INCLUDED_
#define _COMMON_HPP_INCLUDED_

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>

class uncopyable
{
protected:
	uncopyable() {}
	~uncopyable() {}
private:
	uncopyable(const uncopyable&);
	uncopyable& operator = (const uncopyable&);
};

#ifdef ENABLE_LOG
	#include <cstdio>
	#define logf(format, ...) fprintf(stdout, format, __VA_ARGS__)
#else
	#define logf(format, ...)
#endif

#ifdef RELEASE
void* __CRTDECL operator new (unsigned int count);
void __CRTDECL operator delete (void* ptr);
#endif

#endif  // _COMMON_HPP_INCLUDED_