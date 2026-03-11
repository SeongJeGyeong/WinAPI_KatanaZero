#pragma once
#include "../UI/UIWorkTool.h"

class Camera;

class Scene
{
protected:
	int32 iSceneSizeX;
	int32 iSceneSizeY;

	string sMapName;

	Camera* _sceneCamera = nullptr;
	UIWorkTool _UI;
	ESceneType _sceneType = ESceneType::NONE;

public:
	virtual ~Scene(){}

	virtual void Init();
	virtual void Destroy();
	virtual void Update(float deltaTime);
	virtual void PostUpdate(float deltaTime);
	virtual void Render(HDC hdc);

	ESceneType GetSceneType() { return _sceneType; }

	Vector2 GetSceneSize() { return Vector2(iSceneSizeX, iSceneSizeY); }
	Camera* GetSceneCamera() { return _sceneCamera; }
	string GetMapName() { return sMapName; }
};

