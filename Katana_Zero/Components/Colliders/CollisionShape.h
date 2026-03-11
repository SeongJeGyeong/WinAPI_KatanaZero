#pragma once

class Collider;

class CollisionShape
{
public:
	virtual ~CollisionShape() = default;

	virtual void Update(Collider* owner, float deltaTime) {}

	virtual float GetWidth() const { return 0.f; }
	virtual float GetHeight() const { return 0.f; }
	virtual Vector2 GetStartPoint(const Vector2& ownerPos) const { return Vector2(); }
	virtual Vector2 GetEndPoint(const Vector2& ownerPos) const { return Vector2(); }
	virtual float GetLength() const { return 0.f; }
	virtual float GetRadian() const { return 0.f; }
	virtual EColliderType GetType() const = 0;
};

class AABBCollisionShape : public CollisionShape
{
private:
	float fWidth = 0.f;
	float fHeight = 0.f;

public:
	void Init(float width, float height);
	virtual void Update(Collider* owner, float deltaTime) override;

	virtual float GetWidth() const override { return fWidth; }
	virtual float GetHeight() const override { return fHeight; }
	virtual EColliderType GetType() const override { return EColliderType::AABB; }
};

class OBBCollisionShape : public CollisionShape
{
private:
	float fWidth = 0.f;
	float fHeight = 0.f;
	float fRotation = 0.f;

public:
	void Init(float width, float height, float rotation);
	virtual void Update(Collider* owner, float deltaTime) override;

	virtual float GetWidth() const override { return fWidth; }
	virtual float GetHeight() const override { return fHeight; }
	float GetRotation() const { return fRotation; }
	virtual EColliderType GetType() const override { return EColliderType::OBB; }
};

class LineCollisionShape : public CollisionShape
{
private:
	Vector2 vStart;
	Vector2 vEnd;

public:
	void Init(Vector2 start, Vector2 end);
	virtual void Update(Collider* owner, float deltaTime) override;

	virtual Vector2 GetStartPoint(const Vector2& ownerPos) const override { return vStart; }
	virtual Vector2 GetEndPoint(const Vector2& ownerPos) const override { return vEnd; }
	virtual EColliderType GetType() const override { return EColliderType::LINE; }
};

class MovingLineCollisionShape : public CollisionShape
{
private:
	float fLength = 0.f;
	float fRadian = 0.f;

public:
	void Init(float length, float radian);
	virtual void Update(Collider* owner, float deltaTime) override;

	virtual Vector2 GetStartPoint(const Vector2& ownerPos) const override;
	virtual Vector2 GetEndPoint(const Vector2& ownerPos) const override;
	virtual float GetLength() const override { return fLength; }
	virtual float GetRadian() const override { return fRadian; }
	virtual EColliderType GetType() const override { return EColliderType::LINE; }
};