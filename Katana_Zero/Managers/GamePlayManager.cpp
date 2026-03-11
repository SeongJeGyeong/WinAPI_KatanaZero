#include "pch.h"
#include "GamePlayManager.h"

void GamePlayManager::Init()
{
    hdcMemCache = CreateCompatibleDC(nullptr);
    iCurrentFrameCounter = 0;
}

void GamePlayManager::Destroy()
{
    DeleteDC(hdcMemCache);
}

void GamePlayManager::Update()
{
}

void GamePlayManager::CaptureFrame(HDC hdc, int32 width, int32 height)
{
	if (bIsRewinding || bIsPaused || bIsCaptureStop) return;

    ++iCurrentFrameCounter;
	if (iCurrentFrameCounter % iFrameSkipCount) return;

    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    BitBlt(hdcMem, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hOldBitmap);
    DeleteDC(hdcMem);

    frameBuffer.push_back({
        hBitmap,
        width,
        height
        });

    if(iCurrentFrameCounter >= 10) iCurrentFrameCounter = 0;
}

void GamePlayManager::StartRewind()
{
    if (frameBuffer.empty()) return;
    bIsRewinding = true;
    bIsPaused = false;
    iCurrentFrameCounter = 0;
}

bool GamePlayManager::RenderRewind(HDC hdc)
{
    if (frameBuffer.empty())
    {
        bIsRewinding = false;
        // 역재생 종료
        return true;
    }

    FrameSnapshot snap = frameBuffer.back();
    frameBuffer.pop_back();
    // 비트맵 직접 그리기
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemCache, snap.hBitmap);

    BitBlt(hdc, 0, 0, snap.width, snap.height, hdcMemCache, 0, 0, SRCCOPY);

    SelectObject(hdcMemCache, hOldBitmap);
    DeleteObject(snap.hBitmap);
    return false;
}