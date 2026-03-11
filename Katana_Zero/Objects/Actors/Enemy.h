#pragma once
#include "Actor.h"

class Camera;
class Player;

template<typename EnumType>
class StateMachine;

class Enemy : public Actor
{
	using Super = Actor;
public:
	~Enemy();

	function<void(Vector2, Vector2, float, float)> OnCreateBullet;
	function<void()> OnDie;
private:
	Camera* _worldCamera = nullptr;

	StateMachine<EEnemyState>* _stateMachine;

	Vector2 vNewPos;
	Vector2 vFrontDir = { 1, 0 };

	Player* _targetPlayer = nullptr;

	float fDetectRange = 600.f;
	float fShotRnage = 500.f;
	bool bDetected = false;

	float fWaitDyingTime = 0.f;
	void Die();

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	void SetCamera(Camera* camera) { _worldCamera = camera; }
	Vector2 GetFrontDir() { return vFrontDir; }
	void SetFrontDir(Vector2 dir) { vFrontDir = dir; }
	void TurnFrontDir() { vFrontDir.x = -vFrontDir.x; }
	bool GetDetected() { return bDetected; }

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionStayOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionEndOverlap(const CollisionInfo& info) override;

	void ProcessGroundCollision(const CollisionInfo& collisionInfo);
	void ProcessStairCollision(const CollisionInfo& collisionInfo, Vector2 oldPos);
	void ProcessWallCollision(const CollisionInfo& collisionInfo);
	void ProcessPlatformCollision(const CollisionInfo& collisionInfo);
	void ProcessCeilingCollision(const CollisionInfo& collisionInfo);

	bool ShouldPrioritizeStairCollision();
	bool ShouldSkipPlatformCollision();
	bool ShouldSkipStairCollision();

	virtual void ChangeState(int32 stateType) override;

	void SetPlayer(Player* player) { _targetPlayer = player; }
	bool PlayerIsBack();
	bool DetectPlayer(float radius, float angleStart, float angleEnd);
	bool InShotRange();
	bool IsBlockedObstacles();

	void DrawSight(HDC hdc, int centerX, int centerY, int radius, float startAngle, float endAngle);

	void ShotBullet();

	virtual void TakeDamage(Actor* damageCauser, const Vector2& attackDirection) override;
	virtual Vector2 GetNewPos() override;
};