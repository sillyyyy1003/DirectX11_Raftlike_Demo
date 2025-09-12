#pragma once
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <xaudio2.h>
#include <wrl/client.h>

class UIBar;

/// @brief This is for managing audio resources and playback in a DirectX11 application.
class AudioManager
{
public:

	enum class AudioType : uint8_t
	{
		BGM,
		SE
	};

	static AudioManager& Instance()
	{
		static AudioManager instance;
		return instance;
	}

	/// <summary>
	/// 初期化
	/// </summary>
	bool Init();

	void UnInit();

	/// @brief Load audio from file
	/// @param name audio name
	/// @param filePath audio file path
	/// @param type audio type:bgm/se/ui
	/// @return if loaded 
	bool LoadAudio(const char* name, const wchar_t* filePath, AudioType type);

	/// @brief 再生
	/// @param name audio name 
	/// @param loop true>> Loop再生
	void Play(const char* name, bool loop = false);

	// 停止播放
	void Stop(const char* name);

	/// Set volume
	void SetVolume(AudioType type, float volume);

	// Set Mute/not
	void SetMute(AudioType type, bool mute);

	/// @brief Clear se when finished
	/// @param name se name
	void OnSEFinished(const char* name);

	/// @brief 全ての音声を停止させる
	void StopAllSound();

	/// @brief 全てのBGMを停止させる
	void StopBgms();

	/// @brief Set UI bars
	/// @param bgmBar 
	/// @param seBar 
	void SetUI(UIBar* bgmBar, UIBar* seBar);

private:
	struct AudioData
	{
		std::vector<BYTE> buffer;
		WAVEFORMATEX format;
		AudioType type;
	};

	struct SoundInstance
	{
		IXAudio2SourceVoice* sourceVoice = nullptr;
		AudioType type;
	};

	Microsoft::WRL::ComPtr<IXAudio2> m_xaudio2;
	IXAudio2MasteringVoice* m_masterVoice = nullptr;

	std::map<std::string, AudioData> m_audioDatas;
	std::map<std::string, SoundInstance> m_soundInstances;
	float m_seVolume = 1.0f;
	float m_bgmVolume = 1.0f;
	bool m_seMute = false;
	bool m_bgmMute = false;

	UIBar* m_bgmBar = nullptr;
	UIBar* m_seBar=nullptr;

	/// @brief Load Wav file
	/// @param filepath file path
	/// @param outBuffer 
	/// @param outFormat
	/// @return 
	bool LoadWaveFile(const wchar_t* filepath, std::vector<BYTE>& outBuffer, WAVEFORMATEX& outFormat);

	/// @brief Update volume for all playing sounds of the specified type
	void UpdateVolume(AudioType type);


};

