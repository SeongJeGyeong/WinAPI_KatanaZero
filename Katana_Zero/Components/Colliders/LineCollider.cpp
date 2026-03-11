#include "pch.h"
#include "LineCollider.h"
#include "Game/Game.h"

void LineCollider::InitLine(Vector2 start, Vector2 end)
{
	LineCollisionShape* shape = new LineCollisionShape();
	shape->Init(start, end);
	SetCollisionShape(shape);
}

void LineCollider::UpdateComponent(float deltaTime)
{
	Super::UpdateComponent(deltaTime);
}

void LineCollider::RenderComponent(HDC hdc)
{
	if (!Game::GetInstance()->GetDebugMode()) return;
	Super::RenderComponent(hdc);

	HPEN pen = CreatePen(PS_SOLID, 2, (bOverlapped) ? RGB(0, 255, 0) : RGB(255, 0, 255));
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);

	Vector2 screenStart = Game::GetInstance()->ConvertCurSceneScreenPos(GetStartPoint());
	Vector2 screenEnd = Game::GetInstance()->ConvertCurSceneScreenPos(GetEndPoint());
	MoveToEx(hdc, screenStart.x, screenStart.y, nullptr);
	LineTo(hdc, screenEnd.x, screenEnd.y);
	SelectObject(hdc, oldPen);
	DeleteObject(pen);
}