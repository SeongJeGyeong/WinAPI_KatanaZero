#include "pch.h"
#include "Bullet.h"
#include "Managers/ResourceManager.h"
#include "Resources/Sprite.h"
#include "Game/Game.h"
#include "Components/Colliders/Collider.h"
#include "Components/EffectorComponent.h"
#include "Managers/SoundManager.h"
#include "Managers/TimeManager.h"

void Bullet::Init(Vector2 pos, Vector2 dir, float length, float radian)
{
	Super::Init(pos);
	vDirection = dir;
	fLength = length;
	fRadian = radian;

	CreateCollider<MovingLineCollisionShape>(ECollisionLayer::ENEMY_HITBOX, length, radian);

	_sprite = ResourceManager::GetInstance()->GetSprite("spr_bullet");
	SetIsActive(true);
}

void Bullet::Update(float deltaTime)
{
	if (bIsDead) return;
	if (TimeManager::GetInstance()->GetHitStop()) return;

	fLifeTime += deltaTime;
	if (fLifeTime >= 0.5f) bIsDead = true;

	Vector2 pos = GetPos();
	vNewPos = pos + vDirection * 1000.f * deltaTime;
}

void Bullet::PostUpdate(float deltaTime)
{
	if (bIsDead) return;
	SetPos(vNewPos);
}

void Bullet::Render(HDC hdc)
{
	if (bIsDead) return;
	Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(GetPos());
	_sprite->RenderRotatedSpriteBitBlt(hdc, screenPos, fRadian, 1.f, 0, false);

	Super::Render(hdc);
}

void Bullet::TakeDamage(Actor* damageCauser, const Vector2& attackDirection)
{
	if (bWasHit) return;
	bWasHit = true;
	fLifeTime = 0.f;
	vDirection = { -vDirection.x, -vDirection.y };
	SoundManager::GetInstance()->PlaySFX("sound_slash_bullet");
}

void Bullet::OnCollisionBeginOverlap(const CollisionInfo& info)
{
	bIsDead = true;
}