#pragma once

class Texture;

class EditorSub
{
private:
	RECT	_rect;
	HDC		_hdc = {};
	HDC		_hdcBack = {};
	HBITMAP _bmpBack = {};

	HDC		_hdcBitmap = {};
	HBITMAP _bitmap = 0;
	int32 iSizeX = 0;
	int32 iSizeY = 0;

	vector<Texture*> _tileTextureList;
	int32 iSelectedTileSet = 0;

	Vector2 vRenderPos;
	Vector2 vSelectedPos = { -1, -1 };

	ERenderLayer eSelectedLayer = ERenderLayer::BACKGROUND;

public:
	void Init();
	void Update();
	void Render();

	Vector2 GetSelectedTexturePos();
	ERenderLayer GetSelectedLayer() { return eSelectedLayer; }
	int32 GetSelectedTileSet() { return iSelectedTileSet; }
	Texture* GetTileTexture(int32 tilesetIndex) { return _tileTextureList[tilesetIndex]; }
	vector<Texture*> GetTileTextureList() { return _tileTextureList; }

	void ChangeTileLayer(int32 layerIndex);
	void ChangeTileSet(int32 tilesetIndex);
};

