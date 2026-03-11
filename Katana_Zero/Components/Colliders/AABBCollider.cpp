#include "pch.h"
#include "AABBCollider.h"
#include "Game/Game.h"

void AABBCollider::InitAABB(float width, float height)
{
	AABBCollisionShape* shape = new AABBCollisionShape();
	shape->Init(width, height);
	SetCollisionShape(shape);
}

void AABBCollider::UpdateComponent(float deltaTime)
{
	Super::UpdateComponent(deltaTime);
}

void AABBCollider::RenderComponent(HDC hdc)
{
	if (!Game::GetInstance()->GetDebugMode()) return;
	Super::RenderComponent(hdc);

	Vector2 pos = Game::GetInstance()->ConvertCurSceneScreenPos(GetPos());
	int32 left = pos.x - GetWidth() * 0.5f;
	int32 right = pos.x + GetWidth() * 0.5f;
	int32 top = pos.y - GetHeight() * 0.5f;
	int32 bottom = pos.y + GetHeight() * 0.5f;

	HPEN redPen = CreatePen(PS_SOLID, 2, (bOverlapped) ? RGB(0, 255, 0) : RGB(255, 0, 0));

	HPEN oldPen = (HPEN)SelectObject(hdc, redPen);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, left, top, right, bottom);
	(HPEN)SelectObject(hdc, oldPen);
	DeleteObject(redPen);
}

Vector2 AABBCollider::GetAABBMin()
{
	return { GetPos().x - GetWidth() / 2, GetPos().y - GetHeight() / 2 };
}

Vector2 AABBCollider::GetAABBMax()
{
	return { GetPos().x + GetWidth() / 2 , GetPos().y + GetHeight() / 2 };
}

RECT AABBCollider::GetRect()
{
	RECT rect;

	Vector2 pos = GetPos();

	rect.left = pos.x - GetWidth() * 0.5f;
	rect.right = pos.x + GetWidth() * 0.5f;
	rect.top = pos.y - GetHeight() * 0.5f;
	rect.bottom = pos.y + GetHeight() * 0.5f;

	return rect;
}

void AABBCollider::ChangeSize(float width, float height)
{
	if (AABBCollisionShape* shape = GetCollisionShape<AABBCollisionShape>())
	{
		shape->Init(width, height);
	}
}
