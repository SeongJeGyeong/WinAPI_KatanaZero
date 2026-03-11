#pragma once
#include "Actor.h"

class Sprite;

class Bullet : public Actor
{
	using Super = Actor;
private:
	float fLifeTime = 0.f;
	Vector2 vDirection;
	float fLength;
	float fRadian;
	Sprite* _sprite = nullptr;

	Vector2 vNewPos;

public:
	virtual void Init(Vector2 pos, Vector2 dir, float length, float radian);
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	virtual void TakeDamage(Actor* damageCauser, const Vector2& attackDirection) override;

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) override;
};

