#pragma once
#include "Actor.h"

class Animator;
class EffectorComponent;
class InputComponent;
class Camera;
class CameraComponent;
class PlayerMovementComponent;

template<typename EnumType>
class StateMachine;

class UIImage;
class UIProgressBar;

class Player : public Actor
{
	using Super = Actor;
public:
	function<void(wstring)> OnDyingMessage;
	function<void()> OnClearStage;
	~Player();

private:
	PlayerMovementComponent* _movementComp = nullptr;

	Vector2 vFrontDir = {1, 0};
	float fMoveForce = 20000;

	float fJumpPressedTime = 0.f;

	bool bPressedDown = false;
	bool bIsMaxJump = false;
	bool bIsCrouch = false;
	bool bWaitForAttack = false;
	bool bInvincible = false;

	bool bFirstAttack = true;
	float fAttackEnableTime = 0.3f;
	float fAttackWaitTime = 0.f;

	float fMaxJumpHoldTime = 0.4f;
	float fJumpHoldForce = 2000.f;

	Vector2 vHitNormal;

	StateMachine<EPlayerState>* _stateMachine;

	AttackInfo _attackInfo;

	float framecount = 0;

	UIProgressBar* _blockProgress = nullptr;
	UIProgressBar* _progressBackground = nullptr;

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	bool GetPressedDown() { return bPressedDown; }
	void SetPressedDown(bool pressedDown) { bPressedDown = pressedDown; }

	bool GetIsMaxJump() { return bIsMaxJump; }
	void SetIsMaxJump(bool maxJump) { bIsMaxJump = maxJump; }

	bool GetIsCrouch() { return bIsCrouch; }
	void SetIsCrouch(bool isCrouch) { bIsCrouch = isCrouch; }

	float GetFrontDir() { return vFrontDir.x; }

	void ClearHitActors() { _attackInfo._hitActors.clear(); }

	void SetInvincible(bool invincible) { bInvincible = invincible; }

	bool IsFlipped() { return vFrontDir.x < 0; }

	void Jump();
	void IncreaseJump(float deltaTime);
	void ReleaseJump();
	void Move(bool dir);
	void Crouch(bool active);
	void Roll(bool dir);
	void Attack();

	virtual int32 GetCurrentState() override;

	virtual void ChangeState(int32 stateType) override;

	virtual void AddForce(Vector2 force) override;
	void SetPlayerCamera(Camera* camera);

	void AttackBlocked();

	virtual void TakeDamage(Actor* damageCauser, const Vector2& attackDirection) override;

	virtual Vector2 GetNewPos() override;

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionStayOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionEndOverlap(const CollisionInfo& info) override;

	void Die(bool timeOver);

	void HitCameraShake();

private:
	void AttackDelay(float deltaTime);

	void ProcessGroundCollision(const CollisionInfo& collisionInfo);
	void ProcessGroundFloor(const CollisionInfo& collisionInfo);
	void ProcessGroundCeiling(const CollisionInfo& collisionInfo);
	void ProcessGroundWall(const CollisionInfo& collisionInfo);

	void ProcessPlatformCollision(const CollisionInfo& collisionInfo);
	void ProcessWallCollision(const CollisionInfo& collisionInfo);
	void ProcessCeilingCollision(const CollisionInfo& collisionInfo);
	void ProcessStairCollision(const CollisionInfo& collisionInfo, Vector2 oldPos);

	void printState(HDC hdc);

	void RenderHitbox(HDC hdc, Vector2 pos, float radian, float scale, COLORREF color);
};