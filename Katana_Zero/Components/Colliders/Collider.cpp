#include "pch.h"
#include "Collider.h"

Collider::Collider()
	: iColliderId(GenerateID())
{
}

Collider::~Collider()
{
	SAFE_DELETE(_collisionShape);
}

void Collider::UpdateComponent(float deltaTime)
{
	if (_collisionShape)
	{
		_collisionShape->Update(this, deltaTime);
	}
}

void Collider::RenderComponent(HDC hdc)
{
}

