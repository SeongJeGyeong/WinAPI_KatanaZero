#include "pch.h"
#include "TimeManager.h"
#include "SoundManager.h"

void TimeManager::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&iFrequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&iPrevCount)); // CPU 클럭

	fConstantDeltaTime = 0.0f;
	fTimeScale = 1.0f;
	bSlowMotion = false;
	fTargetTimeScale = 1.0f;
	fTransitionSpeed = 2.0f;
	bIsTransitioning = false;

	srand(time(0));
}

void TimeManager::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	// 실제(월드) 시간 델타타임 (가변)
	fRealDeltaTime = (currentCount - iPrevCount) / static_cast<float>(iFrequency);
	iPrevCount = currentCount;

	// FPS 계산(렌더/루프 프레임 기준)
	iFrameCount++;
	fFrameTime += fRealDeltaTime;

	if (fFrameTime >= 1.0f)
	{
		iFps = iFrameCount;
		fFrameTime -= 1.0f;
		iFrameCount = 0;
	}
}

void TimeManager::FixedUpdate(float fixedDeltaTime)
{
	fConstantDeltaTime = fixedDeltaTime;

	if (bIsTransitioning)
	{
		float diff = fTargetTimeScale - fTimeScale;
		float moveAmount = fTransitionSpeed * fConstantDeltaTime;

		if (abs(diff) <= moveAmount)
		{
			fTimeScale = fTargetTimeScale;
			bIsTransitioning = false;
			bSlowMotion = (fTimeScale < 1.f);
			fMaskAlpha = bSlowMotion ? 255.f : 0.f;
		}
		else
		{
			fTimeScale += (diff > 0 ? moveAmount : -moveAmount);

			float transitionProgress =
				1.0f - (abs(diff) / abs(fTargetTimeScale - fPrevTimeScale));

			if (fTargetTimeScale >= 1.0f)  // 슬로모션 해제
				fMaskAlpha = 255.f * (1.0f - transitionProgress);
			else                           // 슬로모션 진입
				fMaskAlpha = 255.f * transitionProgress;
		}
	}

	if (_pause)
	{
		fDeltaTime = 0.f;
		return;
	}

	// 슬로모션 배터리/회복
	if (bSlowMotion)
	{
		fSlowMotionCountGauge += fConstantDeltaTime;
		if (fSlowMotionCountGauge >= 0.5f)
		{
			fSlowMotionCountGauge = 0.f;
			iSlowMotionBatteryCount--;
		}
		if (iSlowMotionBatteryCount <= 0)
			EndSlowMotion(2.f);
	}
	else if (iSlowMotionBatteryCount < 11)
	{
		fSlowMotionCountGauge += fConstantDeltaTime * 0.5f;
		if (fSlowMotionCountGauge >= 0.5f)
		{
			fSlowMotionCountGauge = 0.f;
			iSlowMotionBatteryCount++;
		}
	}

	// 히트스탑
	if (IsHitStop)
	{
		hitStopDuration += fConstantDeltaTime;
		if (hitStopDuration >= hitStopTime)
		{
			hitStopDuration = 0.f;
			IsHitStop = false;
		}
	}

	fDeltaTime = fConstantDeltaTime * fTimeScale;
}

void TimeManager::StartSlowMotion(float transitionSpeed)
{
	fPrevTimeScale = fTimeScale;
	fTargetTimeScale = 0.2f;
	fTransitionSpeed = transitionSpeed;
	bIsTransitioning = true;
	bSlowMotion = true;
	SoundManager::GetInstance()->SetGlobalSpeed(0.2f);
	SoundManager::GetInstance()->PlaySlowMotion(true);
}

void TimeManager::EndSlowMotion(float transitionSpeed)
{
	fPrevTimeScale = fTimeScale;
	fTargetTimeScale = 1.0f;
	fTransitionSpeed = transitionSpeed;
	bIsTransitioning = true;
	SoundManager::GetInstance()->SetGlobalSpeed(1.f, true);
	SoundManager::GetInstance()->PlaySlowMotion(false);
}

void TimeManager::InitSlowMotionBattery()
{
	fSlowMotionCountGauge = 0.f;
	iSlowMotionBatteryCount = 11;
}