#pragma once
#include "UIBase.h"

class Texture;

class UIProgressBar : public UIBase
{
	using Super = UIBase;
private:
	Texture* _texture = nullptr;
	float fProgress = 1.f;

public:
	UIProgressBar(Vector2 pos, string key, int32 width = 0, int32 height = 0);
	virtual ~UIProgressBar();

	void Update(float deltaTime) override;
	void Render(HDC hdc) override;

	void SetProgress(float progress) { fProgress = progress; }
};

