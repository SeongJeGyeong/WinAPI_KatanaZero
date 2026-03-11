#pragma once
#include "Actor.h"

template<typename EnumType>
class StateMachine;

class Player;
class Texture;

class Boss : public Actor
{
	using Super = Actor;
public:
	~Boss();

	function<void(Vector2, Actor*, Vector2, bool)> OnSpawnAxe;
	function<void()> OnDie;
private:
	StateMachine<EBossState>* _stateMachine;

	Vector2 vNewPos;
	Vector2 vFrontDir = { 1, 0 };

	Player* _targetPlayer = nullptr;

	Texture* _progressBackground = nullptr;
	Texture* _progressCurrent = nullptr;
	Texture* _progressPrev = nullptr;
	float fStruggleGauge = 0.f;
	float fPrevGauge = 0.f;
	float fDetectRange = 800.f;

	bool bCanStruggling = false;
	bool bAttack = false;

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	virtual int32 GetCurrentState() override;
	virtual void ChangeState(int32 stateType) override;

	void SetPlayer(Player* player) { _targetPlayer = player; }

	bool ToBattle();
	void StartStruggle();
	void EndStruggle();
	void Defeat();
	void EndFinishMotion();
	void Die();
	void UpdateDirection();
	void ThrowAxe();
	void ReturnAxe();
	void Lunge();
	void Hurt(Vector2 attackDir);

	virtual void TakeDamage(Actor* damageCauser, const Vector2& attackDirection) override;
	void SetWasHit(bool wasHit) { bWasHit = wasHit; }

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionStayOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionEndOverlap(const CollisionInfo& info) override;

	void ProcessGroundCollision(const CollisionInfo& collisionInfo);
	void ProcessWallCollision(const CollisionInfo& collisionInfo);

	Vector2 GetFrontDir() { return vFrontDir; }
	float CheckDistance();

	void SetAttack(bool attack) { bAttack = attack; }
	virtual Vector2 GetNewPos() override;

	bool TargetNotFound();
	bool GetCanStruggling() { return bCanStruggling; }
	void SetCanStruggling(bool struggling);
};

