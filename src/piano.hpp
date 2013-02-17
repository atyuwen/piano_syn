#ifndef _PIANO_HPP_INCLUDED_
#define _PIANO_HPP_INCLUDED_

void PianoInit(IDirect3DDevice9* device);

void PianoUpdate(IDirect3DDevice9* device, long time);

void PianoDestroy(IDirect3DDevice9* device);

void PianoPlay(int note);

#endif  // _PIANO_HPP_INCLUDED_