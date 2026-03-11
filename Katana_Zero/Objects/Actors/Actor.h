#pragma once
#include "../Object.h"
#include "../../Utils/ComponentContainer.h"

class Texture;
class Collider;
class CollisionShape;

class Actor : public Object
{
	using Super = Object;
public:
	virtual ~Actor();

protected:
	Collider* _collider = nullptr;
	ComponentContainer _components;

	bool bWasHit = false;
	bool bIsDead = false;

public:
	virtual void Init(Vector2 pos);
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) {}
	virtual void Render(HDC hdc) override;

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) {}
	virtual void OnCollisionStayOverlap(const CollisionInfo& info) {}
	virtual void OnCollisionEndOverlap(const CollisionInfo& info) {}
	virtual int32 GetCurrentState() { return 0; }

	Collider* GetCollider() { return _collider; }
	bool GetWasHit() { return bWasHit; }

	void CreateColliderInternal(ECollisionLayer layer, CollisionShape* shape);
	void DestroyCollider();

	virtual void TakeDamage(Actor* damageCauser, const Vector2& attackDirection) {}

	virtual bool GetIsDead() { return bIsDead; }
	virtual void SetIsDead(bool isDead) { bIsDead = isDead; }
	virtual Vector2 GetNewPos() { return Vector2(0, 0); }

	virtual void AddForce(Vector2 force) {};

	template <typename T>
	T* GetComponent()
	{
		return _components.GetComponent<T>();
	}

	void UpdateAttachedComponents(float deltaTime)
	{
		_components.UpdateComponents(deltaTime);
	}

	template<typename TShape, typename... Args>
	void CreateCollider(ECollisionLayer layer, Args&&... args)
	{
		TShape* shape = new TShape();
		shape->Init(std::forward<Args>(args)...);

		CreateColliderInternal(layer, shape);
	}

	virtual void ChangeState(int32 stateType) {}
};

