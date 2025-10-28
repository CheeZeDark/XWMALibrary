#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0602  // Windows 8
#define WINVER 0x0602
#include <windows.h>
#include <mfapi.h>
#include <mfobjects.h>
#include <mfreadwrite.h>
#include <xaudio2.h>
#include <vector>
#include <iostream>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "xaudio2.lib")
 /*
 XAUDIO2_BUFFER buf = {};
IXAudio2SourceVoice* source = nullptr;
IXAudio2MasteringVoice* master = nullptr;
IXAudio2* xaudio2 = nullptr;
IMFMediaBuffer* buffer = nullptr;
 */