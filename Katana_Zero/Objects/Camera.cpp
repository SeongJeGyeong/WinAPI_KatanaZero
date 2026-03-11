#include "pch.h"
#include "Camera.h"
#include "../Managers/TimeManager.h"

void Camera::Init()
{
	Super::Init();

	fCameraWidth = 1280.f;
	fCameraHeight = 720.f;
}

void Camera::Update(float deltaTime)
{
	// 먼저 목표 위치로 설정
	vPos = vTargetPos;

	if (bCameraShake)
	{
		TimeManager* timeManager = TimeManager::GetInstance();
		fShakeDuration += timeManager->GetConstDeltaTime();

		if (fShakeDuration >= 0.03f)
		{
			OutputDebugString(L"shake\n");
			fShakeDuration = 0;

			// 오프셋 계산
			vShakeOffset.x = (rand() % 2) ? 2.f : -2.f;
			vShakeOffset.y = (rand() % 2) ? 2.f : -2.f;
		}

		// 쉐이크 오프셋 적용
		vPos += vShakeOffset;

		fShakeTime += timeManager->GetConstDeltaTime();
		if (fShakeTime >= timeManager->GetHitStopTime())
		{
			fShakeTime = 0.f;
			bCameraShake = false;
			vShakeOffset = { 0, 0 };
		}
	}
	else
	{
		// 쉐이크가 아닐 때는 클램핑
		float halfSizeX = fCameraWidth / 2;
		float halfSizeY = fCameraHeight / 2;
		vPos.x = clamp(vPos.x, halfSizeX, iWorldSizeX - halfSizeX);
		vPos.y = clamp(vPos.y, halfSizeY, iWorldSizeY - halfSizeY);
	}
}

void Camera::Render(HDC hdc)
{
}

Vector2 Camera::ConvertScreenPos(Vector2 worldPos)
{
	Vector2 convertPos;
	convertPos.x = worldPos.x - (vPos.x - (fCameraWidth / 2));
	convertPos.y = worldPos.y - (vPos.y - (fCameraHeight / 2));
	return convertPos;
}

Vector2 Camera::ConvertWorldPos(Vector2 screenPos)
{
	Vector2 convertPos;
	convertPos.x = screenPos.x + (vPos.x - (fCameraWidth / 2));
	convertPos.y = screenPos.y + (vPos.y - (fCameraHeight / 2));
	return convertPos;
}

POINT Camera::ConvertScreenPos(POINT worldPos)
{
	POINT convertPos;
	convertPos.x = worldPos.x - (vPos.x - (fCameraWidth / 2));
	convertPos.y = worldPos.y - (vPos.y - (fCameraHeight / 2));
	return convertPos;

}

POINT Camera::ConvertWorldPos(POINT screenPos)
{
	POINT convertPos;
	convertPos.x = screenPos.x + (vPos.x - (fCameraWidth / 2));
	convertPos.y = screenPos.y + (vPos.y - (fCameraHeight / 2));
	return convertPos;
}

void Camera::SetInitialPos(const Vector2& pos)
{
	vPos = pos;
	float halfSizeX = fCameraWidth * 0.5f;
	float halfSizeY = fCameraHeight * 0.5f;
	vPos.x = clamp(vPos.x, halfSizeX, iWorldSizeX - halfSizeX);
	vPos.y = clamp(vPos.y, halfSizeY, iWorldSizeY - halfSizeY);
}
