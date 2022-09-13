#pragma once

#if defined(__amd64__)
extern "C" void _amd64_fxsave(char *Buffer);
extern "C" void _amd64_fxrstor(char *Buffer);
void _fxsave(char *Buffer) { _amd64_fxsave(Buffer); }
void _fxrstor(char *Buffer) { _amd64_fxrstor(Buffer); }
#elif defined(__i386__)
extern "C" void _i686_fxsave(char *Buffer);
extern "C" void _i686_fxrstor(char *Buffer);
void _fxsave(char *Buffer) { _i686_fxsave(Buffer); }
void _fxrstor(char *Buffer) { _i686_fxrstor(Buffer); }
#endif
