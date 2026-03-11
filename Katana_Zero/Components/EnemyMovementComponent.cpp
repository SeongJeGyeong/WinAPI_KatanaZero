#include "pch.h"
#include "EnemyMovementComponent.h"
#include "../Objects/Actors/Actor.h"
#include "../Managers/TimeManager.h"

void EnemyMovementComponent::InitComponent(Actor* owner)
{
	SetOwner(owner);
}

void EnemyMovementComponent::UpdateComponent(float deltaTime)
{
    if (GetOwner()->GetWasHit())
    {
        float constDeltaTime = TimeManager::GetInstance()->GetConstDeltaTime();
        HitPhysics(constDeltaTime);
        vNewPos = GetPos() + (vVelocity * constDeltaTime);
    }
    else
    {
        ApplyPhysics(deltaTime);
        vNewPos = GetPos() + (vVelocity * deltaTime);
    }
}

void EnemyMovementComponent::RenderComponent(HDC hdc)
{
}

void EnemyMovementComponent::ApplyPhysics(float deltaTime)
{
    // 중력의 방향 단위 벡터와 속도를 내적하여, 중력 방향의 값만 가진 벡터를 얻음.
    Vector2 normalGravity = vGravity.GetNormalize();
    float GravityLength = vVelocity.Dot(normalGravity);

    if (bOnGround || bOnPlatform || bOnStair)
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

    //float fps = (float)TimeManager::GetInstance()->GetFps();
    float friction = 0.8f;
    if (GetOwner()->GetIsDead() && !(bOnGround || bOnPlatform || bOnStair)) friction = 0.98f;

    //sideVec *= pow(friction, deltaTime / (1.0f / fps));
    sideVec *= pow(friction, deltaTime / (1.0f / fMaxFrameRate));
    if (sideVec.Length() < 1.0f) sideVec = Vector2(0.f, 0.f);

    // 최종적인 velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;
}

void EnemyMovementComponent::HitPhysics(float deltaTime)
{
    float fps = (float)TimeManager::GetInstance()->GetFps();

    // 중력의 방향 단위 벡터와 속도를 내적하여, 중력 방향의 값만 가진 벡터를 얻음.
    Vector2 normalGravity = vGravity.GetNormalize();
    float GravityLength = vVelocity.Dot(normalGravity);

    if (bOnGround || bOnPlatform || bOnStair)
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

    float upFactor = 2000.f;
    float sideFactor = 2000.f;

    Vector2 gravityVector = normalGravity * GravityLength;
    Vector2 sideVec = vVelocity - gravityVector;
    float sideLength = sideVec.Length();

    if (GravityLength > upFactor) gravityVector = normalGravity * upFactor;

    if (sideLength > sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;
    else if (sideLength < -sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;

    float friction = 0.98f;

    sideVec *= pow(friction, deltaTime / (1.0f / fps));
    if (sideVec.Length() < 1.0f)
        sideVec = Vector2(0.f, 0.f);

    // 최종적인 velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;
    wstring str = std::format(L"velocity({0}, {1})\n", vVelocity.x, vVelocity.y);
    OutputDebugString(str.c_str());
}
