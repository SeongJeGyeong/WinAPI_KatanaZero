#include "pch.h"
#include "CollisionShape.h"

void AABBCollisionShape::Init(float width, float height)
{
	fWidth = width;
	fHeight = height;
}

void AABBCollisionShape::Update(Collider* owner, float deltaTime)
{
}

void OBBCollisionShape::Init(float width, float height, float rotation)
{
	fWidth = width;
	fHeight = height;
	fRotation = rotation;
}

void OBBCollisionShape::Update(Collider* owner, float deltaTime)
{
}

void LineCollisionShape::Init(Vector2 start, Vector2 end)
{
	vStart = start;
	vEnd = end;
}

void LineCollisionShape::Update(Collider* owner, float deltaTime)
{
}

void MovingLineCollisionShape::Init(float length, float radian)
{
	fLength = length;
	fRadian = radian;
}

void MovingLineCollisionShape::Update(Collider* owner, float deltaTime)
{
}

Vector2 MovingLineCollisionShape::GetStartPoint(const Vector2& ownerPos) const
{
	float halfLength = fLength * 0.5f;
	float dx = cos(fRadian) * halfLength;
	float dy = sin(fRadian) * halfLength;

	return { ownerPos.x - dx, ownerPos.y - dy };
}

Vector2 MovingLineCollisionShape::GetEndPoint(const Vector2& ownerPos) const
{
	float halfLength = fLength * 0.5f;
	float dx = cos(fRadian) * halfLength;
	float dy = sin(fRadian) * halfLength;

	return { ownerPos.x + dx, ownerPos.y + dy };
}
