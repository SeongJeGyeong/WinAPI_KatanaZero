#include "pch.h"
#include "AudioSource.h"

bool AudioSource::LoadWavFile(const fs::path& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        OutputDebugStringW((L"Cannot open WAV file: " + filePath.wstring() + L"\n").c_str());
        return false;
    }

    // RIFF 헤더 읽기
    char riff[4];
    file.read(riff, 4);
    if (strncmp(riff, "RIFF", 4) != 0) {
        OutputDebugString(L"Not a valid WAV file (RIFF)\n");
        return false;
    }

    uint32_t fileSize;
    file.read(reinterpret_cast<char*>(&fileSize), 4);

    char wave[4];
    file.read(wave, 4);
    if (strncmp(wave, "WAVE", 4) != 0) {
        OutputDebugString(L"Not a valid WAV file (WAVE)\n");
        return false;
    }

    // 청크 읽기
    bool formatFound = false;
    bool dataFound = false;

    while (!file.eof() && (!formatFound || !dataFound)) {
        char chunkId[4];
        uint32_t chunkSize;

        file.read(chunkId, 4);
        if (file.eof()) break;

        file.read(reinterpret_cast<char*>(&chunkSize), 4);

        if (strncmp(chunkId, "fmt ", 4) == 0) {
            // WAVEFORMATEX 구조체 크기 할당
            _wfx = reinterpret_cast<WAVEFORMATEX*>(malloc(sizeof(WAVEFORMATEX)));

            // fmt 청크 읽기
            file.read(reinterpret_cast<char*>(&_wfx->wFormatTag), 2);
            file.read(reinterpret_cast<char*>(&_wfx->nChannels), 2);
            file.read(reinterpret_cast<char*>(&_wfx->nSamplesPerSec), 4);
            file.read(reinterpret_cast<char*>(&_wfx->nAvgBytesPerSec), 4);
            file.read(reinterpret_cast<char*>(&_wfx->nBlockAlign), 2);
            file.read(reinterpret_cast<char*>(&_wfx->wBitsPerSample), 2);
            _wfx->cbSize = 0;

            // 남은 바이트 스킵
            if (chunkSize > 16) {
                file.seekg(chunkSize - 16, std::ios::cur);
            }

            formatFound = true;
        }
        else if (strncmp(chunkId, "data", 4) == 0) {
            // 오디오 데이터 읽기
            _bufferSize = chunkSize;
            _buffer = new uint8_t[_bufferSize];
            file.read(reinterpret_cast<char*>(_buffer), _bufferSize);

            // 재생 시간 계산
            if (_wfx) {
                _duration = static_cast<float>(_bufferSize) /
                    static_cast<float>(_wfx->nAvgBytesPerSec);
            }

            dataFound = true;
        }
        else {
            // 다른 청크는 스킵
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    file.close();

    if (formatFound && dataFound) {
        _isLoaded = true;
        OutputDebugStringW((L"WAV file loaded successfully: " + filePath.wstring() + L"\n").c_str());
        return true;
    }

    Cleanup();

    //FILE* fp = nullptr;
    //errno_t err = _wfopen_s(&fp, filePath.c_str(), L"rb");
    // if (!fp) return false;

    //DWORD chunkId, chunkSize, format;
    //fread(&chunkId, sizeof(DWORD), 1, fp);   // "RIFF"
    //fread(&chunkSize, sizeof(DWORD), 1, fp);
    //fread(&format, sizeof(DWORD), 1, fp);    // "WAVE"

    //// fmt chunk
    //fread(&chunkId, sizeof(DWORD), 1, fp);   // "fmt "
    //fread(&chunkSize, sizeof(DWORD), 1, fp);

    //WAVEFORMATEX* wfx = (WAVEFORMATEX*)malloc(chunkSize);
    //fread(wfx, chunkSize, 1, fp);

    //// data chunk
    //fread(&chunkId, sizeof(DWORD), 1, fp);   // "data"
    //fread(&chunkSize, sizeof(DWORD), 1, fp);

    //BYTE* buffer = (BYTE*)malloc(chunkSize);
    //fread(buffer, chunkSize, 1, fp);

    //fclose(fp);

    //_wfx = wfx;
    //_buffer = buffer;
    //_bufferSize = chunkSize;
    return true;
}

void AudioSource::Cleanup()
{
    if (_buffer) {
        delete[] _buffer;
        _buffer = nullptr;
    }

    if (_wfx) {
        free(_wfx);
        _wfx = nullptr;
    }

    _bufferSize = 0;
    _duration = 0.0f;
    _isLoaded = false;
}

//void AudioSource::PlayAudio(IXAudio2& xaudio2, float volume, float speed, bool loop)
//{
//  
//}
