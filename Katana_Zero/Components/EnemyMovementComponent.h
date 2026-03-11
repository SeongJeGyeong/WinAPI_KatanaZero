#pragma once
#include "Component.h"
class EnemyMovementComponent : public Component
{
	using Super = Component;
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	Vector2 vGravity = Vector2(0, 2000.f);
	Vector2 vVelocity = {0, 0};
	Vector2 vAcceleration = {0, 0};

	Vector2 vNewPos;

	bool bOnGround = false;
	bool bOnStair = false;
	bool bOnPlatform = false;

	float fMaxFrameRate = 60.f;

public:
	virtual void InitComponent(Actor* owner);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void ApplyPhysics(float deltaTime);
	void HitPhysics(float deltaTime);

	Vector2 GetAcceleration() { return vAcceleration; }
	void SetAcceleration(Vector2 accel) { vAcceleration = accel; }
	void AddAcceleration(Vector2 accel) { vAcceleration += accel; }

	Vector2 GetVelocity() { return vVelocity; }
	void SetVelocity(Vector2 velocity) { vVelocity = velocity; }
	void SetVelocityX(float xValue) { vVelocity.x = xValue; }
	void SetVelocityY(float yValue) { vVelocity.y = yValue; }

	Vector2 GetNewPos() { return vNewPos; }

	void SetNewPosX(float xValue) { vNewPos.x = xValue; }
	void SetNewPosY(float yValue) { vNewPos.y = yValue; }
	void AddNewPos(Vector2 pos) { vNewPos += pos; }

	bool GetOnGround() { return bOnGround; }
	void SetOnGround(bool onGround) { bOnGround = onGround; }
	bool GetOnStair() { return bOnStair; }
	void SetOnStair(bool onStair) { bOnStair = onStair; }
	bool GetOnPlatform() { return bOnPlatform; }
	void SetOnPlatform(bool onPlatform) { bOnPlatform = onPlatform; }
};

