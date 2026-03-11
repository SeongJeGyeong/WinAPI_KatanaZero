#pragma once
#include "UIBase.h"

class Texture;

class UIImage : public UIBase
{
	using Super = UIBase;
private:
	Texture* _texture = nullptr;
	int32 iAlpha = -1;
	float fScale = 1.f;
public:
	UIImage(Vector2 pos, string key, int32 width = 0, int32 height = 0, float scale = 1.f, int32 alpha = -1);
	virtual ~UIImage();

	void Update(float deltaTime) override;
	void Render(HDC hdc) override;

	void SetAlpha(int32 alpha) { iAlpha = alpha; }
	void SetScale(float scale) { fScale = scale; }
	void ImageFade(bool fadeIn, float deltaTime);

};