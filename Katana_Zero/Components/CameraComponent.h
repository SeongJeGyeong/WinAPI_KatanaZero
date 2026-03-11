#pragma once
#include "Component.h"

class Camera;
class CameraComponent : public Component
{
	using Super = Component;
private:
	Camera* _camera = nullptr;

public:
	virtual void InitComponent(Actor* owner);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;
	Vector2 ConvertScreenPos(Vector2 worldPos);
	Vector2 ConvertWorldPos(Vector2 screenPos);
	POINT ConvertScreenPos(POINT worldPos);
	POINT ConvertWorldPos(POINT screenPos);

	void SetCamera(Camera* camera) { _camera = camera; }

	virtual uint32  GetPriority() override { return EComponentPriority::CAMERA; }
	void SetSceneCameraShake(bool shake);
};

