#pragma once

class Actor;

class Component
{
public:

	enum EComponentPriority : uint32
	{
		INPUT,
		CAMERA,
		MOVEMENT,
		ANIMATION,
		EFFECT,
		DEFAULT
	};

	virtual ~Component() {}

	static size_t StaticClass()
	{
		// id 변수의 주소값을 고유 키값처럼 사용
		static char id = 0;
		return (size_t)&id;
	}

private:
	Actor* _owner = nullptr;
	uint32 id;

public:
	virtual void InitComponent() {};
	virtual void UpdateComponent(float deltaTime) = 0;
	virtual void RenderComponent(HDC hdc) = 0;

	Actor* GetOwner() const { return _owner; }
	void SetOwner(Actor* owner) { _owner = owner; }
	Vector2 GetPos() const;

	virtual void print(){}

	virtual uint32 GetPriority() { return EComponentPriority::DEFAULT; }
};

