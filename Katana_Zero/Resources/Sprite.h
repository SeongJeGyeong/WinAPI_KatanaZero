#pragma once
#include "Texture.h"
class Sprite : public Texture
{
	using Super = Texture;
private:
	int32 iSpriteSizeX;
	int32 iSpriteSizeY;
	float fMagnification = 1;		// 스프라이트 크기 확대 배율
	int32 iFrameCount = 0;
	float fDuration = 0;
	bool bLoop = false;

public:
	void LoadSprite(wstring filePath, int32 frameSizeX, int32 frameSizeY, float magnification, int32 frameCount, float duration, bool loop, int32 alpha = 255);
	void RenderStretchedSprite(HDC hdc, Vector2 pos, int32 curFrame, bool isFlipped, int32 alpha = -1);

	void RenderRotatedSprite(HDC hdc, Vector2 pos, float radian, float scale, int32 curFrame, bool isFlipped, int32 alpha = -1);
	void RenderRotatedSpriteBitBlt(HDC hdc, Vector2 pos, float radian, float scale, int32 curFrame, bool isFlipped);
	void CalculateRotatedPoints(POINT points[3], float centerX, float centerY, float radian, int imgWidth, int imgHeight);
	RECT GetRotatedBounds(int32 centerX, int32 centerY, float radian, int32 sizeX, int32 sizeY);

	bool IsSpriteEnd(int32 currentFrame)
	{
		return (currentFrame == iFrameCount - 1);
	}

	float GetDuration() { return fDuration; }
	bool GetIsLoop() { return bLoop; }

	int32 GetFrameCount() { return iFrameCount; }
};

