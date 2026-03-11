#pragma once
#include "Object.h"
class Camera : public Object
{
	using Super = Object;

private:
	float fCameraWidth;
	float fCameraHeight;
	int32 iWorldSizeX;
	int32 iWorldSizeY;

	Vector2 vShakePos{ 2.f, 2.f };
	float fShakeTime = 0.f;
	float fShakeDuration = 0.f;
	bool bCameraShake = false;

	Vector2 vTargetPos;      // CameraComponent가 설정한 목표 위치
	Vector2 vShakeOffset;    // 쉐이크 오프셋

public:
	virtual void Init() override;
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;
	Vector2 ConvertScreenPos(Vector2 worldPos);
	Vector2 ConvertWorldPos(Vector2 screenPos);
	POINT ConvertScreenPos(POINT worldPos);
	POINT ConvertWorldPos(POINT screenPos);

	Vector2 GetWorldSize() { return Vector2(iWorldSizeX, iWorldSizeY); }
	void SetWorldSize(int32 x, int32 y) { iWorldSizeX = x; iWorldSizeY = y; }

	Vector2 GetCameraSize() { return { fCameraWidth , fCameraHeight }; }

	void SetCameraShake(bool cameraShake) { bCameraShake = cameraShake; }

	void SetInitialPos(const Vector2& pos);
	void SetTargetPos(const Vector2& pos) { vTargetPos = pos; }

};

