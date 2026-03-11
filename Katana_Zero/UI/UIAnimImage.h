#pragma once
#include "UIBase.h"

class Sprite;

class UIAnimImage : public UIBase
{
	using Super = UIBase;
private:
	Sprite* _sprite = nullptr;
	int32 iAlpha = -1;
	float fElapsedTime = 0.f;
	int iCurrentFrame = 0;
	bool bPlaying = false;
	float fScale = 1.f;

public:
	UIAnimImage(Vector2 pos, string key, int32 width = 0, int32 height = 0, float scale = 1, int32 alpha = -1);
	virtual ~UIAnimImage();

	void Update(float deltaTime) override;
	void Render(HDC hdc) override;

	void SetAlpha(int32 alpha) { iAlpha = alpha; }
	void ImageFade(bool fadeIn, float deltaTime);

};