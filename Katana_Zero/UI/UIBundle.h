#pragma once
#include "UIBase.h"

class Texture;

class UIBundle : public UIBase
{
	using Super = UIBase;

private:
	Texture* _texture = nullptr;
	int32 iBundleCount = 1;
	float fMargin = 0.f;

public:
	UIBundle(Vector2 pos, string key, int32 width = 0, int32 height = 0, int32 count = 1);
	virtual ~UIBundle();

	void Update(float deltaTime) override;
	void Render(HDC hdc) override;
	void SetMargin(float margin) { fMargin = margin; }
	void SetBundleCount(int32 count) { iBundleCount = count; }
};

