#include "pch.h"
#include "Axe.h"
#include "Game/Game.h"
#include "Managers/ResourceManager.h"
#include "Resources/Sprite.h"
#include "Components/Colliders/Collider.h"
#include "Managers/SoundManager.h"
#include "Managers/TimeManager.h"
#include "Components/EffectorComponent.h"

void Axe::Init(Vector2 pos, Actor* owner, Vector2 dir, bool throwOrSwing)
{
	Super::Init(pos);

	CreateCollider<AABBCollisionShape>(ECollisionLayer::ENEMY_HITBOX, 70, 70);
	_sprite = ResourceManager::GetInstance()->GetSprite("spr_kissyface_axe");
	vDirection = dir;
	_owner = owner;
	bThrowOrSwing = throwOrSwing;
	fRadian = 0.f;
	fSwingDistance = 100.f;
	bReachMaxDist = false;

	_components.AddComponent<EffectorComponent>();
	EffectorComponent* effector = _components.GetComponent<EffectorComponent>();
	if (effector != nullptr)
	{
		effector->InitComponent();
		effector->SetOwner(this);
	}
}

void Axe::Update(float deltaTime)
{
	_components.UpdateComponents(deltaTime);
	if (!bIsActive) return;
	if (TimeManager::GetInstance()->GetHitStop()) return;
	if (bThrowOrSwing)
	{
		ThrowUpdate(deltaTime);
	}
	else
	{
		SwingUpdate(deltaTime);
	}
}

void Axe::PostUpdate(float deltaTime)
{
	if (!bIsActive || bStuck) return;
	SetPos(vNewPos);
}

void Axe::Render(HDC hdc)
{
	if (!bIsActive) return;

	Vector2 pos = GetPos();
	Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(pos);

	bool flip = false;
	if (bThrowOrSwing)
	{
		if (vDirection.x > 0) flip = true;
		_sprite->RenderRotatedSprite(hdc, screenPos, fRadian, 1.f, 0, flip);
	}
	else
	{
		_sprite->RenderRotatedSprite(hdc, screenPos, -fRadian + PI * 0.5f, 1.f, 0, flip);
	}

	_components.RenderComponents(hdc);
	Super::Render(hdc);
}

void Axe::ThrowUpdate(float deltaTime)
{
	if (bStuck)
	{
		fStuckTime += deltaTime;
		if (fStuckTime >= 0.5f)
		{
			bStuck = false;
			fStuckTime = 0.f;
			vDirection.x = -vDirection.x;
			SoundManager::GetInstance()->PlaySFX("sound_boss_kissyface_axereturn_01");
		}
		else
		{
			return;
		}
	}
	fRadian -= fThrowRotationSpeed * deltaTime;

	Vector2 pos = GetPos();
	vNewPos = pos + vDirection * fThrowSpeed * deltaTime;
	GetComponent<EffectorComponent>()->PlayAfterImage("spr_kissyface_axe", vNewPos, 0, (vDirection.x > 0), fRadian);
}

void Axe::SwingUpdate(float deltaTime)
{
	fRadian += fSwingRotationSpeed * deltaTime;
	if (fRadian > 2 * PI) fRadian -= 2 * PI;

	fSwingDistance = bReachMaxDist ? fSwingDistance - fSwingMovementSpeed * deltaTime : fSwingDistance + fSwingMovementSpeed * deltaTime;

	if (fSwingDistance >= 180.f) bReachMaxDist = true;

	float rotatedX = fSwingDistance * cosf(fRadian);
	float rotatedY = fSwingDistance * sinf(fRadian);

	Vector2 ownerPos = _owner->GetPos();
	vNewPos = { ownerPos.x + rotatedX, ownerPos.y - rotatedY };
	GetComponent<EffectorComponent>()->PlayAfterImage("spr_kissyface_axe", vNewPos, 0, false, -fRadian + PI * 0.5f);
}

void Axe::TakeDamage(Actor* damageCauser, const Vector2& attackDirection)
{
	if (!bIsActive || !bThrowOrSwing) return;
	bWasHit = true;
	vDirection = { -vDirection.x, -vDirection.y };
	SoundManager::GetInstance()->PlaySFX("sound_slash_bullet");
}

void Axe::OnCollisionBeginOverlap(const CollisionInfo& info)
{
	if (!bIsActive) return;

	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
		if (!bThrowOrSwing || bStuck) break;
		bStuck = true;
		SoundManager::GetInstance()->PlaySFX("sound_bullethit3");
		Game::GetInstance()->ShakeSceneCamera();
		break;
	case ECollisionLayer::WALL:
		if (!bThrowOrSwing || bStuck) break;
		bStuck = true;
		SoundManager::GetInstance()->PlaySFX("sound_bullethit3");
		Game::GetInstance()->ShakeSceneCamera();
		break;
	default:
		break;
	}
}

void Axe::OnCollisionStayOverlap(const CollisionInfo& info)
{
	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
		break;
	case ECollisionLayer::WALL:
		break;
	case ECollisionLayer::ENEMY:
		bIsActive = false;
		bWasHit = false;
		OnReturnAxe();
		break;
	default:
		break;
	}
}

void Axe::OnCollisionEndOverlap(const CollisionInfo& info)
{
}