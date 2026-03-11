#pragma once
#include "Component.h"

class BossMovementComponent : public Component
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
	Vector2 vVelocity = { 0, 0 };
	Vector2 vAcceleration = { 0, 0 };

	Vector2 vNewPos;

	bool bOnGround = false;
	bool bOnStair = false;

	////// lunge //////
	bool bIsLunge;
	const float fLungeSpeed = 1000.f;
	///////////////////


public:
	virtual void InitComponent(Actor* owner);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void ApplyPhysics(float deltaTime);

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

	void SetLungeJump(bool lunge) { bIsLunge = lunge; }

	void ParabolicJump(const Vector2& targetPos);
	Vector2 CalculateParabolicVelocity(const Vector2& startPos, const Vector2& targetPos);
};

