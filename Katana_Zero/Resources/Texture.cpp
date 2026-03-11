#include "pch.h"
#include "Texture.h"
#include "../Game/Game.h"

void Texture::Load(wstring filePath, int32 alpha)
{
	// WinAPI 텍스처 로딩하는 방법
	HDC hdc = ::GetDC(Game::GetInstance()->GetHwnd());

	_textureHdc = ::CreateCompatibleDC(hdc);
	_bitmap = (HBITMAP)::LoadImageW(
		nullptr,
		filePath.c_str(),
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION
	);
	if (_bitmap == 0)
	{
		::MessageBox(Game::GetInstance()->GetHwnd(), filePath.c_str(), L"Invalid Texture Load", MB_OK);
		return;
	}

	HBITMAP prev = (HBITMAP)::SelectObject(_textureHdc, _bitmap);
	::DeleteObject(prev);

	BITMAP bit = {};
	::GetObject(_bitmap, sizeof(BITMAP), &bit);

	iSizeX = bit.bmWidth;
	iSizeY = bit.bmHeight;
	iAlpha = alpha;

	// 좌우 반전된 hdc도 하나 저장
	_flippedTextureHdc = ::CreateCompatibleDC(_textureHdc);
	HBITMAP flippedBitmap = ::CreateCompatibleBitmap(_textureHdc, iSizeX, iSizeY);
	(HBITMAP)::SelectObject(_flippedTextureHdc, flippedBitmap);

	StretchBlt(
		_flippedTextureHdc,
		0, 0, iSizeX, iSizeY,
		_textureHdc,
		iSizeX - 1, 0, -iSizeX, iSizeY,
		SRCCOPY
	);

	DeleteObject(flippedBitmap);
}

void Texture::RenderTexture(HDC hdc, Vector2 pos, int32 width, int32 height, int32 alpha)
{
	if (width == 0) width = iSizeX;
	if (height == 0) height = iSizeY;

	BLENDFUNCTION op;
	op.BlendOp = AC_SRC_OVER;
	op.BlendFlags = 0;
	op.SourceConstantAlpha = (alpha == -1) ? iAlpha : alpha;
	op.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(hdc,
		(int32)pos.x - (width * 0.5f),
		(int32)pos.y - (height * 0.5f),
		width,
		height,
		_textureHdc,
		(int32)0,
		(int32)0,
		iSizeX,
		iSizeY,
		op);
}

void Texture::RenderTexture(HDC hdc, int32 startPosX, int32 startPosY, int32 width, int32 height, int32 srcStartX, int32 srcStartY, int32 srcEndX, int32 srcEndY, bool isFlipped, int32 alpha)
{
	BLENDFUNCTION op;
	op.BlendOp = AC_SRC_OVER;
	op.BlendFlags = 0;
	op.SourceConstantAlpha = (alpha == -1) ? iAlpha : alpha;
	op.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(hdc,
		startPosX,
		startPosY,
		width,
		height,
		isFlipped ? _flippedTextureHdc : _textureHdc,
		srcStartX,
		srcStartY,
		srcEndX,
		srcEndY,
		op);
}

void Texture::RenderTextureNoAlpha(HDC hdc, Vector2 pos, int32 width, int32 height)
{
	if (width == 0) width = iSizeX;
	if (height == 0) height = iSizeY;

	StretchBlt(
		hdc,
		(int32)pos.x - (width * 0.5f),
		(int32)pos.y - (height * 0.5f),
		width,
		height,
		_textureHdc,
		0,
		0,
		iSizeX,
		iSizeY,
		SRCCOPY
	);
}

void Texture::RenderProgress(HDC hdc, Vector2 pos, float progress, int32 width, int32 height)
{
	StretchBlt(
		hdc,
		(int32)pos.x,
		(int32)pos.y,
		width,
		height,
		_textureHdc,
		0,
		0,
		iSizeX * progress,
		iSizeY,
		SRCCOPY
	);
}
