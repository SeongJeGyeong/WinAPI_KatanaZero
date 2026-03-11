#include "pch.h"
#include "UIImage.h"
#include "../Managers/ResourceManager.h"
#include "../Resources/Texture.h"
#include "UIAnimImage.h"

UIImage::UIImage(Vector2 pos, string key, int32 width, int32 height, float scale, int32 alpha)
	: Super(pos)
{
	_texture = ResourceManager::GetInstance()->GetTexture(key);
	if (_texture == nullptr) return;
	if (width != 0 || height != 0)
	{
		iSizeX = width;
		iSizeY = height;
		bIsStretch = true;
	}
	else
	{
		iSizeX = _texture->GetTextureSizeX();
		iSizeY = _texture->GetTextureSizeY();
	}
	iAlpha = alpha;
	fScale = scale;
}

UIImage::~UIImage()
{
}

void UIImage::Update(float deltaTime)
{
}

void UIImage::Render(HDC hdc)
{
	if (iAlpha == 0) return;
	if (_texture)
	{
		_texture->RenderTexture(hdc, vPos, iSizeX * fScale, iSizeY * fScale, iAlpha);
	}
}

void UIImage::ImageFade(bool fadeOut, float deltaTime)
{
	float fAlpha = fadeOut ? iAlpha + deltaTime : iAlpha - deltaTime;
	fAlpha = clamp(fAlpha, 0.f, 255.f);
	iAlpha = fAlpha;
	wstring str = std::format(L"alpha({0})\n", iAlpha);
	OutputDebugString(str.c_str());
}