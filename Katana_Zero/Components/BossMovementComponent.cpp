#include "pch.h"
#include "BossMovementComponent.h"
#include "../Objects/Actors/Actor.h"
#include "../Managers/TimeManager.h"

void BossMovementComponent::InitComponent(Actor* owner)
{
    SetOwner(owner);
}

void BossMovementComponent::UpdateComponent(float deltaTime)
{
    ApplyPhysics(deltaTime);
    vNewPos = GetPos() + (vVelocity * deltaTime);
}

void BossMovementComponent::RenderComponent(HDC hdc)
{
}

void BossMovementComponent::ApplyPhysics(float deltaTime)
{
    if (bIsLunge)
    {
        vVelocity.y += vGravity.y * deltaTime;
        return;
    }

    // 중력의 방향 단위 벡터와 속도를 내적하여, 중력 방향의 값만 가진 벡터를 얻음.
    Vector2 normalGravity = vGravity.GetNormalize();
    float GravityLength = vVelocity.Dot(normalGravity);

    if (GetOwner()->GetWasHit())
    {
        vAcceleration += vGravity;
    }
    else if (bOnGround)
    {
        vVelocity.y = 0.f;
        GravityLength = 0.f;
    }
    else
    {
        // 중력 가속도 적용
        vAcceleration += vGravity;
    }

    // 가속도는 속도(velocity)를 변화시킨다.
    vVelocity += vAcceleration * deltaTime;

    float upFactor = 1000.f;
    float sideFactor = 1000.f;

    Vector2 gravityVector = normalGravity * GravityLength;
    Vector2 sideVec = vVelocity - gravityVector;
    float sideLength = sideVec.Length();

    if (GravityLength > upFactor) gravityVector = normalGravity * upFactor;

    if (sideLength > sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;
    else if (sideLength < -sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;

    float friction = 0.8f;
    if ((GetOwner()->GetWasHit() && !bOnGround) || GetOwner()->GetCurrentState() == EBossState::BOSS_LUNGE) friction = 0.98f;

    float fps = (float)TimeManager::GetInstance()->GetFps();
    sideVec *= pow(friction, deltaTime / (1.0f / fps));;
    if (sideVec.Length() < 1.0f)
        sideVec = Vector2(0.f, 0.f);

    // 최종적인 velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;
}

void BossMovementComponent::ParabolicJump(const Vector2& targetPos)
{
    bIsLunge = true;
    bOnGround = false;

    // 초기 속도 계산
    vVelocity = CalculateParabolicVelocity(GetPos(), targetPos);
    vAcceleration = Vector2(0.f, 0.f);
}

Vector2 BossMovementComponent::CalculateParabolicVelocity(const Vector2& startPos, const Vector2& targetPos)
{
    Vector2 initialVelocity;

    float dx = targetPos.x - startPos.x;
    float dy = targetPos.y - startPos.y;
    float direction = (dx >= 0.f) ? 1.f : -1.f;
    dx = fabsf(dx);

    const float g = vGravity.y;

    // 목표까지 일정한 수평 속도를 기준으로 시간 계산
    float time = dx / 1000.f;

    // 타임 범위 클램프
    if (time < 0.3f) time = 0.3f;
    if (time > 0.5f) time = 0.5f;

    // 수평 속도
    initialVelocity.x = direction * (dx / time);

    // 수직 속도
    initialVelocity.y = (dy - 0.5f * g * time * time) / time;
    return initialVelocity;
}