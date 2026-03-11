#include "pch.h"
#include "CameraComponent.h"
#include "../Game/Game.h"
#include "../Objects/Camera.h"
#include "../Managers/InputManager.h"
#include "../Managers/TimeManager.h"

void CameraComponent::InitComponent(Actor* owner)
{
	SetOwner(owner);
}

void CameraComponent::UpdateComponent(float deltaTime)
{
	Vector2 pos = GetPos();
	Vector2 screenSize = _camera->GetCameraSize();
	Vector2 worldSize = _camera->GetWorldSize();
	float halfSizeX = screenSize.x / 2;
	float halfSizeY = screenSize.y / 2;

	pos.x = clamp(pos.x, halfSizeX, worldSize.x - halfSizeX);
	pos.y = clamp(pos.y, halfSizeY, worldSize.y - halfSizeY);

	_camera->SetTargetPos(pos);
}

void CameraComponent::RenderComponent(HDC hdc)
{
	_camera->Render(hdc);
}

Vector2 CameraComponent::ConvertScreenPos(Vector2 worldPos)
{
	return _camera->ConvertScreenPos(worldPos);
}

Vector2 CameraComponent::ConvertWorldPos(Vector2 screenPos)
{
	return _camera->ConvertWorldPos(screenPos);
}

POINT CameraComponent::ConvertScreenPos(POINT worldPos)
{
	return _camera->ConvertScreenPos(worldPos);
}

POINT CameraComponent::ConvertWorldPos(POINT screenPos)
{
	return _camera->ConvertWorldPos(screenPos);
}

void CameraComponent::SetSceneCameraShake(bool shake)
{
	_camera->SetCameraShake(shake);
}