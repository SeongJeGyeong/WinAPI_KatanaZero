#pragma once
#include "Component.h"

class Player;

class PlayerMovementComponent : public Component
{
	using Super = Component;
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	Player* _player = nullptr;

	Vector2 vGravity = Vector2(0, 2000.f);
	Vector2 vVelocity = {};
	Vector2 vAcceleration = {};

	Vector2 vNewPos;

	float fJumpInitialVelocity = 450.f;
	float fAirResistance = 50.f;

	bool bOnGround = false;
	
	bool bIsLeftWall = false;
	bool bIsRightWall = false;

	bool bIsJumped = false;
	bool bIsPlatform = false;
	bool bIsAir = false;
	bool bOnStair = false;
	bool bIsStairRight = false;

	Vector2 vStairDir = { 0, 0 };

	float fMaxFrameRate = 60.f;

public:
	virtual void InitComponent(Actor* owner);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void ApplyPhysics(float deltaTime);
	void UpdatePosition();

	void Jump();
	void WallJump();
	void AttackForce(Vector2 dir);

	Vector2 GetAcceleration() { return vAcceleration; }
	void SetAcceleration(Vector2 accel) { vAcceleration = accel; }
	void AddAcceleration(Vector2 accel) { vAcceleration += accel; }

	Vector2 GetVelocity() { return vVelocity; }
	void SetVelocity(Vector2 velocity) { vVelocity = velocity; }
	void AddVelocity(Vector2 velocity) { vVelocity += velocity; }
	void SetVelocityX(float xValue) { vVelocity.x = xValue; }
	void SetVelocityY(float yValue) { vVelocity.y = yValue; }

	Vector2 GetNewPos() { return vNewPos; }
	void SetNewPos(Vector2 pos) { vNewPos = pos; }
	void AddNewPos(Vector2 pos) { vNewPos += pos; }
	void SetNewPosX(float xValue) { vNewPos.x = xValue; }
	void SetNewPosY(float yValue) { vNewPos.y = yValue; }

	bool GetOnGround() { return bOnGround; }
	void SetOnGround(bool isGround) { bOnGround = isGround; }
	bool GetOnStair() { return bOnStair; }
	void SetOnStair(bool onStair) { bOnStair = onStair; }
	bool GetIsJumped() { return bIsJumped; }
	void SetIsJumped(bool isJumped) { bIsJumped = isJumped; }

	Vector2 GetStairDirection() { return vStairDir; }
	void SetStairDirection(Vector2 direction) { vStairDir = direction; }
	bool GetIsStairRight() { return bIsStairRight; }
	void SetIsStairRight(bool stair) { bIsStairRight = stair; }

	bool GetIsWall() { return bIsLeftWall || bIsRightWall; }
	bool GetIsLeftWall() { return bIsLeftWall; }
	bool GetIsRightWall() { return bIsRightWall; }
	void ReleaseWall() { bIsLeftWall = false; bIsRightWall = false; }
	void SetIsLeftWall(bool isWall) { bIsLeftWall = isWall; }
	void SetIsRightWall(bool isWall) { bIsRightWall = isWall; }
	bool GetIsPlatform() { return bIsPlatform; }
	void SetIsPlatform(bool isPlatform) { bIsPlatform = isPlatform; }

	virtual uint32  GetPriority() override { return EComponentPriority::MOVEMENT; }
};

