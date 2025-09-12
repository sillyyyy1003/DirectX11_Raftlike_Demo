#include "AudioManager.h"
#include <fstream>
#include <cassert>
#include <cstring>
#include "UIBar.h"

#pragma pack(push, 1)


#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif
#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif


//=============================================================================
// ユーティリティ関数群
//=============================================================================
HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;
    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());
        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());
        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;
        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                return HRESULT_FROM_WIN32(GetLastError());
        }
        dwOffset += sizeof(DWORD) * 2;
        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }
        dwOffset += dwChunkDataSize;
        if (bytesRead >= dwRIFFDataSize) return S_FALSE;
    }
    return S_OK;
}

HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}

struct RIFF_HEADER {
    char riff[4];
    DWORD size;
    char wave[4];
};

struct CHUNK_HEADER {
    char id[4];
    DWORD size;
};

#pragma pack(pop)

bool AudioManager::Init()
{
	// Create XAudio2 engine
    HRESULT hr = XAudio2Create(&m_xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) return false;

	// Create a mastering voice
    hr = m_xaudio2->CreateMasteringVoice(&m_masterVoice);
    return SUCCEEDED(hr);
}

void AudioManager::UnInit()
{
    // stop all sound
    for (auto& [name, instance] : m_soundInstances)
    {
        if (instance.sourceVoice)
        {
            instance.sourceVoice->Stop(0);
            instance.sourceVoice->FlushSourceBuffers();
            instance.sourceVoice->DestroyVoice();
            instance.sourceVoice = nullptr;
        }
    }
    m_soundInstances.clear();
    m_audioDatas.clear();

	// Destroy mastering voice and XAudio2 engine
    if (m_masterVoice)
    {
        m_masterVoice->DestroyVoice();
        m_masterVoice = nullptr;
    }
    m_xaudio2.Reset();
}

bool AudioManager::LoadAudio(const char* name, const wchar_t* filePath, AudioType type)
{
    AudioData data;
    if (!LoadWaveFile(filePath, data.buffer, data.format)) return false;

	data.type = type;
    m_audioDatas[name] = data;

    // Create source view
    IXAudio2SourceVoice* sourceVoice = nullptr;
    if (FAILED(m_xaudio2->CreateSourceVoice(&sourceVoice, &data.format)))
    {
        return false;
    }

    m_soundInstances[name] = { sourceVoice, type }; 
    return true;
}

void AudioManager::Play(const char* name, bool loop)
{
    auto it = m_soundInstances.find(name);
    if (it == m_soundInstances.end()) return;

    IXAudio2SourceVoice* sourceVoice = it->second.sourceVoice;
    if (!sourceVoice) return;

    sourceVoice->Stop();                // 停止播放，重置状态
    sourceVoice->FlushSourceBuffers();  // 清空缓存

    // 重新提交 buffer
    auto& data = m_audioDatas[name];
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = static_cast<UINT32>(data.buffer.size());
    buffer.pAudioData = data.buffer.data();
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
    sourceVoice->SubmitSourceBuffer(&buffer);

    // 设置音量
    float vol = (data.type == AudioType::SE ? (m_seMute ? 0.0f : m_seVolume) : (m_bgmMute ? 0.0f : m_bgmVolume));
    sourceVoice->SetVolume(vol);

    sourceVoice->Start();

}

void AudioManager::Stop(const char* name)
{
    auto it = m_soundInstances.find(name);
    if (it != m_soundInstances.end() && it->second.sourceVoice)
    {
        it->second.sourceVoice->Stop();
        it->second.sourceVoice->FlushSourceBuffers(); // Clear buffer

    }
}

void AudioManager::SetVolume(AudioType type, float volume)
{
	// Clamp volume between 0.0 and 1.0
    volume = max(0.0f, min(volume, 1.0f));
    if (type == AudioType::SE)
    {
	    m_seVolume = volume;
		if (m_seBar) m_seBar->UpdateUI(m_seVolume); // volume is already clamped between 0.0 and 1.0
    }
    else
    {
	    m_bgmVolume = volume;
		if (m_bgmBar)m_bgmBar->UpdateUI(m_bgmVolume); // volume is already clamped between 0.0 and 1.0
    }
    UpdateVolume(type);
}

void AudioManager::SetMute(AudioType type, bool mute)
{
    if (type == AudioType::SE) m_seMute = mute;
    else m_bgmMute = mute;
    UpdateVolume(type);
}

void AudioManager::OnSEFinished(const char* name)
{
    auto it = m_soundInstances.find(name);
    if (it != m_soundInstances.end())
    {
        if (it->second.sourceVoice)
            it->second.sourceVoice->DestroyVoice();

        m_soundInstances.erase(it);
    }
}

void AudioManager::StopAllSound()
{
    for (auto& [name, instance] : m_soundInstances)
    {
        if (instance.sourceVoice)
        {
            instance.sourceVoice->Stop(0);
            instance.sourceVoice->FlushSourceBuffers();
        }
    }
}

void AudioManager::StopBgms()
{
    for (auto& [name, instance] : m_soundInstances)
    {
        if (instance.sourceVoice && instance.type == AudioType::BGM)
        {
            instance.sourceVoice->Stop(0);
            instance.sourceVoice->FlushSourceBuffers();
        }
    }
}

void AudioManager::SetUI(UIBar* bgmBar, UIBar* seBar)
{
	m_seBar = seBar;
	m_bgmBar = bgmBar;
	if (m_bgmBar) m_bgmBar->UpdateUI(m_bgmVolume);
	if (m_seBar) m_seBar->UpdateUI(m_seVolume);

}

bool AudioManager::LoadWaveFile(const wchar_t* filepath, std::vector<BYTE>& outBuffer, WAVEFORMATEX& outFormat)
{
    HANDLE hFile = CreateFileW(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwChunkSize = 0;
    DWORD dwChunkPosition = 0;
    DWORD filetype = 0;

    // 查找RIFF块，并确认是WAVE类型
    if (FAILED(FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition)))
    {
        CloseHandle(hFile);
        return false;
    }
    if (FAILED(ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition)))
    {
        CloseHandle(hFile);
        return false;
    }
    if (filetype != fourccWAVE)
    {
        CloseHandle(hFile);
        return false;
    }

    // 查找fmt块
    if (FAILED(FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition)))
    {
        CloseHandle(hFile);
        return false;
    }
    std::vector<BYTE> fmtBuf(dwChunkSize);
    if (FAILED(ReadChunkData(hFile, fmtBuf.data(), dwChunkSize, dwChunkPosition)))
    {
        CloseHandle(hFile);
        return false;
    }
    // 兼容WAVEFORMATEXTENSIBLE和WAVEFORMATEX
    WAVEFORMATEX* wf = reinterpret_cast<WAVEFORMATEX*>(fmtBuf.data());
    memcpy(&outFormat, wf, min(sizeof(WAVEFORMATEX), dwChunkSize));

    // 查找data块并读取音频数据
    if (FAILED(FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition)))
    {
        CloseHandle(hFile);
        return false;
    }
    outBuffer.resize(dwChunkSize);
    if (FAILED(ReadChunkData(hFile, outBuffer.data(), dwChunkSize, dwChunkPosition)))
    {
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);
    return true;
}

void AudioManager::UpdateVolume(AudioType type)
{
    for (auto& [name, instance] : m_soundInstances)
    {
        if (instance.type == type && instance.sourceVoice)
        {
            float vol = (type == AudioType::SE ? (m_seMute ? 0.0f : m_seVolume) : (m_bgmMute ? 0.0f : m_bgmVolume));
            instance.sourceVoice->SetVolume(vol);
        }
    }
}
