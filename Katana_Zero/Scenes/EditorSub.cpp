#include "pch.h"
#include "EditorSub.h"
#include "../Game/Game.h"
#include "../Managers/ResourceManager.h"
#include "../Resources/Texture.h"
#include "../Managers/InputManager.h"
#include "../Managers/TimeManager.h"

void EditorSub::Init()
{
	HWND hwnd = Game::GetInstance()->GetHwndSub();
	::GetClientRect(hwnd, &_rect);

	_hdc = ::GetDC(hwnd);

	// 서브창도 더블 버퍼링
	_hdcBack = ::CreateCompatibleDC(_hdc);
	_bmpBack = ::CreateCompatibleBitmap(_hdc, _rect.right, _rect.bottom); // hdc와 호환되는 비트맵 생성
	HBITMAP prev = (HBITMAP)::SelectObject(_hdcBack, _bmpBack); // DC와 BMP를 연결
	::DeleteObject(prev);

	_tileTextureList = ResourceManager::GetInstance()->GetTileMapList();
	ChangeTileSet(0);
}

void EditorSub::Update()
{
	if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
	{
		POINT mousePos = InputManager::GetInstance()->GetSubMousePos();
		// 클릭한 그리드 위치 계산
		int x = mousePos.x / TILE_SIZE;
		int y = mousePos.y / TILE_SIZE;

		int32 tileSetGridX = iSizeX / TILE_SIZE;
		int32 tileSetGridY = iSizeY / TILE_SIZE;

		if (x >= 0 && x < tileSetGridX && y >= 0 && y < tileSetGridY)
		{
			vSelectedPos.x = x;
			vSelectedPos.y = y;
		}
	}

	if (InputManager::GetInstance()->GetButtonDown(KeyType::KEY_1))
	{
		ChangeTileLayer(0);
	}
	else if (InputManager::GetInstance()->GetButtonDown(KeyType::KEY_2))
	{
		ChangeTileLayer(1);
	}
	else if (InputManager::GetInstance()->GetButtonDown(KeyType::KEY_3))
	{
		ChangeTileLayer(2);
	}

	if (InputManager::GetInstance()->GetButtonDown(KeyType::Q))
	{
		if(iSelectedTileSet != 0) ChangeTileSet(iSelectedTileSet-1);
	}
	else if (InputManager::GetInstance()->GetButtonDown(KeyType::E))
	{
		if(iSelectedTileSet < _tileTextureList.size()-1) ChangeTileSet(iSelectedTileSet + 1);
	}

}

void EditorSub::Render()
{
	if (iSelectedTileSet == 2)
	{
		_tileTextureList[iSelectedTileSet]->RenderTextureNoAlpha(_hdcBack, vRenderPos);
	}
	else
	{
		_tileTextureList[iSelectedTileSet]->RenderTexture(_hdcBack, vRenderPos);
	}

	int32 maxX = GWinSizeY / TILE_SIZE;
	for (int y = 0; y <= maxX; y++)
	{
		MoveToEx(_hdcBack, 0, y * TILE_SIZE, nullptr);
		LineTo(_hdcBack, GWinSizeX, y * TILE_SIZE);
	}

	int32 maxY = GWinSizeX / TILE_SIZE;
	for (int x = 0; x <= maxY; x++)
	{
		MoveToEx(_hdcBack, x * TILE_SIZE, 0, nullptr);
		LineTo(_hdcBack, x * TILE_SIZE, GWinSizeY);
	}

	HPEN bluePen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
	HPEN hOldPen = (HPEN)SelectObject(_hdcBack, bluePen);
	SelectObject(_hdcBack, GetStockObject(NULL_BRUSH));

	Rectangle(_hdcBack, vRenderPos.x - iSizeX / 2, vRenderPos.y - iSizeY / 2, vRenderPos.x + iSizeX / 2, vRenderPos.y + iSizeY / 2);

	if (vSelectedPos.x != -1 || vSelectedPos.y != -1)
	{
		int x = vSelectedPos.x * TILE_SIZE;
		int y = vSelectedPos.y * TILE_SIZE;

		HPEN redPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		(HPEN)SelectObject(_hdcBack, redPen);
		SelectObject(_hdcBack, GetStockObject(NULL_BRUSH));
		Rectangle(_hdcBack, x, y, x + TILE_SIZE, y + TILE_SIZE);
		DeleteObject(redPen);
	}

	SelectObject(_hdcBack, hOldPen);
	DeleteObject(bluePen);

	{
		POINT mousePos = InputManager::GetInstance()->GetSubMousePos();
		wstring str = std::format(L"Mouse({0}, {1})", mousePos.x, mousePos.y);
		::TextOut(_hdcBack, 300, 10, str.c_str(), static_cast<int32>(str.size()));
	}

	{
		wstring layer;
		switch (eSelectedLayer)
		{
		case BACKGROUND:
			layer = L"Background";
			break;
		case FOREGROUND:
			layer = L"Foreground";
			break;
		case ACTOR:
			layer = L"Actor";
			break;
		default:
			break;
		}
		wstring str = L"TileLayer : " + layer;
		::TextOut(_hdcBack, 20, 10, str.c_str(), static_cast<int32>(str.size()));
	}

	// Double Buffering
	::BitBlt(_hdc, 0, 0, _rect.right, _rect.bottom, _hdcBack, 0, 0, SRCCOPY); // 비트 블릿 : 고속 복사
	::PatBlt(_hdcBack, 0, 0, _rect.right, _rect.bottom, WHITENESS);
}

Vector2 EditorSub::GetSelectedTexturePos()
{
	int32 texOriginX = (vRenderPos.x - iSizeX * 0.5) / TILE_SIZE;
	int32 texOriginY = (vRenderPos.y - iSizeY * 0.5) / TILE_SIZE;
	return Vector2(vSelectedPos.x - texOriginX, vSelectedPos.y - texOriginY);
}

void EditorSub::ChangeTileLayer(int32 layerIndex)
{
	eSelectedLayer = (ERenderLayer)layerIndex;

	switch (eSelectedLayer)
	{
	case BACKGROUND:
		break;
	case FOREGROUND:
		break;
	case ACTOR:
		break;
	default:
		break;
	}

}

void EditorSub::ChangeTileSet(int32 tilesetIndex)
{
	iSelectedTileSet = tilesetIndex;
	iSizeX = _tileTextureList[iSelectedTileSet]->GetTextureSizeX();
	iSizeY = _tileTextureList[iSelectedTileSet]->GetTextureSizeY();

	vRenderPos = { (float)iSizeX / 2, (float)iSizeY / 2 };
}
