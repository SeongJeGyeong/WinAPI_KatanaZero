#pragma once
#include "Component.h"

struct tileRenderInfo
{
	Vector2 worldPos;
	Vector2 tilePos;
};

class Texture;

class TileRenderer : public Component
{
public:
	TileRenderer() { iHalfTileSize = TILE_SIZE / 2; }
	~TileRenderer();

	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	HDC _mapCacheDC = nullptr;
	HBITMAP  _mapCacheBitmap = nullptr;

	int32 iMapSizeX;
	int32 iMapSizeY;
	int32 iHalfTileSize;
	vector<Texture*> _tileMapList;

	vector<tileRenderInfo> _tileRenderList;

public:
	virtual void InitComponent(int32 mapSizeX, int32 mapSizeY);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void AddTileTexture(Texture* texture);
	void AddTileInfo(int32 tileSetIndex, Vector2 worldPos, Vector2 tilePos);
	void AddWholeMap(int32 tileSetIndex);
};