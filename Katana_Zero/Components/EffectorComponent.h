#pragma once
#include "Component.h"

class Sprite;

struct SpawnInfo
{
	string effectName;
	Vector2 vPos;

	bool bLoop;
	int32 iFrameCount;
	int32 iCurrentFrame;
	float fDuration;

	float fElapsedTime;
	float fRadAngle;
	float fScale;
	bool bAttached;
	bool bIsFlipped;
};

struct AfterImageInfo
{
	string effectName;
	Vector2 vPos;
	int32 iFrameCount;
	bool isFlipped;
	float fAlpha = 200.f;
	float fRadian = 0.f;
	COLORREF tintColor = RGB(255, 255, 255);  // 색상 틴트
};

class EffectorComponent : public Component
{
	using Super = Component;
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	vector<SpawnInfo> _spawnList;
	vector<SpawnInfo> _constSpawnList;	// 항상 일정하게 플레이해야 하는 이펙트
	vector<AfterImageInfo> _afterImageList;	// 잔상

public:
	virtual void InitComponent() override;
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void UpdateConstantEffect(float deltaTime);

	void RenderEffects(HDC hdc);
	void RenderConstEffects(HDC hdc);
	void RenderAfterImage(HDC hdc);

	void Destroy();

	void PlayEffect(string name, bool flipped, float radian, float scale, bool attached, Vector2 pos = Vector2(0, 0));
	void PlayEffectForDuration(string name, bool flipped, float radian, float scale, bool attached, float duration, Vector2 pos = Vector2(0, 0));
	void PlayConstantEffect(string name, bool flipped, float radian, float scale, bool attached, Vector2 pos = Vector2(0, 0));
	void PlayAfterImage(string name, Vector2 pos, int32 frameCount, bool flipped, float radian = 0.f, COLORREF tintColor = RGB(255, 255, 255));
	void RemoveLoopEffect(string name, bool constEffect);

	virtual uint32 GetPriority() override { return EComponentPriority::EFFECT; }
	void ClearSpawnList() { _spawnList.clear(); }
};

