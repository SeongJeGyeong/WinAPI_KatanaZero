#include "pch.h"
#include "UIWorkTool.h"
#include "UIBase.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UIBundle.h"
#include "UIProgressBar.h"
#include "UIAnimImage.h"

void UIWorkTool::Destroy()
{
	DestroyAllWidget();
}

void UIWorkTool::Init()
{
	_isVisible = true;
}

void UIWorkTool::Update(float deltaTime)
{
	for (auto iter : _widgetList)
	{
		if (iter->IsOpen())
		{
			iter->Update(deltaTime);
		}
	}
}

void UIWorkTool::Render(HDC hdc)
{
	if (!_isVisible) return;
	for (auto iter : _widgetList)
	{
		if (iter->IsOpen())
		{
			iter->Render(hdc);
		}
	}
}

void UIWorkTool::DestroyAllWidget()
{
	for (auto& widget : _widgetList)
	{
		SAFE_DELETE(widget);
	}
	_widgetList.clear();
}

UIButton* UIWorkTool::CreateButton(Vector2 pos, string key, wstring text, int32 width, int32 height, int32 fontSize, uint64 textColor)
{
	UIButton* button = new UIButton(pos, key, width, height, fontSize, textColor);
	if (text != L"")
	{
		button->SetText(text);
	}
	_widgetList.emplace_back(button);
	return button;
}

UIImage* UIWorkTool::CreateImage(Vector2 pos, string key, int32 width, int32 height, float scale, int32 alpha)
{
	UIImage* image = new UIImage(pos, key, width, height, scale, alpha);
	_widgetList.emplace_back(image);
	return image;
}

UIBundle* UIWorkTool::CreateBundle(Vector2 pos, string key, int32 width, int32 height, int32 count)
{
	UIBundle* bundle = new UIBundle(pos, key, width, height, count);
	_widgetList.emplace_back(bundle);
	return bundle;
}

UIProgressBar* UIWorkTool::CreateProgressBar(Vector2 pos, string key, int32 width, int32 height)
{
	UIProgressBar* progressBar = new UIProgressBar(pos, key, width, height);
	_widgetList.emplace_back(progressBar);
	return progressBar;
}

UIAnimImage* UIWorkTool::CreateAnimImage(Vector2 pos, string key, int32 width, int32 height, float scale, int32 alpha)
{
	UIAnimImage* animImage = new UIAnimImage(pos, key, width, height, scale, alpha);
	_widgetList.emplace_back(animImage);
	return animImage;
}
