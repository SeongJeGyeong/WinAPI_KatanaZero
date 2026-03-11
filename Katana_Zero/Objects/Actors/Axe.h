#pragma once
#include "Actor.h"

class Sprite;

class Axe : public Actor
{
	using Super = Actor;
public:
	~Axe(){}

	function<void()> OnReturnAxe;
private:
	Actor* _owner;

	Vector2 vDirection;
	float fRadian;
	Sprite* _sprite = nullptr;
	Vector2 vNewPos = {0.f, 0.f};

	bool bThrowOrSwing = true;
	bool bStuck = false;
	float fStuckTime = 0.f;
	float fThrowSpeed = 2000.f;
	float fThrowRotationSpeed = 30.f;

	float fSwingDistance = 100.f;
	float fSwingRotationSpeed = 15.f;
	float fSwingMovementSpeed = 300.f;
	bool bReachMaxDist = false;

public:
	virtual void Init(Vector2 pos, Actor* owner, Vector2 dir, bool throwOrSwing);
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	void ThrowUpdate(float deltaTime);
	void SwingUpdate(float deltaTime);

	virtual void TakeDamage(Actor* damageCauser, const Vector2& attackDirection) override;
	Vector2 GetNewPos() { return vNewPos; }

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionStayOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionEndOverlap(const CollisionInfo& info) override;
};

