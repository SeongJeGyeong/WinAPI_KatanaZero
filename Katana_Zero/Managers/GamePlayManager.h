#pragma once
#include "../Utils/Singleton.h"
#include <thread>

struct FrameSnapshot
{
	HBITMAP hBitmap;
	int32 width;
	int32 height;
};

class GamePlayManager : public Singleton<GamePlayManager>
{
	friend Singleton<GamePlayManager>;
private:
	GamePlayManager() {}

	deque<FrameSnapshot> frameBuffer;
	HDC hdcMemCache = nullptr;  // 재사용을 위한 메모리 DC
	int32 iFrameSkipCount = 15;
	int32 iCurrentFrameCounter = 0;
	bool bIsRewinding = false;
	bool bIsPaused = false;
	bool bIsCaptureStop = false;

public:
	void Init();
	void Destroy();
	void Update();

	void ClearBuffer() { frameBuffer.clear(); }
	void CaptureFrame(HDC hdc, int32 width, int32 height);

	void StartRewind();
	bool RenderRewind(HDC hdc);

	bool GetIsCaptureStop() { return bIsCaptureStop; }
	void SetIsCaptureStop(bool captureStop) { bIsCaptureStop = captureStop; }

	bool GetIsPaused() { return bIsPaused; }
	void SetIsPaused(bool paused) { bIsPaused = paused; }

	void TogglePause() { bIsPaused = !bIsPaused; }

	bool GetIsRewinding() { return bIsRewinding; }
};

