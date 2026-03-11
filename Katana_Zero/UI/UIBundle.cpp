#include "pch.h"
#include "UIBundle.h"
#include "../Managers/ResourceManager.h"
#include "../Resources/Texture.h"

UIBundle::UIBundle(Vector2 pos, string key, int32 width, int32 height, int32 count)
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

	iBundleCount = count;
}

UIBundle::~UIBundle()
{
}

void UIBundle::Update(float deltaTime)
{
}

void UIBundle::Render(HDC hdc)
{
	if (_texture)
	{
		for (int32 i = 0; i < iBundleCount; ++i)
		{
			Vector2 pos = vPos;
			pos.x += i * (iSizeX + fMargin);
			_texture->RenderTextureNoAlpha(hdc, pos, iSizeX, iSizeY);
		}
	}
}
