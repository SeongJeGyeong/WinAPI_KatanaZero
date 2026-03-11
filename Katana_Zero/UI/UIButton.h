#pragma once
#include "UIBase.h"

class Texture;

class UIButton : public UIBase
{
	using Super = UIBase;
public:
	UIButton(Vector2 pos, string key, int32 width = 0, int32 height = 0, int32 fontSize = 0, uint64 textColor = RGB(255, 255, 255));
	virtual ~UIButton() {};

private:
	function<void()> onClickedEvent;
	Texture* _texture = nullptr;
	wstring sButtonText = L"";
	int32 iFontSize = 0;
	uint64 ulTextColor = RGB(255, 255, 255);
	int32 iButtonAlpha = -1;

	EButtonTextAlign _textAlign = EButtonTextAlign::CENTER;
	float fMargin = 0.f;
	POINT _prevMousePoint;

public:
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;
	void SetText(wstring text) { sButtonText = text; }
	void SetButtonTextAlign(EButtonTextAlign align) { _textAlign = align; }
	void SetMargin(float margin) { fMargin = margin; }
	void SetClickEvent(function<void()> click) { onClickedEvent = click; }

	void OnBtnHovered();
	void OnBtnUnHovered();
};

