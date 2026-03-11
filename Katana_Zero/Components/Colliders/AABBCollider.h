#pragma once
#include "Collider.h"

class AABBCollider : public Collider
{
	using Super = Collider;
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

public:
	void InitAABB(float width, float height);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	Vector2 GetAABBMin();
	Vector2 GetAABBMax();
	Vector2 GetCenter() { return GetPos(); }

	RECT GetRect();

	virtual void ChangeSize(float width, float height) override;
};

