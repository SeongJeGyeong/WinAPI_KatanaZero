#include "pch.h"
#include "SoundManager.h"
#include "../Resources/AudioSource.h"

void SoundManager::Init(fs::path directory)
{
	// COM 초기화
	HRESULT hr;
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		std::cerr << "COM 초기화 실패" << std::endl;
		return;
	}

	// XAudio2 인스턴스 생성
	hr = XAudio2Create(&_xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		std::cerr << "XAudio2 생성 실패" << std::endl;
		return;
	}

	// 마스터링 보이스 생성
	hr = _xaudio2->CreateMasteringVoice(&_masterVoice);
	if (FAILED(hr)) {
		std::cerr << "마스터링 보이스 생성 실패" << std::endl;
		return;
	}

    fs::path path = directory / L"Json/Audios.json";
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		MessageBox(nullptr, L"Failed to open JSON file", L"Error", MB_OK);
		return;
	}

	json data = json::parse(file);

	fs::path bgmPath = directory / L"Audios/BGM/";
	for (auto& item : data["BGM"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = bgmPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			if (fileName == "song_rainonbrick")
				_bgmMap[EBGMType::MAIN_MENU] = audio;
			else if (fileName == "song_katanazero")
				_bgmMap[EBGMType::STAGE] = audio;
			else if (fileName == "song_bossbattle")
				_bgmMap[EBGMType::BOSS_ROOM] = audio;
		}
	}

	fs::path sfxPath = directory / L"Audios/SFX/";
	for (auto& item : data["SFX"]["Main"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_sfxMap[fileName] = audio;
		}
	}

	int32 idx = 0;
	for (auto& item : data["SFX"]["PlayerRunning"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_playerRunningSfx[idx++] = audio;
		}
	}
	idx = 0;
	for (auto& item : data["SFX"]["PlayerSlash"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_playerSlashSfx[idx++] = audio;
		}
	}
	idx = 0;
	for (auto& item : data["SFX"]["EnemyWalk"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_enemyWalkSfx[idx++] = audio;
		}
	}
	idx = 0;
	for (auto& item : data["SFX"]["EnemyRun"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_enemyRunSfx[idx++] = audio;
		}
	}
	idx = 0;
	for (auto& item : data["SFX"]["BloodSquirt"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_bloodSfx[idx++] = audio;
		}
	}

	_slowMotionSfx.voice = nullptr;
	_slowMotionSfx.inUse = false;
}

void SoundManager::Update(float deltaTime)
{
	// 부드러운 속도 전환 처리
	if (_globalSpeed != _targetSpeed) {
		float speedDiff = _targetSpeed - _globalSpeed;
		float speedChange = speedDiff * _speedTransitionRate * deltaTime;

		if (std::abs(speedDiff) < 0.01f) {
			_globalSpeed = _targetSpeed;
		}
		else {
			_globalSpeed += speedChange;
		}

		// 현재 BGM 속도 업데이트
		if (_currentBGM) {
			_currentBGM->SetFrequencyRatio(_bgmBaseSpeed * _globalSpeed);
		}

		// 재생 중인 모든 SFX 속도 업데이트
		for (auto& sfx : _sfxPool) {
			if (sfx.inUse && sfx.voice) {
				if (sfx.type == ESFXType::SFX_STRUGGLE)
				{
					int i = 1;
				}
				XAUDIO2_VOICE_STATE state;
				sfx.voice->GetState(&state);
				if (state.BuffersQueued > 0) {
					sfx.voice->SetFrequencyRatio(_globalSpeed);
				}
			}
		}
	}

	// 사용이 끝난 SFX 보이스 정리
	auto now = std::chrono::steady_clock::now();
	for (auto it = _sfxPool.begin(); it != _sfxPool.end();)
	{
		if (it->inUse && it->voice && !it->isLooping)
		{
			XAUDIO2_VOICE_STATE state;
			it->voice->GetState(&state);
			if (state.BuffersQueued == 0) 
			{
				// 재생이 끝났으므로 정리
				it->voice->Stop();
				it->voice->FlushSourceBuffers();
				it->inUse = false;
				it->isLooping = false;
				it = _sfxPool.erase(it);
				continue;
			}
			else 
			{
				// 타임아웃 체크 (10초)
				auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->startTime).count();
				if (elapsed > 10) {
					it->voice->Stop();
					it->voice->FlushSourceBuffers();
					it->inUse = false;
					it->isLooping = false;
					it = _sfxPool.erase(it);
					continue;
				}
			}
		}
		++it;
	}
}

void SoundManager::PlayBGM(EBGMType bgmType, bool loop, float volume, float speed)
{
	auto it = _bgmMap.find(bgmType);
	if (it == _bgmMap.end()) return;

	if (_currentBGM)
	{
		// 현재 BGM과 같은 타입이면 무시
		if (_currentBGM && _currentBGMType == bgmType) {
			return;
		}

		_currentBGM->Stop();
		_currentBGM->DestroyVoice();
		_currentBGM = nullptr;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	// 새 소스 보이스 생성
	HRESULT hr = _xaudio2->CreateSourceVoice(&_currentBGM, audio->_wfx);
	if (FAILED(hr)) {
		OutputDebugString(L"Failed to create source voice for BGM\n");
		return;
	}

	// 버퍼 설정
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = audio->_bufferSize;
	buffer.pAudioData = audio->_buffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (loop) {
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	// 볼륨과 속도 설정
	_currentBGM->SetVolume(volume * _bgmVolume * _masterVolume);
	_currentBGM->SetFrequencyRatio(speed * _globalSpeed);

	// 버퍼 제출 및 재생
	hr = _currentBGM->SubmitSourceBuffer(&buffer);
	if (SUCCEEDED(hr)) {
		_currentBGM->Start(0);
		_currentBGMType = bgmType;
		OutputDebugString(L"BGM started playing\n");
	}
}

void SoundManager::StopBGM()
{
	if (_currentBGM) {
		_currentBGM->Stop();
		_currentBGM->FlushSourceBuffers();
		_currentBGM->DestroyVoice();
		_currentBGM = nullptr;
		OutputDebugString(L"BGM stopped\n");
	}
}

void SoundManager::PauseBGM()
{
	if (_currentBGM) {
		_currentBGM->Stop(XAUDIO2_PLAY_TAILS);
	}
}

void SoundManager::ResumeBGM()
{
	if (_currentBGM) {
		_currentBGM->Start();
	}
}

void SoundManager::PlaySFX(string sfxName, bool loop, float volume, float speed, ESFXType type)
{
	auto it = _sfxMap.find(sfxName);
	if (it == _sfxMap.end()) {
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	SetSFX(audio, volume, speed, loop, type);
}

void SoundManager::StopAllSFX()
{
	for (auto& sfx : _sfxPool) {
		if (sfx.voice) {
			sfx.voice->Stop();
			sfx.voice->FlushSourceBuffers();
			sfx.voice->DestroyVoice();
			sfx.voice = nullptr;
			sfx.inUse = false;
		}
	}
}

void SoundManager::SetGlobalSpeed(float speed, bool immediate)
{
	speed = std::clamp(speed, 0.1f, 2.0f); // 0.1x ~ 2x 속도 제한

	if (immediate) {
		_globalSpeed = speed;
		_targetSpeed = speed;

		// BGM 즉시 속도 변경
		if (_currentBGM) {
			_currentBGM->SetFrequencyRatio(_bgmBaseSpeed * _globalSpeed);
		}

		// 모든 재생 중인 SFX 즉시 속도 변경
		for (auto& sfx : _sfxPool) {
			if (sfx.inUse && sfx.voice) {
				XAUDIO2_VOICE_STATE state;
				sfx.voice->GetState(&state);
				if (state.BuffersQueued > 0) {
					sfx.voice->SetFrequencyRatio(_globalSpeed);
				}
			}
		}
	}
	else {
		// 부드러운 전환을 위해 목표 속도만 설정
		_targetSpeed = speed;
	}
}

void SoundManager::SetBGMSpeed(float speed)
{
	_bgmBaseSpeed = std::clamp(speed, 0.1f, 2.0f);

	if (_currentBGM) {
		_currentBGM->SetFrequencyRatio(_bgmBaseSpeed * _globalSpeed);
	}
}

void SoundManager::SetMasterVolume(float volume)
{
	_masterVolume = std::clamp(volume, 0.0f, 1.0f);

	// 현재 재생 중인 BGM 볼륨 업데이트
	if (_currentBGM) {
		_currentBGM->SetVolume(_bgmVolume * _masterVolume);
	}
}

void SoundManager::SetBGMVolume(float volume)
{
	_bgmVolume = std::clamp(volume, 0.0f, 1.0f);

	if (_currentBGM) {
		_currentBGM->SetVolume(_bgmVolume * _masterVolume);
	}
}

void SoundManager::SetSFXVolume(float volume)
{
	_sfxVolume = std::clamp(volume, 0.0f, 1.0f);
}

void SoundManager::PlayPlayerSlash()
{
	auto it = _playerSlashSfx.find(rand() % 3);
	if (it == _playerSlashSfx.end()) 
	{
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	SetSFX(audio, 1.f, 1.f, false);
}

void SoundManager::PlayPlayerRun()
{
	auto it = _playerRunningSfx.find(rand() % 4);
	if (it == _playerRunningSfx.end())
	{
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	SetSFX(audio, 1.f, 1.f, false);
}

void SoundManager::PlayEnemyWalk()
{
	auto it = _enemyWalkSfx.find(rand() % 4);
	if (it == _enemyWalkSfx.end())
	{
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	SetSFX(audio, 0.5f, 1.f, false);
}

void SoundManager::PlayEnemyRun()
{
	auto it = _enemyRunSfx.find(rand() % 4);
	if (it == _enemyRunSfx.end())
	{
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	SetSFX(audio, 1.f, 1.f, false);
}

void SoundManager::PlayEnemySlashKill()
{
	auto it = _sfxMap.find((rand() % 2) ? "sound_enemy_death_sword_01" : "sound_enemy_death_sword_02");
	if (it == _sfxMap.end()) {
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	SetSFX(audio, 1.f, 1.f, false);
}

void SoundManager::PlayBloodSquirt()
{

}

void SoundManager::PlaySlowMotion(bool engage)
{
	auto it = _sfxMap.find(engage ? "sound_slomo_engage" : "sound_slomo_disengage");
	if (it == _sfxMap.end()) {
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	if (_slowMotionSfx.voice)
	{
		_slowMotionSfx.voice->Stop();
		_slowMotionSfx.voice->FlushSourceBuffers();
		_slowMotionSfx.voice->DestroyVoice();
		_slowMotionSfx.voice = nullptr;
		_slowMotionSfx.inUse = false;
	}

	HRESULT hr = _xaudio2->CreateSourceVoice(&_slowMotionSfx.voice, audio->_wfx);
	if (FAILED(hr)) {
		OutputDebugString(L"Failed to create source voice for sfx\n");
		return;
	}

	IXAudio2SourceVoice* voice = voice = _slowMotionSfx.voice;
	_slowMotionSfx.inUse = true;
	_slowMotionSfx.startTime = std::chrono::steady_clock::now();

	if (!voice) {
		OutputDebugString(L"No available SFX voice\n");
		return;
	}

	// 버퍼 설정
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = audio->_bufferSize;
	buffer.pAudioData = audio->_buffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	// 볼륨과 속도 설정
	voice->SetVolume(0.5f * _sfxVolume * _masterVolume);
	voice->SetFrequencyRatio(1.f);

	voice->SubmitSourceBuffer(&buffer);
	voice->Start(0);
}

void SoundManager::SetSFX(AudioSource* audio, float volume, float speed, bool loop, ESFXType type)
{
	// 사용 가능한 보이스 찾기
	IXAudio2SourceVoice* voice = nullptr;
	SFXVoice sfx;
	HRESULT hr = _xaudio2->CreateSourceVoice(&sfx.voice, audio->_wfx);
	if (FAILED(hr)) return;

	voice = sfx.voice;
	sfx.inUse = true;
	sfx.isLooping = loop;
	sfx.type = type;
	sfx.startTime = std::chrono::steady_clock::now();

	_sfxPool.push_back(sfx);

	//if (!voice) {
	//	// 새 보이스 생성
	//	for (auto& sfx : _sfxPool) {
	//		if (!sfx.voice) {
	//			HRESULT hr = _xaudio2->CreateSourceVoice(&sfx.voice, audio->_wfx);
	//			if (SUCCEEDED(hr)) {
	//				voice = sfx.voice;
	//				sfx.inUse = true;
	//				sfx.isLooping = loop;
	//				sfx.type = type;
	//				sfx.startTime = std::chrono::steady_clock::now();
	//				break;
	//			}
	//		}
	//	}
	//}

	if (!voice) {
		OutputDebugString(L"No available SFX voice\n");
		return;
	}

	// 버퍼 설정
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = audio->_bufferSize;
	buffer.pAudioData = audio->_buffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (loop) buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	// 볼륨과 속도 설정
	voice->SetVolume(volume * _sfxVolume * _masterVolume);
	voice->SetFrequencyRatio(speed * _globalSpeed);

	// 기존 버퍼 정리 후 새 버퍼 제출
	voice->Stop();
	voice->FlushSourceBuffers();
	voice->SubmitSourceBuffer(&buffer);
	voice->Start(0);
}

void SoundManager::StopSFX(ESFXType type)
{
	if (_sfxPool.empty()) return;

	for (auto& sfx : _sfxPool) {
		if (sfx.type == type && sfx.inUse && sfx.voice) {
			sfx.voice->Stop();
			sfx.voice->FlushSourceBuffers();
			sfx.inUse = false;
			sfx.isLooping = false;
			break;
		}
	}
}

IXAudio2SourceVoice* SoundManager::GetAvailableSFXVoice()
{
	for (auto& sfx : _sfxPool) {
		if (sfx.voice && !sfx.inUse) {
			sfx.inUse = true;
			sfx.startTime = std::chrono::steady_clock::now();
			return sfx.voice;
		}
	}
	return nullptr;
}

void SoundManager::ReleaseSFXVoice(IXAudio2SourceVoice* voice)
{
}
