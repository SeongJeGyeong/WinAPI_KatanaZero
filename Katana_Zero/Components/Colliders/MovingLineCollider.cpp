#include "pch.h"
#include "MovingLineCollider.h"
#include "Game/Game.h"

void MovingLineCollider::InitLine(float length, float radian)
{
	MovingLineCollisionShape* shape = new MovingLineCollisionShape();
	shape->Init(length, radian);
	SetCollisionShape(shape);
}

void MovingLineCollider::UpdateComponent(float deltaTime)
{
	Super::UpdateComponent(deltaTime);
}

void MovingLineCollider::RenderComponent(HDC hdc)
{
	if (!Game::GetInstance()->GetDebugMode()) return;
	Super::RenderComponent(hdc);

	Vector2 screenStart = Game::GetInstance()->ConvertCurSceneScreenPos(GetStartPoint());
	Vector2 screenEnd = Game::GetInstance()->ConvertCurSceneScreenPos(GetEndPoint());

	HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);

	MoveToEx(hdc, screenStart.x, screenStart.y, nullptr);
	LineTo(hdc, screenEnd.x, screenEnd.y);

	SelectObject(hdc, oldPen);
	DeleteObject(pen);
}