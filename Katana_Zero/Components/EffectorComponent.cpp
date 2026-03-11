#include "pch.h"
#include "EffectorComponent.h"
#include "../Managers/ResourceManager.h"
#include "../Resources/Sprite.h"
#include "../Objects/Actors/Actor.h"
#include "../Game/Game.h"
#include "../Managers/TimeManager.h"

void EffectorComponent::InitComponent()
{

}

void EffectorComponent::UpdateComponent(float deltaTime)
{
	for (auto it = _spawnList.begin(); it != _spawnList.end();)
	{
		(*it).fElapsedTime += deltaTime;

		if ((*it).fElapsedTime >= (*it).fDuration)
		{
			(*it).fElapsedTime = 0.f;
			if ((*it).iCurrentFrame == (*it).iFrameCount - 1)
			{
				if ((*it).bLoop)
				{
					(*it).iCurrentFrame = 0;
				}
				else
				{
					it = _spawnList.erase(it);
					continue;
				}
			}
			else
			{
				(*it).iCurrentFrame += 1;
			}
		}

		++it;
	}

	for (auto it = _afterImageList.begin(); it != _afterImageList.end();)
	{
		it->fAlpha -= 510.f * TimeManager::GetInstance()->GetConstDeltaTime();
		if (it->fAlpha <= 0.f)
		{
			it = _afterImageList.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void EffectorComponent::RenderComponent(HDC hdc)
{
	RenderConstEffects(hdc);
	RenderAfterImage(hdc);
	RenderEffects(hdc);
}

void EffectorComponent::UpdateConstantEffect(float deltaTime)
{
	for (auto it = _constSpawnList.begin(); it != _constSpawnList.end();)
	{
		(*it).fElapsedTime += deltaTime;

		if ((*it).fElapsedTime >= (*it).fDuration)
		{
			(*it).fElapsedTime = 0.f;
			if ((*it).iCurrentFrame >= (*it).iFrameCount)
			{
				if ((*it).bLoop)
				{
					(*it).iCurrentFrame = 0;
				}
				else
				{
					it = _constSpawnList.erase(it);
					continue;
				}
			}
			else
			{
				(*it).iCurrentFrame += 1;
			}
		}

		++it;
	}
}

void EffectorComponent::RenderEffects(HDC hdc)
{
	for (const SpawnInfo& info : _spawnList)
	{
		Vector2 Pos = info.bAttached ? GetPos() + info.vPos : info.vPos;
		Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(Pos);
		if (info.fRadAngle == 0.f)
		{
			ResourceManager::GetInstance()->GetSprite(info.effectName)->RenderStretchedSprite(hdc, screenPos, info.iCurrentFrame, info.bIsFlipped);
		}
		else
		{
			ResourceManager::GetInstance()->GetSprite(info.effectName)->RenderRotatedSprite(hdc, screenPos, info.fRadAngle, info.fScale, info.iCurrentFrame, info.bIsFlipped);
		}
	}
}

void EffectorComponent::RenderConstEffects(HDC hdc)
{
	for (const SpawnInfo& constInfo : _constSpawnList)
	{
		Vector2 Pos = constInfo.bAttached ? GetPos() + constInfo.vPos : constInfo.vPos;
		Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(Pos);
		if (constInfo.fRadAngle == 0.f)
		{
			ResourceManager::GetInstance()->GetSprite(constInfo.effectName)->RenderStretchedSprite(hdc, screenPos, constInfo.iCurrentFrame, constInfo.bIsFlipped);
		}
		else
		{
			ResourceManager::GetInstance()->GetSprite(constInfo.effectName)->RenderRotatedSprite(hdc, screenPos, constInfo.fRadAngle, constInfo.fScale, constInfo.iCurrentFrame, constInfo.bIsFlipped);
		}
	}
}

void EffectorComponent::RenderAfterImage(HDC hdc)
{
	for (const AfterImageInfo& info : _afterImageList)
	{
		Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(info.vPos);
		if (info.fRadian == 0.f)
		{
			ResourceManager::GetInstance()->GetSprite(info.effectName)->RenderStretchedSprite(
				hdc, 
				screenPos, 
				info.iFrameCount, 
				info.isFlipped, 
				info.fAlpha
			);
		}
		else
		{
			ResourceManager::GetInstance()->GetSprite(info.effectName)->RenderRotatedSprite(
				hdc, 
				screenPos, 
				info.fRadian, 
				1.f, 
				0, 
				info.isFlipped, 
				info.fAlpha
			);
		}
	}
}

void EffectorComponent::Destroy()
{

}

void EffectorComponent::PlayEffect(string name, bool flipped, float radian, float scale, bool attached, Vector2 pos)
{
	Sprite* sprite = ResourceManager::GetInstance()->GetSprite(name);
	if (sprite == nullptr) return;

	SpawnInfo info;
	info.effectName = name;
	info.iFrameCount = sprite->GetFrameCount();
	info.fDuration = sprite->GetDuration();
	info.bLoop = sprite->GetIsLoop();
	info.iCurrentFrame = 0;
	info.bIsFlipped = flipped;
	info.fRadAngle = radian;
	info.fScale = scale;
	info.fElapsedTime = 0.f;
	info.bAttached = attached;
	info.vPos = pos;

	_spawnList.push_back(info);
}

void EffectorComponent::PlayEffectForDuration(string name, bool flipped, float radian, float scale, bool attached, float duration, Vector2 pos)
{
}

void EffectorComponent::PlayConstantEffect(string name, bool flipped, float radian, float scale, bool attached, Vector2 pos)
{
	Sprite* sprite = ResourceManager::GetInstance()->GetSprite(name);
	if (sprite == nullptr) return;

	SpawnInfo info;
	info.effectName = name;
	info.iFrameCount = sprite->GetFrameCount();
	info.fDuration = sprite->GetDuration();
	info.bLoop = sprite->GetIsLoop();
	info.iCurrentFrame = 0;
	info.bIsFlipped = flipped;
	info.fRadAngle = radian;
	info.fScale = scale;
	info.fElapsedTime = 0.f;
	info.bAttached = attached;
	info.vPos = pos;

	_constSpawnList.push_back(info);
}

void EffectorComponent::PlayAfterImage(string name, Vector2 pos, int32 frameCount, bool flipped, float radian, COLORREF tintColor)
{
	Sprite* sprite = ResourceManager::GetInstance()->GetSprite(name);
	if (sprite == nullptr) return;

	AfterImageInfo info;
	info.effectName = name;
	info.vPos = pos;
	info.iFrameCount = frameCount;
	info.isFlipped = flipped;
	info.fAlpha = 100.f;
	info.fRadian = radian;
	info.tintColor;

	_afterImageList.push_back(info);
}

void EffectorComponent::RemoveLoopEffect(string name, bool constEffect)
{
	if (constEffect)
	{
		for (auto it = _constSpawnList.begin(); it != _constSpawnList.end(); ++it)
		{
			if (it->effectName == name)
			{
				_constSpawnList.erase(it);
				return;
			}
		}
	}
	else
	{
		for (auto it = _spawnList.begin(); it != _spawnList.end(); ++it)
		{
			if (it->effectName == name)
			{
				_spawnList.erase(it);
				return;
			}
		}
	}
}
