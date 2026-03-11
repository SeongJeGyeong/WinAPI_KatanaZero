#pragma once

class UIBase;
class UIButton;
class UIImage;
class UIBundle;
class UIProgressBar;
class UIAnimImage;

class UIWorkTool
{
private:
	vector<UIBase*> _widgetList;

	bool _isVisible = false;

public:
	void Init();
	void Destroy();

	void Update(float deltaTime);
	void Render(HDC hdc);

	void DestroyAllWidget();

	UIButton* CreateButton(Vector2 pos, string key, wstring text = L"", int32 width = 0, int32 height = 0, int32 fontSize = 0, uint64 textColor = RGB(255, 255, 255));
	UIImage* CreateImage(Vector2 pos, string key, int32 width = 0, int32 height = 0, float scale = 1.f, int32 alpha = -1);
	UIBundle* CreateBundle(Vector2 pos, string key, int32 width = 0, int32 height = 0, int32 count = 1);
	UIProgressBar* CreateProgressBar(Vector2 pos, string key, int32 width = 0, int32 height = 0);
	UIAnimImage* CreateAnimImage(Vector2 pos, string key, int32 width = 0, int32 height = 0, float scale = 1.f, int32 alpha = -1);

	void SetUIVisibility(bool visible) { _isVisible = visible; }
};

