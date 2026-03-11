#pragma once
#include "Components/Component.h"
#include "CollisionShape.h"

class Collider : public Component
{
public:
	Collider();
	virtual ~Collider();

	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	const uint64 iColliderId;

	bool bCollideActive;
	CollisionShape* _collisionShape = nullptr;

protected:
	ECollisionLayer eCollisionLayer;
	bool bOverlapped = false;
	bool bBlocked = false;

private:
	uint64 GenerateID()
	{
		static uint64 id = 0;
		return id++;
	}

public:
	virtual void InitComponent() {};
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void SetCollisionShape(CollisionShape* shape) { _collisionShape = shape; }

	template<typename T>
	T* GetCollisionShape() const
	{
		return dynamic_cast<T*>(_collisionShape);
	}

	ECollisionLayer GetCollisionLayer() { return eCollisionLayer; }
	void SetCollisionLayer(ECollisionLayer layer) { eCollisionLayer = layer; }
	EColliderType GetColliderType() { return _collisionShape->GetType(); }

	virtual float GetWidth() { return _collisionShape ? _collisionShape->GetWidth() : 0.f; }
	virtual float GetHeight() { return _collisionShape ? _collisionShape->GetHeight() : 0.f; }
	virtual Vector2 GetStartPoint() { return _collisionShape ? _collisionShape->GetStartPoint(GetPos()) : Vector2(); }
	virtual Vector2 GetEndPoint() { return _collisionShape ? _collisionShape->GetEndPoint(GetPos()) : Vector2(); }
	virtual float GetLength() { return _collisionShape ? _collisionShape->GetLength() : 0.f; }
	virtual float GetRadian() { return _collisionShape ? _collisionShape->GetRadian() : 0.f; }

	bool GetOverlapped() { return bOverlapped; }
	void SetOverlapped(bool overlap) { bOverlapped = overlap; }
	bool GetBlocked() { return bBlocked; }
	void SetBlocked(bool block) { bBlocked = block; }

	uint64 GetColliderId() { return iColliderId; }

	virtual void ChangeSize(float width, float height) {}
};

