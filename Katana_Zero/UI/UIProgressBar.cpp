#include "pch.h"
#include "UIProgressBar.h"
#include "../Managers/ResourceManager.h"
#include "../Resources/Texture.h"

UIProgressBar::UIProgressBar(Vector2 pos, string key, int32 width, int32 height)
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
	fProgress = 1.f;
}

UIProgressBar::~UIProgressBar()
{
}

void UIProgressBar::Update(float deltaTime)
{
}

void UIProgressBar::Render(HDC hdc)
{
	Vector2 pos = { vPos.x - iSizeX * 0.5f, vPos.y - iSizeY * 0.5f };
	_texture->RenderProgress(hdc, pos, fProgress, iSizeX * fProgress, iSizeY);
}
