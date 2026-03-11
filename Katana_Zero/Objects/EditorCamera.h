#pragma once
#include "Object.h"
class EditorCamera : public Object
{
	using Super = Object;
private:
	float fCameraWidth;
	float fCameraHeight;
	int32 iWorldSizeX;
	int32 iWorldSizeY;

public:
	virtual void Init() override;
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;
	Vector2 ConvertScreenPos(Vector2 worldPos);
	Vector2 ConvertWorldPos(Vector2 screenPos);
	POINT ConvertScreenPos(POINT worldPos);
	POINT ConvertWorldPos(POINT screenPos);

	void SetWorldSize(int32 x, int32 y) { iWorldSizeX = x; iWorldSizeY = y; }

	Vector2 GetCameraSize() { return { fCameraWidth , fCameraHeight }; }
};

