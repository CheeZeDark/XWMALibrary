// dllmain.cpp : Defines the entry point for the DLL application.
#include "includes.h"
#include <xaudio2.h>
#include <wmsdk.h>    // IWMSyncReader
#pragma comment(lib, "wmcodecdspuuid.lib")
#define DLLIMPORT __declspec(dllexport)

DLLIMPORT void PlayXWMAFile(LPCWSTR filename, int loopcount) {

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    MFStartup(MF_VERSION);

    IMFSourceReader* reader = nullptr;
    MFCreateSourceReaderFromURL(filename, nullptr, &reader);

    IMFMediaType* pcmType = nullptr;
    MFCreateMediaType(&pcmType);
    pcmType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pcmType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pcmType);

    IMFMediaType* currentType = nullptr;
    reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &currentType);

    WAVEFORMATEX* wfx = nullptr;
    UINT32 wfxSize = 0;
    MFCreateWaveFormatExFromMFMediaType(currentType, &wfx, &wfxSize, MFWaveFormatExConvertFlag_Normal);

    std::vector<BYTE> pcmData;
    while (true) {
        DWORD streamIndex, flags;
        LONGLONG timestamp;
        IMFSample* sample = nullptr;
        reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &timestamp, &sample);
        if (!sample || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) break;

        IMFMediaBuffer* buffer = nullptr;
        sample->ConvertToContiguousBuffer(&buffer);

        BYTE* data = nullptr;
        DWORD maxLen = 0, curLen = 0;
        buffer->Lock(&data, &maxLen, &curLen);
        pcmData.insert(pcmData.end(), data, data + curLen);
        buffer->Unlock();
        buffer->Release();
        sample->Release();
    }

    IXAudio2* xaudio2 = nullptr;
    XAudio2Create(&xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

    IXAudio2MasteringVoice* master = nullptr;
    xaudio2->CreateMasteringVoice(&master);

    IXAudio2SourceVoice* source = nullptr;
    xaudio2->CreateSourceVoice(&source, wfx);

    XAUDIO2_BUFFER buf = {};
    buf.pAudioData = pcmData.data();
    buf.AudioBytes = static_cast<UINT32>(pcmData.size());
    buf.Flags = XAUDIO2_END_OF_STREAM;
    buf.LoopCount = XAUDIO2_LOOP_INFINITE;

    source->SubmitSourceBuffer(&buf);
    source->Start(0);

    /*
        // Cleanup
        source->Stop(0);
        source->DestroyVoice();
        master->DestroyVoice();
        xaudio2->Release();

        CoTaskMemFree(wfx);
        currentType->Release();
        pcmType->Release();
        reader->Release();

        MFShutdown();
        CoUninitialize();
    */

}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

