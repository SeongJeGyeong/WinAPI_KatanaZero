#include "pch.h"
#include "EditorCamera.h"
#include "../Managers/InputManager.h"

void EditorCamera::Init()
{
	Super::Init();

	fCameraWidth = 1280.f;
	fCameraHeight = 720.f;
}

void EditorCamera::Update(float deltaTime)
{
	/*if (InputManager::GetInstance()->GetButtonPressed(KeyType::W))
	{
		vPos.y -= 200.f * deltaTime;
	}
	if (InputManager::GetInstance()->GetButtonPressed(KeyType::S))
	{
		vPos.y += 200.f * deltaTime;
	}
	if (InputManager::GetInstance()->GetButtonPressed(KeyType::A))
	{
		vPos.x -= 200.f * deltaTime;
	}
	if (InputManager::GetInstance()->GetButtonPressed(KeyType::D))
	{
		vPos.x += 200.f * deltaTime;
	}*/

	float halfSizeX = fCameraWidth / 2;
	float halfSizeY = fCameraHeight / 2;
	vPos.x = clamp(vPos.x, halfSizeX, iWorldSizeX - halfSizeX);
	vPos.y = clamp(vPos.y, halfSizeY, iWorldSizeY - halfSizeY);
}

void EditorCamera::Render(HDC hdc)
{

}

Vector2 EditorCamera::ConvertScreenPos(Vector2 worldPos)
{
	Vector2 convertPos;
	convertPos.x = worldPos.x - (vPos.x - (fCameraWidth / 2));
	convertPos.y = worldPos.y - (vPos.y - (fCameraHeight / 2));
	return convertPos;
}

Vector2 EditorCamera::ConvertWorldPos(Vector2 screenPos)
{
	Vector2 convertPos;
	convertPos.x = screenPos.x + (vPos.x - (fCameraWidth / 2));
	convertPos.y = screenPos.y + (vPos.y - (fCameraHeight / 2));
	return convertPos;
}

POINT EditorCamera::ConvertScreenPos(POINT worldPos)
{
	POINT convertPos;
	convertPos.x = worldPos.x - (vPos.x - (fCameraWidth / 2));
	convertPos.y = worldPos.y - (vPos.y - (fCameraHeight / 2));
	return convertPos;

}

POINT EditorCamera::ConvertWorldPos(POINT screenPos)
{
	POINT convertPos;
	convertPos.x = screenPos.x + (vPos.x - (fCameraWidth / 2));
	convertPos.y = screenPos.y + (vPos.y - (fCameraHeight / 2));
	return convertPos;
}
