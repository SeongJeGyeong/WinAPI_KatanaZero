#include "pch.h"
#include "TileRenderer.h"
#include "../Resources/Texture.h"
#include "../Game/Game.h"
#include "../Objects/Camera.h"

TileRenderer::~TileRenderer()
{
    SAFE_DELETE(_mapCacheDC);
    SAFE_DELETE(_mapCacheBitmap);
}

void TileRenderer::InitComponent(int32 mapSizeX, int32 mapSizeY)
{
    iMapSizeX = mapSizeX;
    iMapSizeY = mapSizeY;
	HDC screenDC = GetDC(nullptr);
	_mapCacheDC = CreateCompatibleDC(screenDC);
	_mapCacheBitmap = CreateCompatibleBitmap(screenDC, iMapSizeX, iMapSizeY);
	SelectObject(_mapCacheDC, _mapCacheBitmap);
	ReleaseDC(nullptr, screenDC);

	iHalfTileSize = TILE_SIZE / 2;
}

void TileRenderer::UpdateComponent(float deltaTime)
{
}

void TileRenderer::RenderComponent(HDC hdc)
{
	Camera* camera = Game::GetInstance()->GetCurrentSceneCamera();
	if (camera == nullptr) return;

	Vector2 screenPos = camera->ConvertScreenPos(Vector2(0, 0));
	Vector2 screenSize = camera->GetCameraSize();

    BitBlt(
        hdc,
        screenPos.x, 
        screenPos.y,
        iMapSizeX,
        iMapSizeY,
        _mapCacheDC,
        0, 
        0,         
        SRCCOPY
    );
}

void TileRenderer::AddTileTexture(Texture* texture)
{
	_tileMapList.push_back(texture);
}

void TileRenderer::AddTileInfo(int32 tileSetIndex, Vector2 worldPos, Vector2 tilePos)
{
    if (_tileMapList.empty()) return;

    BLENDFUNCTION blendOp = {};
    blendOp.BlendOp = AC_SRC_OVER;
    blendOp.BlendFlags = 0;
    blendOp.SourceConstantAlpha = 255;
    blendOp.AlphaFormat = AC_SRC_ALPHA;

    int32 destX = static_cast<int32>(worldPos.x - TILE_SIZE * 0.5f);
    int32 destY = static_cast<int>(worldPos.y - TILE_SIZE * 0.5f);

    int32 srcX = static_cast<int32>(tilePos.x * TILE_SIZE);
    int32 srcY = static_cast<int32>(tilePos.y * TILE_SIZE);

    AlphaBlend(
        _mapCacheDC,
        destX,
        destY,
        TILE_SIZE,
        TILE_SIZE,
        _tileMapList[tileSetIndex]->GetTexture(),
        srcX,
        srcY,
        TILE_SIZE,
        TILE_SIZE,
        blendOp
    );
}

void TileRenderer::AddWholeMap(int32 tileSetIndex)
{
    _mapCacheDC = _tileMapList[tileSetIndex]->GetTexture();
}