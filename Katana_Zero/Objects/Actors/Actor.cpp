#include "pch.h"
#include "Actor.h"
#include "Managers/CollisionManager.h"
#include "Components/Colliders/OBBCollider.h"
#include "Components/Colliders/AABBCollider.h"
#include "Components/Colliders/LineCollider.h"
#include "Components/Colliders/MovingLineCollider.h"

Actor::~Actor()
{
	if (_collider) CollisionManager::GetInstance()->DeleteCollider(_collider);
	_components.ClearComponents();
}

void Actor::Init(Vector2 pos)
{
	Super::Init();
	Super::SetPos(pos);
	bIsActive = true;
}

void Actor::Update(float deltaTime)
{
}

void Actor::Render(HDC hdc)
{
	if (_collider) _collider->RenderComponent(hdc);
}

void Actor::CreateColliderInternal(ECollisionLayer layer, CollisionShape* shape)
{
	DestroyCollider();

	Collider* newCollider = nullptr;

	if (dynamic_cast<AABBCollisionShape*>(shape) != nullptr)
	{
		newCollider = new AABBCollider();
	}
	else if (dynamic_cast<OBBCollisionShape*>(shape) != nullptr)
	{
		newCollider = new OBBCollider();
	}
	else if (dynamic_cast<MovingLineCollisionShape*>(shape) != nullptr)
	{
		newCollider = new MovingLineCollider();
	}
	else if (dynamic_cast<LineCollisionShape*>(shape) != nullptr)
	{
		newCollider = new LineCollider();
	}
	else
	{
		SAFE_DELETE(shape);
		return;
	}

	newCollider->SetOwner(this);
	newCollider->SetCollisionLayer(layer);
	newCollider->SetCollisionShape(shape);
	_collider = newCollider;

	CollisionManager::GetInstance()->AddCollider(_collider);
}

void Actor::DestroyCollider()
{
	if (_collider == nullptr)
		return;

	CollisionManager::GetInstance()->DeleteCollider(_collider);

	SAFE_DELETE(_collider);
}