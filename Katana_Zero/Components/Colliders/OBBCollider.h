#pragma once
#include "Collider.h"

class OBBCollider : public Collider
{
	using Super = Collider;
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	Vector2 vCenter;

public:
	void InitOBB(float width, float height, float rotation);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	pair<Vector2, Vector2> GetAxes() const;
	vector<Vector2> GetVertices() const;

	Vector2 GetCenter() { return vCenter; }
	void SetCenter(Vector2 center) { vCenter = center; }

	pair<float, float> ProjectionToAxis(const Vector2& axis) const;
};

