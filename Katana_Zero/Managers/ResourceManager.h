#pragma once
#include "../Utils/Singleton.h"

class Texture;
class Sprite;

class ResourceManager : public Singleton<ResourceManager>
{
private:
	unordered_map<string, Texture*> _textures;
	unordered_map<string, Sprite*> _sprites;
	vector<Texture*> _tileMapList;
	HCURSOR _cursor = nullptr;

	HFONT _fontList[5];

public:
	void Init(HWND hwnd, fs::path directory);
	void Update(float deltaTime);
	void Destroy() override;

	void LoadFile(const string currentKey, const string parentKey, const json& obj, const fs::path& path);
	void LoadTextureFile(const string parentKey, const json& obj, const fs::path& path);
	void LoadSpriteFile(const json& obj, const fs::path& path);
	Texture* GetTexture(string name);
	Sprite* GetSprite(string name);
	vector<Texture*> GetTileMapList() { return _tileMapList; }
	Texture* GetTileMapForIndex(int32 index) { return _tileMapList[index]; }

	HCURSOR CreateCursorFromImage(const wstring& path);

	HCURSOR GetCursor() { return _cursor; }

	HFONT GetFont(int32 size);
};