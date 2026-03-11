#pragma once
#include "Scene.h"
class LobbyScene : public Scene
{
	using Super = Scene;

public:
	function<void(string)> OnStartGame;
	function<void()> OnOpenEditor;
	function<void()> OnExitGame;

public:
	virtual void Init() override;
	virtual void Destroy() override;
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;
};

