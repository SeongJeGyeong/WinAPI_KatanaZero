#include "pch.h"
#include "OBBCollider.h"
#include "Game/Game.h"

void OBBCollider::InitOBB(float width, float height, float rotation)
{
	OBBCollisionShape* shape = new OBBCollisionShape();
	shape->Init(width, height, rotation);
	SetCollisionShape(shape);
}

void OBBCollider::UpdateComponent(float deltaTime)
{
	Super::UpdateComponent(deltaTime);
}

void OBBCollider::RenderComponent(HDC hdc)
{
	if (!Game::GetInstance()->GetDebugMode()) return;
	Super::RenderComponent(hdc);

	pair<Vector2, Vector2> axes = GetAxes();

	vector<Vector2> vertices = GetVertices();
	POINT points[4];
	for (int i = 0; i < 4; ++i)
	{
		points[i].x = vertices[i].x;
		points[i].y = vertices[i].y;
	}

	HPEN pen = CreatePen(PS_SOLID, 1, bOverlapped ? RGB(0, 0, 255) : RGB(255, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Polygon(hdc, points, 4);

	MoveToEx(hdc, GetPos().x, GetPos().y, nullptr);
	LineTo(hdc, GetPos().x + axes.first.x * 20, GetPos().y + axes.first.y * 20);

	MoveToEx(hdc, GetPos().x, GetPos().y, nullptr);
	LineTo(hdc, GetPos().x + axes.second.x * 20, GetPos().y + axes.second.y * 20);

	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldBrush);
	DeleteObject(pen);
}

// x, y 축 계산, 단위 벡터 반환
pair<Vector2, Vector2> OBBCollider::GetAxes() const
{
	OBBCollisionShape* shape = GetCollisionShape<OBBCollisionShape>();
	if (!shape) return { Vector2(1.f, 0.f), Vector2(0.f, 1.f) };

	float cosRotation = cosf(shape->GetRotation());
	float sinRotation = sinf(shape->GetRotation());

	return { Vector2(cosRotation, sinRotation),		// X축 방향
			 Vector2(-sinRotation, cosRotation) };	// Y축 방향(X축을 반시계방향으로 90도 회전)
}

// 각 정점 위치 계산
vector<Vector2> OBBCollider::GetVertices() const
{
	const OBBCollisionShape* shape = GetCollisionShape<OBBCollisionShape>();
	if (!shape)
	{
		return vector<Vector2> { Vector2(0.f, 0.f), Vector2(0.f, 0.f), Vector2(0.f, 0.f), Vector2(0.f, 0.f) };
	}

	float halfWidth = shape->GetWidth() / 2;
	float halfHeight = shape->GetHeight() / 2;

	Vector2 center = GetPos();
	float rotation = shape->GetRotation();

	Vector2 topL = Vector2(center.x - halfWidth, center.y - halfHeight);
	Vector2 topR = Vector2(center.x + halfWidth, center.y - halfHeight);
	Vector2 bottomL = Vector2(center.x - halfWidth, center.y + halfHeight);
	Vector2 bottomR = Vector2(center.x + halfWidth, center.y + halfHeight);

	return vector<Vector2> { topL.Rotate(rotation), bottomL.Rotate(rotation), bottomR.Rotate(rotation), topR.Rotate(rotation) };
}

// 축 투영
pair<float, float> OBBCollider::ProjectionToAxis(const Vector2& axis) const
{
	// 두 도형이 충돌했다면, 두 도형의 각각의 가장자리에 수직인 모든 벡터에 두 도형의 투영된 벡터들이 겹치게 된다.
	vector<Vector2> vertices = GetVertices();
	float mn = vertices[0].Dot(axis);
	float mx = mn;

	for (int32 i = 1; i < vertices.size(); ++i)
	{
		float projection = vertices[i].Dot(axis);
		mn = min(mn, projection);
		mx = max(mx, projection);
	}

	return { mn, mx };
}