#pragma once

// WAV 파일 헤더 구조체
struct WAVHeader {
    char riff[4];           // "RIFF"
    uint32_t size;          // 파일 크기 - 8
    char wave[4];           // "WAVE"
};

struct WAVChunk {
    char id[4];             // 청크 ID
    uint32_t size;          // 청크 크기
};

struct WAVFormat {
    uint16_t audioFormat;   // 오디오 포맷 (1 = PCM)
    uint16_t numChannels;   // 채널 수
    uint32_t sampleRate;    // 샘플링 레이트
    uint32_t byteRate;      // 바이트 레이트
    uint16_t blockAlign;    // 블록 정렬
    uint16_t bitsPerSample; // 비트 깊이
};

class AudioSource
{
public:
    WAVEFORMATEX* _wfx = nullptr;
    uint8_t* _buffer = nullptr;
    uint32_t  _bufferSize = 0;
    XAUDIO2_BUFFER _xaBuffer = {0};

    float _duration = 0.f;
    bool _isLoaded = false;

public:
    bool LoadWavFile(const fs::path& filepath);
    void Cleanup();

    float GetDuration() const { return _duration; }
    bool IsLoaded() const { return _isLoaded; }

    //void PlayAudio(class IXAudio2& xaudio2, float volume, float speed, bool loop);
};

