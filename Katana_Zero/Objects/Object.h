#pragma once

class Object
{
public:
	Object() = default;
	virtual ~Object(){}

protected:
	Vector2 vPos;
	bool bIsActive = false;

protected:
	virtual void Init() { bIsActive = false; }
	virtual void Update(float deltaTime) = 0;
	virtual void Render(HDC hdc) = 0;

public:
	Vector2 GetPos() { return vPos; }
	void SetPos(Vector2 pos) { vPos = pos; }

	bool GetIsActive() { return bIsActive; }
	void SetIsActive(bool active) { bIsActive = active; }
};

