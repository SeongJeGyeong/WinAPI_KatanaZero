#include "pch.h"
#include "UIAnimImage.h"
#include "../Managers/ResourceManager.h"
#include "../Resources/Sprite.h"

UIAnimImage::UIAnimImage(Vector2 pos, string key, int32 width, int32 height, float scale, int32 alpha)
	: Super(pos)
{
	_sprite = ResourceManager::GetInstance()->GetSprite(key);
	if (_sprite == nullptr) return;
	if (width != 0 || height != 0)
	{
		iSizeX = width;
		iSizeY = height;
		bIsStretch = true;
	}
	else
	{
		iSizeX = _sprite->GetTextureSizeX();
		iSizeY = _sprite->GetTextureSizeY();
	}
	iAlpha = alpha;
	fScale = scale;
	bPlaying = true;
}

UIAnimImage::~UIAnimImage()
{
}

void UIAnimImage::Update(float deltaTime)
{
	if (!bPlaying) return;
	fElapsedTime += deltaTime;
	if (fElapsedTime >= _sprite->GetDuration())
	{
		fElapsedTime = 0.f;
		if (_sprite->IsSpriteEnd(iCurrentFrame))
		{
			if (!_sprite->GetIsLoop())
			{
				bPlaying = false;
			}
			else
			{
				iCurrentFrame = 0;
			}
		}
		else
		{
			++iCurrentFrame;
		}
	}
}

void UIAnimImage::Render(HDC hdc)
{
	if (iAlpha == 0) return;
	if (_sprite)
	{
		_sprite->RenderStretchedSprite(hdc, vPos, iCurrentFrame, false);
	}
}

void UIAnimImage::ImageFade(bool fadeOut, float deltaTime)
{
	float fAlpha = fadeOut ? iAlpha + deltaTime : iAlpha - deltaTime;
	fAlpha = clamp(fAlpha, 0.f, 255.f);
	iAlpha = fAlpha;
	wstring str = std::format(L"alpha({0})\n", iAlpha);
	OutputDebugString(str.c_str());
}