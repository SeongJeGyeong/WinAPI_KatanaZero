#include "pch.h"
#include "CollisionManager.h"
#include "Components/Colliders/Collider.h"
#include "Components/Colliders/OBBCollider.h"
#include "Components/Colliders/AABBCollider.h"
#include "Components/Colliders/LineCollider.h"
#include "Components/Colliders/MovingLineCollider.h"
#include "Objects/Actors/Actor.h"
#include "Objects/Actors/Player.h"
#include "Game/Game.h"
#include "Components/PlayerMovementComponent.h"
#include "Objects/Actors/Enemy.h"
#include "Components/EnemyMovementComponent.h"
#include "Objects/Actors/Axe.h"

void CollisionManager::Init()
{
	// Player Collision Layer
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::GROUND, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::WALL, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::CEILING, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::PLATFORM, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::STAIR, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::ENEMY_HITBOX, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::PORTAL, ECollisionResponse::C_BLOCK, true);

	// Enemy Collision Layer
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::GROUND, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::WALL, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::CEILING, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::PLATFORM, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::STAIR, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::PLAYER_HITBOX, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::ENEMY_HITBOX, ECollisionResponse::C_BLOCK, true);

	// 총알, 도끼 반사용
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::ENEMY, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::PLAYER_HITBOX, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::GROUND, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::WALL, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::CEILING, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::STAIR, ECollisionResponse::C_BLOCK, true);

	RebuildLayerCache();
}

void CollisionManager::RebuildLayerCache()
{
	for (int32 receive = 0; receive < (int32)ECollisionLayer::END; ++receive)
	{
		_cachedSendLayers[receive].clear();
		for (int32 send = 0; send < (int32)ECollisionLayer::END; ++send)
		{
			if (COLLISION_BIT_MASK_BLOCK[receive] & (uint16)(1 << send))
				_cachedSendLayers[receive].push_back(send);
		}
	}
}

void CollisionManager::Update()
{
	//PLAYER	0
	//ENEMY		1
	//GROUND	2
	//PLATFORM	3
	//WALL		4
	//CEILING	5
	//STAIR		6
	//PLAYER_HITBOX
	//ENEMY_HITBOX
	//PORTAL
	//END

	for (int32 receiveLayer = 0; receiveLayer < (int32)ECollisionLayer::END; ++receiveLayer)
	{
		if (_colliderList[receiveLayer].empty()) continue;

		for (Collider* receiveCollider : _colliderList[receiveLayer])
		{
			if (!receiveCollider->GetOwner()->GetIsActive()) continue;

			for (int32 sendLayer : _cachedSendLayers[receiveLayer])
			{
				if (_colliderList[sendLayer].empty()) continue;

				for (Collider* sendCollider : _colliderList[sendLayer])
				{
					if (!sendCollider->GetOwner()->GetIsActive()) continue;

					const uint64 mapKey = CollisionMapID(receiveCollider->GetColliderId(), sendCollider->GetColliderId()).ToUInt64();

					auto iter = _collisionMap.find(mapKey);
					const bool isFirstContact = (iter == _collisionMap.end());

					// 이미 이번 프레임에 처리된 충돌 쌍이면 스킵
					if (!isFirstContact && iter->second) continue;

					CollisionInfo info;
					info.isColliding = false;

					if (CheckCollision(receiveCollider, sendCollider, info))
					{
						if (isFirstContact)
						{
							receiveCollider->GetOwner()->OnCollisionBeginOverlap(info);
							_collisionMap.emplace(mapKey, true);
							sendCollider->SetOverlapped(true);
						}
						else
						{
							info.collisionLayer = receiveCollider->GetCollisionLayer();
							info.collisionActor = receiveCollider->GetOwner();
							sendCollider->GetOwner()->OnCollisionStayOverlap(info);

							info.collisionLayer = sendCollider->GetCollisionLayer();
							info.collisionActor = sendCollider->GetOwner();
							receiveCollider->GetOwner()->OnCollisionStayOverlap(info);

							iter->second = true;
						}
					}
					else
					{
						if (!isFirstContact)
						{
							receiveCollider->GetOwner()->OnCollisionEndOverlap(info);
							_collisionMap.erase(iter);
							sendCollider->SetOverlapped(false);
						}
					}
				}
			}
		}
	}
}

void CollisionManager::PostUpdate()
{
	for (auto& [key, visited] : _collisionMap)
		visited = false;
}

void CollisionManager::Render(HDC hdc)
{
}

void CollisionManager::AddCollider(Collider* collider)
{
	_colliderList[collider->GetCollisionLayer()].push_back(collider);
}

void CollisionManager::DeleteCollider(Collider* collider)
{
	ECollisionLayer layer = collider->GetCollisionLayer();
	vector<Collider*>& list = _colliderList[layer];

	auto iter = find(list.begin(), list.end(), collider);
	if (iter != list.end())	list.erase(iter);
}

void CollisionManager::ClearColliderList()
{
	for (int32 i = 0; i < (int32)ECollisionLayer::END; ++i)
		_colliderList[i].clear();
}

void CollisionManager::SetBitFlag(ECollisionLayer layer1, ECollisionLayer layer2, ECollisionResponse responseType, bool on)
{
	uint16* mask = (responseType == C_BLOCK) ? COLLISION_BIT_MASK_BLOCK : COLLISION_BIT_MASK_IGNORE;

	const int16 receive = (int16)layer1;
	const int16 send = (int16)layer2;

	if (on)
	{
		mask[receive] |= (uint16)(1 << send);
		mask[send] |= (uint16)(1 << receive);
	}
	else
	{
		mask[receive] &= ~(uint16)(1 << send);
		mask[send] &= ~(uint16)(1 << receive);
	}
}

bool CollisionManager::CheckCollision(Collider* receive, Collider* send, CollisionInfo& info)
{
	switch (receive->GetCollisionLayer())
	{
	case ECollisionLayer::PLAYER:
		return PlayerCollisionCheck(receive, send, info);
	case ECollisionLayer::ENEMY:
		return EnemyCollisionCheck(receive, send, info);
	case ECollisionLayer::ENEMY_HITBOX:
		return (receive->GetColliderType() == EColliderType::LINE)
			? BulletCollisionCheck(receive, send, info)
			: AxeCollisionCheck(receive, send, info);
	default:
		return false;
	}

	return false;
}

bool CollisionManager::CheckBetweenAABB(AABBCollider* receive, AABBCollider* send, CollisionInfo& info)
{
	Vector2 receiveMin = receive->GetAABBMin();
	Vector2 receiveMax = receive->GetAABBMax();
	Vector2 sendMin = send->GetAABBMin();
	Vector2 sendMax = send->GetAABBMax();

	Vector2 receiveCenter = receive->GetCenter();
	Vector2 sendCenter = send->GetCenter();

	// 충돌체끼리 중심을 잇는 벡터
	double nx = sendCenter.x - receiveCenter.x;
	double ny = sendCenter.y - receiveCenter.y;

	// 충돌체의 길이, 높이의 절반
	Vector2 receiveExtent = (receiveMax - receiveMin) * 0.5f;
	Vector2 sendExtent = (sendMax - sendMin) * 0.5f;

	// n.x가 1이거나 -1이면 x축이 충돌
	double overlapX = receiveExtent.x + sendExtent.x - fabsf(nx);
	// n.y가 1이거나 -1이면 y축이 충돌
	double overlapY = receiveExtent.y + sendExtent.y - fabsf(ny);

	if (overlapX < 0 || overlapY < 0) return false;

	Vector2 normal;
	Vector2 hitPos;

	// 위, 아래 충돌
	if(overlapX > overlapY)
	{
		// 사각형끼리 겹치는 위치는 2군데
		normal = (ny < 0.f) ? Vector2(0.f, -1.f) : Vector2(0.f, 1.f);
		const float contactX = (max(receiveMin.x, sendMin.x) + min(receiveMax.x, sendMax.x)) * 0.5f;
		const float contactY = (ny < 0.f) ? receiveMin.y : receiveMax.y;
		hitPos = { contactX, contactY };
	}
	// 좌, 우 충돌
	else
	{
		normal = (nx < 0.f) ? Vector2(-1.f, 0.f) : Vector2(1.f, 0.f);
		const float contactX = (nx < 0.f) ? receiveMin.x : receiveMax.x;
		const float contactY = (max(receiveMin.y, sendMin.y) + min(receiveMax.y, sendMax.y)) * 0.5f;
		hitPos = { contactX, contactY };
	}

	info.collisionActor = send->GetOwner();
	info.vHitNormal = normal;
	info.collisionPoint = hitPos;
	return true;
}

// Liang-Barsky 알고리즘
bool CollisionManager::CheckLinetoAABB(Vector2 start, Vector2 end, Vector2 AABBmin, Vector2 AABBmax, CollisionInfo& info)
{
	// 방향 벡터
	float dx = end.x - start.x;
	float dy = end.y - start.y;

	// 4개 경계에 대한 p와 q 값
	// 음수 p: 밖->안 (진입), 양수 p: 안->밖 (탈출)
	float p[4] = { -dx, dx, -dy, dy };
	float q[4] = {
		start.x - AABBmin.x,   // 왼쪽 경계까지 거리
		AABBmax.x - start.x,   // 오른쪽 경계까지 거리  
		start.y - AABBmin.y,   // 아래 경계까지 거리
		AABBmax.y - start.y    // 위 경계까지 거리
	};

	float u1 = 0.0f;			// 최대 진입 매개변수
	float u2 = 1.0f;			// 최소 탈출 매개변수

	for (int i = 0; i < 4; i++) 
	{
		if (p[i] == 0) 
		{
			// 선분이 경계와 평행하면서 박스 밖 -> 충돌 없음
			if (q[i] < 0) return false;
		}
		else 
		{
			float t = q[i] / p[i];

			if (p[i] < 0) 
				u1 = max(u1, t); // 진입점
			else 
				u2 = min(u2, t); // 탈출점

			// 조기 종료: 진입이 탈출보다 늦으면 충돌 없음
			if (u1 > u2) return false;
		}
	}

	// u1 <= u2이면 충돌
	info.vHitNormal = (end - start).GetNormalize();
	return true;
}

bool CollisionManager::CheckLinetoLine(Vector2 start1, Vector2 end1, Vector2 start2, Vector2 end2, CollisionInfo& info)
{
	int ccw1 = CCW(start1, end1, start2);
	int ccw2 = CCW(start1, end1, end2);
	int ccw3 = CCW(start2, end2, start1);
	int ccw4 = CCW(start2, end2, end1);

	// 일반적인 교차: 각 선분이 다른 선분을 가로지름
	if (ccw1 * ccw2 < 0 && ccw3 * ccw4 < 0) return true;

	// 특수 케이스: 한 점이 다른 선분 위에 있는 경우
	if (ccw1 == 0 && IsPointOnSegment(start1, end1, start2)) return true;
	if (ccw2 == 0 && IsPointOnSegment(start1, end1, end2)) return true;
	if (ccw3 == 0 && IsPointOnSegment(start2, end2, start1)) return true;
	if (ccw4 == 0 && IsPointOnSegment(start2, end2, end1)) return true;

	return false;
}

// 세 점의 방향성을 계산 (시계반대방향, 시계방향, 일직선)
int32 CollisionManager::CCW(const Vector2& a, const Vector2& b, const Vector2& c) 
{
	const float cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);

	if (cross > 0) return 1;   // 반시계방향
	if (cross < 0) return -1;  // 시계방향
	return 0;                  // 일직선
}

bool CollisionManager::IsPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c)
{
	// c가 a와 b 사이에 있는지 확인 (일직선상일 때만 호출됨)
	return min(a.x, b.x) <= c.x && c.x <= max(a.x, b.x) && min(a.y, b.y) <= c.y && c.y <= max(a.y, b.y);
}

bool CollisionManager::CheckSeparatingAxis(pair<float, float> proj1, pair<float, float> proj2)
{
	return !(proj1.second < proj2.first || proj2.second < proj1.first);
}

vector<Vector2> CollisionManager::MakeAABBCorners(Vector2 pos, float halfWidth, float halfHeight)
{
	return {
		{ pos.x - halfWidth, pos.y - halfHeight },
		{ pos.x + halfWidth, pos.y - halfHeight },
		{ pos.x + halfWidth, pos.y + halfHeight },
		{ pos.x - halfWidth, pos.y + halfHeight }
	};
}


bool CollisionManager::OverlapOnAxis(const std::vector<Vector2>& send, const std::vector<Vector2>& receive, Vector2 axis)
{
	// 축 정규화
	const float lenSq = axis.x * axis.x + axis.y * axis.y;
	if (lenSq < 1e-12f) return true;// 잘못된 축 무시

	//float len = sqrtf(axis.x * axis.x + axis.y * axis.y);
	//if (len == 0.0f) return true; 
	//axis.x /= len;
	//axis.y /= len;

	Vector2 range1 = ProjectionAxis(send, axis);
	Vector2 range2 = ProjectionAxis(receive, axis);

	// 구간 겹침 여부 확인
	return !(range1.y < range2.x || range2.y < range1.x);
}

bool CollisionManager::PlayerCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info)
{
	const float playerHalfWidth = receive->GetWidth() * 0.5f;
	const float playerHalfHeight = receive->GetHeight() * 0.5f;

	Player* player = static_cast<Player*>(receive->GetOwner());
	PlayerMovementComponent* moveComponent = player->GetComponent<PlayerMovementComponent>();

	const Vector2 curPos = receive->GetPos();
	const Vector2 newPos = moveComponent->GetNewPos();

	// 플레이어 AABB 계산
	RECT playerOldRect = {
		(LONG)(curPos.x - playerHalfWidth),
		(LONG)(curPos.y - playerHalfHeight),
		(LONG)(curPos.x + playerHalfWidth),
		(LONG)(curPos.y + playerHalfHeight)
	};

	RECT playerNewRect = {
		(LONG)(newPos.x - playerHalfWidth),
		(LONG)(newPos.y - playerHalfHeight),
		(LONG)(newPos.x + playerHalfWidth),
		(LONG)(newPos.y + playerHalfHeight)
	};

	switch (send->GetCollisionLayer())
	{
	case ECollisionLayer::GROUND:
	case ECollisionLayer::PORTAL:
		info = CheckAABBGroundCollision(playerOldRect, playerNewRect, send);
		break;
	case ECollisionLayer::WALL:
		info = CheckAABBtoLineWallCollision(curPos, newPos, playerHalfWidth, playerHalfHeight, send);
		break;
	case ECollisionLayer::CEILING:
		info = CheckAABBtoLineCeilingCollision(curPos, newPos, playerHalfWidth, playerHalfHeight, send);
		break;
	case ECollisionLayer::STAIR:
		if (!moveComponent->GetIsPlatform())
			info = CheckAABBtoLineStairCollision(curPos, newPos, playerHalfWidth, playerHalfHeight, send, moveComponent->GetOnStair());
		break;
	case ECollisionLayer::PLATFORM:
		info = CheckAABBtoLinePlatformCollision(curPos, newPos, playerHalfWidth, playerHalfHeight, send);
		break;
	case ECollisionLayer::ENEMY_HITBOX:
		if (send->GetColliderType() == EColliderType::AABB)
		{
			info.isColliding = CheckBetweenAABB(static_cast<AABBCollider*>(receive), static_cast<AABBCollider*>(send), info);
		}
		else
		{
			Vector2 aabbMin = { (float)playerNewRect.left, (float)playerNewRect.top };
			Vector2 aabbMax = { (float)playerNewRect.right, (float)playerNewRect.bottom };
			info.isColliding = CheckLinetoAABB(send->GetEndPoint(), send->GetStartPoint(), aabbMin, aabbMax, info);
			if (info.isColliding)
				info.collisionActor = send->GetOwner();
		}
		break;
	default:
		break;
	}

	info.collisionLayer = send->GetCollisionLayer();
	return info.isColliding;
}

bool CollisionManager::EnemyCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info)
{
	const float colliderHalfWidth = receive->GetWidth() * 0.5f;
	const float colliderHalfHeight = receive->GetHeight() * 0.5f;

	Actor* receiveOwner = receive->GetOwner();
	const Vector2 curPos = receive->GetPos();
	const Vector2 newPos = receiveOwner->GetNewPos();


	RECT oldRect = {
		(LONG)(curPos.x - colliderHalfWidth),
		(LONG)(curPos.y - colliderHalfHeight),
		(LONG)(curPos.x + colliderHalfWidth),
		(LONG)(curPos.y + colliderHalfHeight)
	};

	RECT newRect = {
		(LONG)(newPos.x - colliderHalfWidth),
		(LONG)(newPos.y - colliderHalfHeight),
		(LONG)(newPos.x + colliderHalfWidth),
		(LONG)(newPos.y + colliderHalfHeight)
	};

	switch (send->GetCollisionLayer())
	{
	case ECollisionLayer::GROUND:
		info = CheckAABBGroundCollision(oldRect, newRect, send);
		break;
	case ECollisionLayer::WALL:
		info = CheckAABBtoLineWallCollision(curPos, newPos, colliderHalfWidth, colliderHalfHeight, send);
		break;
	case ECollisionLayer::CEILING:
		info = CheckAABBtoLineCeilingCollision(curPos, newPos, colliderHalfWidth, colliderHalfHeight, send);
		break;
	case ECollisionLayer::STAIR:
		{
			Enemy* enemy = static_cast<Enemy*>(receiveOwner);
			bool onStair = enemy->GetComponent<EnemyMovementComponent>()->GetOnStair();
			bool ignoreUp = !(receiveOwner->GetWasHit() || receiveOwner->GetIsDead());
			info = CheckAABBtoLineStairCollision(curPos, newPos, colliderHalfWidth, colliderHalfHeight, send, onStair, ignoreUp);
		}
		break;
	case ECollisionLayer::PLATFORM:
		info = CheckAABBtoLinePlatformCollision(curPos, newPos, colliderHalfWidth, colliderHalfHeight, send);
		break;
	case ECollisionLayer::ENEMY_HITBOX:
		if (send->GetColliderType() == EColliderType::LINE)
		{
			Vector2 aabbMin = { (float)newRect.left, (float)newRect.top };
			Vector2 aabbMax = { (float)newRect.right, (float)newRect.bottom };
			// 반사됐기 때문에 시작, 끝 지점 반대로 넣어줌
			info.isColliding = CheckLinetoAABB(send->GetEndPoint(), send->GetStartPoint(), aabbMin, aabbMax, info);
			if (info.isColliding)
				info.collisionActor = send->GetOwner();
		}
		else
		{
			info.isColliding = CheckBetweenAABB(static_cast<AABBCollider*>(receive), static_cast<AABBCollider*>(send), info);
		}
		break;
	default:
		break;
	}

	info.collisionLayer = send->GetCollisionLayer();
	return info.isColliding;
}

bool CollisionManager::BulletCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info)
{
	const Vector2 pos = receive->GetPos();
	const float radian = receive->GetRadian();
	const float halfLength = receive->GetLength() * 0.5f;
	const float dx = cosf(radian) * halfLength;
	const float dy = sinf(radian) * halfLength;
	const Vector2 start = { pos.x - dx, pos.y - dy };
	const Vector2 end = { pos.x + dx, pos.y + dy };

	switch (send->GetCollisionLayer())
	{
	case ECollisionLayer::GROUND:
		{
			const RECT AABB = static_cast<AABBCollider*>(send)->GetRect();
			const Vector2 AABBmin = { (float)AABB.left, (float)AABB.top };
			const Vector2 AABBmax = { (float)AABB.right, (float)AABB.bottom };
			info.isColliding = CheckLinetoAABB(start, end, AABBmin, AABBmax, info);
		}
		break;
	case ECollisionLayer::WALL:
	case ECollisionLayer::CEILING:
	case ECollisionLayer::STAIR:
		info.isColliding = CheckLinetoLine(start, end, send->GetStartPoint(), send->GetEndPoint(), info);
		break;
	default:
		break;
	}

	info.collisionLayer = send->GetCollisionLayer();
	return info.isColliding;
}

bool CollisionManager::AxeCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info)
{
	const float axeHalfWidth = receive->GetWidth() * 0.5f;
	const float axeHalfHeight = receive->GetHeight() * 0.5f;

	Axe* axe = static_cast<Axe*>(receive->GetOwner());
	const Vector2 curPos = axe->GetPos();
	const Vector2 newPos = axe->GetNewPos();


	RECT oldRect = {
		(LONG)(curPos.x - axeHalfWidth),
		(LONG)(curPos.y - axeHalfHeight),
		(LONG)(curPos.x + axeHalfWidth),
		(LONG)(curPos.y + axeHalfHeight)
	};

	RECT newRect = {
		(LONG)(newPos.x - axeHalfWidth),
		(LONG)(newPos.y - axeHalfHeight),
		(LONG)(newPos.x + axeHalfWidth),
		(LONG)(newPos.y + axeHalfHeight)
	};

	switch (send->GetCollisionLayer())
	{
	case ECollisionLayer::GROUND:
		info = CheckAABBGroundCollision(oldRect, newRect, send);
		break;
	case ECollisionLayer::WALL:
		info = CheckAABBtoLineWallCollision(axe->GetPos(), axe->GetNewPos(), axeHalfWidth, axeHalfHeight, send);
		break;
	case ECollisionLayer::CEILING:
		info = CheckAABBtoLineCeilingCollision(axe->GetPos(), axe->GetNewPos(), axeHalfWidth, axeHalfHeight, send);
		break;
	case ECollisionLayer::ENEMY:
		info.isColliding = CheckBetweenAABB(static_cast<AABBCollider*>(receive), static_cast<AABBCollider*>(send), info);
		break;
	}

	info.collisionLayer = send->GetCollisionLayer();
	return info.isColliding;
}

bool CollisionManager::CheckOBBHitBox(Actor* attackActor, AttackInfo& hitInfo)
{
	const Vector2 center = attackActor->GetPos();
	// obb vs aabb 용
	const vector<Vector2> corners = GetRotatedCorners(center.x, center.y, hitInfo.fAttackRadian, hitInfo.fWidth, hitInfo.fHeight);

	// obb vs line 용
	const float cosRotation = cosf(hitInfo.fAttackRadian);
	const float sinRotation = sinf(hitInfo.fAttackRadian);
	const Vector2 localAxisX = Vector2(cosRotation, sinRotation);
	const Vector2 localAxisY = Vector2(-sinRotation, cosRotation);
	const Vector2 AABBmin = Vector2(-hitInfo.fWidth * 0.5f, -hitInfo.fHeight * 0.5f);
	const Vector2 AABBmax = Vector2(hitInfo.fWidth * 0.5f, hitInfo.fHeight * 0.5f);

	bool isReflectBullet = false;

	for (int32 receiveLayer = 0; receiveLayer < (int32)ECollisionLayer::END; ++receiveLayer)
	{
		if (!(COLLISION_BIT_MASK_BLOCK[receiveLayer] & (uint16)(1 << (int32)hitInfo._attackLayer))) continue;

		for (Collider* receiveCollider : _colliderList[receiveLayer])
		{
			if (receiveCollider->GetOwner()->GetWasHit()) continue;
			if (MatchColliderId(receiveCollider, hitInfo._hitActors)) continue;

			if (receiveLayer == (int32)ECollisionLayer::ENEMY)
			{
				const vector<Vector2> colliderCorners = MakeAABBCorners(receiveCollider->GetPos(), receiveCollider->GetWidth() * 0.5f, receiveCollider->GetHeight() * 0.5f);
				if (CheckOBBtoAABB(corners, colliderCorners))
				{
					hitInfo._hitActors.push_back(receiveCollider);
					receiveCollider->GetOwner()->TakeDamage(attackActor, hitInfo.vAttackDir);
				}
			}
			else if (receiveLayer == (int32)ECollisionLayer::ENEMY_HITBOX)
			{
				if (receiveCollider->GetColliderType() == EColliderType::LINE)
				{
					// 로컬 좌표계로 변환하여 라인 vs AABB 검사
					Vector2 relative1 = receiveCollider->GetStartPoint() - center;
					Vector2 relative2 = receiveCollider->GetEndPoint() - center;
					relative1 = { relative1.Dot(localAxisX), relative1.Dot(localAxisY) };
					relative2 = { relative2.Dot(localAxisX), relative2.Dot(localAxisY) };

					CollisionInfo info;
					if (CheckLinetoAABB(relative1, relative2, AABBmin, AABBmax, info))
					{
						isReflectBullet = true;
						hitInfo._hitActors.push_back(receiveCollider);
						receiveCollider->GetOwner()->TakeDamage(attackActor, hitInfo.vAttackDir);
					}
				}
				else
				{
					const vector<Vector2> colCorners = MakeAABBCorners(receiveCollider->GetPos(), receiveCollider->GetWidth() * 0.5f, receiveCollider->GetHeight() * 0.5f);
					if (CheckOBBtoAABB(corners, colCorners))
					{
						isReflectBullet = true;
						hitInfo._hitActors.push_back(receiveCollider);
						receiveCollider->GetOwner()->TakeDamage(attackActor, hitInfo.vAttackDir);
					}
				}
			}
		}
	}

	return isReflectBullet;
}

bool CollisionManager::CheckAABBHitBox(Actor* attackActor, Vector2 center, float width, float height)
{
	const Vector2 hitBoxMin = { center.x - width * 0.5f, center.y - height * 0.5f };
	const Vector2 hitBoxMax = { center.x + width * 0.5f, center.y + height * 0.5f };

	for (auto& collider : _colliderList[ECollisionLayer::PLAYER])
	{
		Vector2 colPos = collider->GetPos();
		const float colHalfWidth = collider->GetWidth() * 0.5f;
		const float colHalfHeight = collider->GetHeight() * 0.5f;

		//const Vector2 colMin = { colPos.x - colHalfWidth, colPos.y - colHalfHeight };
		//const Vector2 colMax = { colPos.x + colHalfWidth, colPos.y + colHalfHeight };

		// 충돌체끼리 중심을 잇는 벡터
		const double nx = center.x - colPos.x;
		const double ny = center.y - colPos.y;

		const float overlapX = (colHalfWidth + width * 0.5f) - fabsf(nx);
		const float overlapY = (colHalfHeight + height * 0.5f) - fabsf(ny);

		// 충돌하지 않으면 true를 반환해서 히트박스를 계속 유지
		if (overlapX < 0 || overlapY < 0) return true;

		Vector2 dir = colPos - center;
		dir.Normalize();
		collider->GetOwner()->TakeDamage(attackActor, dir);
	}

	return false;
}

bool CollisionManager::CheckOBBtoAABB(const vector<Vector2>& OBB, const vector<Vector2>& AABB)
{
	// 검사할 축: OBB의 두 변의 법선벡터, AABB의 x축, y축의 법선벡터
	Vector2 edge1 = { OBB[1].x - OBB[0].x, OBB[1].y - OBB[0].y };
	Vector2 edge2 = { OBB[3].x - OBB[0].x, OBB[3].y - OBB[0].y };
	Vector2 axes[4] = {
		{ -edge1.y, edge1.x },
		{ -edge2.y, edge2.x },
		{ 1, 0 }, 
		{ 0, 1 }
	};

	for (const Vector2& axis : axes)
	{
		// 분리축 발견 = 충돌 없음
		if (!OverlapOnAxis(OBB, AABB, axis)) return false;
	}

	return true;	// 모든 축에서 겹침
}

CollisionInfo CollisionManager::CheckAABBGroundCollision(const RECT& playerOldRect, const RECT& playerNewRect, Collider* groundCollider)
{
	CollisionInfo result = {};
	float groundHalfWidth = groundCollider->GetWidth() * 0.5f;
	float groundHalfHeight = groundCollider->GetHeight() * 0.5f;
	Vector2 groundPos = groundCollider->GetPos();

	RECT groundRect = {
		groundPos.x - groundHalfWidth,
		groundPos.y - groundHalfHeight,
		groundPos.x + groundHalfWidth,
		groundPos.y + groundHalfHeight
	};

	// 현재 위치에서 겹치는지 확인
	// 위에서 아래는 같은 높이기만 해도 겹치는 걸로 판단.
	if (!(playerNewRect.left < groundRect.right &&
		playerNewRect.right > groundRect.left &&
		playerNewRect.top < groundRect.bottom &&
		playerNewRect.bottom >= groundRect.top))
	{
		return result;
	}

	// 어느 방향에서 충돌했는지 확인
	bool fromTop = (playerOldRect.bottom <= groundRect.top && playerNewRect.bottom >= groundRect.top);		// 위에서 아래
	bool fromBottom = (playerOldRect.top >= groundRect.bottom && playerNewRect.top < groundRect.bottom);	// 아래에서 위로
	bool fromLeft = (playerOldRect.right <= groundRect.left && playerNewRect.right > groundRect.left);		// 왼쪽에서 오른쪽으로
	bool fromRight = (playerOldRect.left >= groundRect.right && playerNewRect.left < groundRect.right);		// 오른쪽에서 왼쪽으로

	// 여러 방향에서 동시에 충돌한 경우, 침투 깊이가 가장 작은 방향을 선택
	float topPenetration = fromTop ? (playerNewRect.bottom - groundRect.top) : FLT_MAX;
	float bottomPenetration = fromBottom ? (groundRect.bottom - playerNewRect.top) : FLT_MAX;
	float leftPenetration = fromLeft ? (playerNewRect.right - groundRect.left) : FLT_MAX;
	float rightPenetration = fromRight ? (groundRect.right - playerNewRect.left) : FLT_MAX;

	float minPenetration = min(topPenetration, bottomPenetration);
	minPenetration = min(minPenetration, leftPenetration);
	minPenetration = min(minPenetration, rightPenetration);

	if (minPenetration == FLT_MAX) return result; // 충돌하지 않음

	result.isColliding = true;
	result.penetrationDepth = minPenetration;
	result.collisionActor = groundCollider->GetOwner();

	const float midX = (max(playerNewRect.left, groundRect.left) + min(playerNewRect.right, groundRect.right)) * 0.5f;
	const float midY = (max(playerNewRect.top, groundRect.top) + min(playerNewRect.bottom, groundRect.bottom)) * 0.5f;

	// 가장 작은 침투 깊이에 따라 충돌 방향 결정
	if (minPenetration == topPenetration) // 위에서 아래로 (바닥)
	{
		result.collisionPoint = Vector2(midX, (float)groundRect.top);
		result.vHitNormal = Vector2(0, -1); // 위쪽 방향
		result.hitCorner = 0; // 바닥 충돌
	}
	else if (minPenetration == bottomPenetration) // 아래에서 위로 (천장)
	{
		result.collisionPoint = Vector2(midX, (float)groundRect.bottom);
		result.vHitNormal = Vector2(0.f,  1.f); // 아래쪽 방향
		result.hitCorner = 3; // 천장 충돌
	}
	else if (minPenetration == leftPenetration) // 왼쪽에서 오른쪽으로 (오른쪽 벽)
	{
		result.collisionPoint = Vector2((float)groundRect.left,  midY);
		result.vHitNormal = Vector2(-1.f,  0.f); // 왼쪽 방향
		result.hitCorner = 1; // 왼쪽 벽 충돌
	}
	else if (minPenetration == rightPenetration) // 오른쪽에서 왼쪽으로 (왼쪽 벽)
	{
		result.collisionPoint = Vector2((float)groundRect.right, midY);
		result.vHitNormal = Vector2(1.f,  0.f); // 오른쪽 방향
		result.hitCorner = 2; // 오른쪽 벽 충돌
	}

	return result;
}

CollisionInfo CollisionManager::CheckAABBtoLinePlatformCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* lineCollider)
{
	CollisionInfo result = {};

	if ((newPos - oldPos).y < 0.f) return result; // 위로 이동 중이면 스킵

	Vector2 lineStart = lineCollider->GetStartPoint();
	Vector2 lineEnd = lineCollider->GetEndPoint();

	float outY1, outY2;

	bool inSegment1 = GetYOnLineAtX(lineStart, lineEnd, newPos.x - halfWidth, outY1);
	bool inSegment2 = GetYOnLineAtX(lineStart, lineEnd, newPos.x + halfWidth, outY2);

	if (!inSegment1 && !inSegment2) return result;

	// 플랫폼의 최고점 찾기
	float platformTopY = (inSegment1 && inSegment2) ? min(outY1, outY2) : (inSegment1 ? outY1 : outY2);
	// 플레이어 바닥 Y좌표
	float playerBottomY = newPos.y + halfHeight;
	float playerOldBottomY = oldPos.y + halfHeight;

	// 이전에는 위에 있었고, 현재는 플랫폼에 닿았거나 관통했는지 확인
	// 2픽셀 허용
	if (playerOldBottomY <= platformTopY && playerBottomY >= platformTopY - 2.f)
	{
		const Vector2 lineDir = lineEnd - lineStart;
		result.isColliding = true;
		result.collisionPoint = Vector2(newPos.x, platformTopY);
		result.vHitNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
		if (result.vHitNormal.y > 0) result.vHitNormal *= -1;

		result.collisionActor = lineCollider->GetOwner();
		result.penetrationDepth = max(0.0f, playerBottomY - platformTopY);
	}

	return result;
}

CollisionInfo CollisionManager::CheckAABBtoLineWallCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* wallCollider)
{
	CollisionInfo result = {};

	Vector2 moveDir = newPos - oldPos;
	Vector2 lineStart = wallCollider->GetStartPoint();
	Vector2 lineEnd = wallCollider->GetEndPoint();

	Vector2 intersection;
	float t;
	float cross;

	auto TryIntersect = [&](Vector2 mStart, Vector2 mEnd, Vector2 normal) -> bool
		{
			if (LineIntersectsLineSegment(mStart, mEnd, lineStart, lineEnd, intersection, t, cross))
			{
				result.isColliding = true;
				result.collisionPoint = intersection;
				result.vHitNormal = normal;
				result.collisionActor = wallCollider->GetOwner();
				return true;
			}
			return false;
		};

	if (moveDir.x > 0)
	{
		TryIntersect({ oldPos.x + halfWidth, oldPos.y }, { newPos.x + halfWidth, newPos.y }, { -1.f, 0.f });
	}
	else if(moveDir.x < 0)
	{
		TryIntersect({ oldPos.x - halfWidth, oldPos.y }, { newPos.x - halfWidth, newPos.y }, { 1.f, 0.f });
	}
	// 플레이어가 이동 중이 아니어도 벽에 닿았는지는 확인
	else
	{
		// 1px 여유
		if (!TryIntersect({ oldPos.x + halfWidth, oldPos.y }, { newPos.x + halfWidth + 1.f, newPos.y }, { -1.f, 0.f }))
			TryIntersect({ oldPos.x - halfWidth, oldPos.y }, { newPos.x - halfWidth - 1.f, newPos.y }, { 1.f, 0.f });
	}

	return result;
}

CollisionInfo CollisionManager::CheckAABBtoLineCeilingCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* wallCollider)
{
	CollisionInfo result = {};

	if ((newPos - oldPos).y >= 0.f) return result; // 아래로 이동 중이면 스킵

	const Vector2 lineStart = wallCollider->GetStartPoint();
	const Vector2 lineEnd = wallCollider->GetEndPoint();

	Vector2 intersection;
	float t;
	float cross;

	const Vector2 moveStart = Vector2(oldPos.x, oldPos.y - halfHeight);
	const Vector2 moveEnd = Vector2(newPos.x, newPos.y - halfHeight);
	if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
	{
		result.isColliding = true;
		result.collisionPoint = intersection;
		result.vHitNormal = Vector2(0, 1);
		result.collisionActor = wallCollider->GetOwner();
	}

	return result;
}

CollisionInfo CollisionManager::CheckAABBtoLineStairCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* stairCollider, bool wasStair, bool ignoreUpward)
{
	CollisionInfo result = {};

	Vector2 stairStart = stairCollider->GetStartPoint();
	Vector2 stairEnd = stairCollider->GetEndPoint();
	const Vector2 stairDir = stairEnd - stairStart;

	// 계단 방향 판단 (45도 가정)
	const bool isRightUpStair = (stairDir.x > 0 && stairDir.y < 0) || (stairDir.x < 0 && stairDir.y > 0);

	// 이미 계단에 있는 경우
	if (wasStair)
	{
		const Vector2 targetCorner = isRightUpStair
			? Vector2(newPos.x + halfWidth, newPos.y + halfHeight)
			: Vector2(newPos.x - halfWidth, newPos.y + halfHeight);

		// 계단 범위를 약간 벗어나도 근처에 있으면 계속 유지
		float stairMinX = min(stairStart.x, stairEnd.x);
		float stairMaxX = max(stairStart.x, stairEnd.x);
		float tolerance = 10.0f; // 10픽셀 여유

		if (targetCorner.x >= stairMinX - tolerance && targetCorner.x <= stairMaxX + tolerance)
		{
			// 계단 연장선상의 Y값 계산
			float extendedY;
			GetYOnLineAtX(stairStart, stairEnd, targetCorner.x, extendedY);

			if (fabsf(targetCorner.y - extendedY) <= 15.0f)
			{
				result.isColliding = true;
				result.collisionPoint = Vector2(targetCorner.x, extendedY);
				result.vHitNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
				if (result.vHitNormal.y > 0) result.vHitNormal *= -1;
				result.collisionActor = stairCollider->GetOwner();
				result.hitCorner = isRightUpStair ? 2 : 1;
				return result;
			}
		}
		// 계단 범위를 벗어났으면 충돌 없음
		return result;
	}

	// 새로운 계단 충돌 검사
	Vector2 moveDir = newPos - oldPos;
	if (ignoreUpward && moveDir.y < -5.f) return result;

	// 양쪽 모서리 모두 검사하되, 계단 방향에 따라 우선순위 결정
	const Vector2 newBottomLeft = Vector2(newPos.x - halfWidth, newPos.y + halfHeight);
	const Vector2 newBottomRight = Vector2(newPos.x + halfWidth, newPos.y + halfHeight);

	const Vector2 oldBottomLeft = Vector2(oldPos.x - halfWidth, oldPos.y + halfHeight);
	const Vector2 oldBottomRight = Vector2(oldPos.x + halfWidth, oldPos.y + halfHeight);

	// 계단과의 교차점 검사
	Vector2 intersection;
	float t;
	float cross;
	int hitCorner = 0;
	float bestT = FLT_MAX;
	Vector2 bestIntersection;

	for (int offset = -1; offset <= 1; ++offset)
	{
		const float dy = (float)offset;

		// 우상향 계단: 오른쪽 모서리 우선
		if (LineIntersectsLineSegment(
			{ oldBottomRight.x, oldBottomRight.y + dy }, { newBottomRight.x, newBottomRight.y + dy },
			stairStart, stairEnd, intersection, t, cross))
		{
			if (t >= 0.f && t <= 1.f && t < bestT)
			{
				bestT = t;
				bestIntersection = intersection;
				hitCorner = 2;
			}
		}

		// 오른쪽에서 충돌이 없으면 왼쪽도 검사
		if (LineIntersectsLineSegment(
			{ oldBottomLeft.x, oldBottomLeft.y + dy }, { newBottomLeft.x, newBottomLeft.y + dy },
			stairStart, stairEnd, intersection, t, cross))
		{
			if (t >= 0.f && t <= 1.f && t < bestT)
			{
				bestT = t;
				bestIntersection = intersection;
				hitCorner = 1;
			}
		}
	}
	
	if (bestT == FLT_MAX) return result;

	// 플레이어가 계단 위에서 접근하는지 확인 (아래에서 위로 올라가는 것 방지)
	Vector2 stairNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
	if (stairNormal.y > 0) stairNormal *= -1;

	// 결과 설정
	result.isColliding = true;
	result.collisionPoint = bestIntersection;
	result.vHitNormal = stairNormal;
	result.collisionActor = stairCollider->GetOwner();
	result.hitCorner = hitCorner;

	return result;
}

bool CollisionManager::LineIntersectsLineSegment(Vector2 p1, Vector2 p2, Vector2 q1, Vector2 q2, Vector2& intersection, float& t, float& crossOut)
{
	Vector2 dir1 = p2 - p1;
	const Vector2 dir2 = q2 - q1;
	const float cross = dir1.x * dir2.y - dir1.y * dir2.x;
	crossOut = cross;

	if (fabsf(cross) < 1e-6f) return false; // 평행

	const Vector2 diff = q1 - p1;
	const float t1 = (diff.x * dir2.y - diff.y * dir2.x) / cross;
	const float t2 = (diff.x * dir1.y - diff.y * dir1.x) / cross;

	if (t1 >= 0.0f && t1 <= 1.0f && t2 >= 0.0f && t2 <= 1.0f)
	{
		intersection = p1 + dir1 * t1;
		t = t1;
		return true;
	}

	return false;
}

bool CollisionManager::GetYOnLineAtX(const Vector2& a, const Vector2& b, float x, float& outY)
{
	const float dx = b.x - a.x;
	if (fabsf(dx) < 1e-6f) return false; // 수직선 예외 처리

	const float t = (x - a.x) / dx;
	outY = a.y + (b.y - a.y) * t;

	return (t >= 0.0f && t <= 1.0f);
}

bool CollisionManager::MatchColliderId(Collider* collider, vector<Collider*> list)
{
	const uint32 id = collider->GetColliderId();
	for (Collider* col : list)
		if (id == col->GetColliderId()) return true;

	return false;
}
