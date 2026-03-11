#pragma once
#include "../Utils/Singleton.h"

struct CollisionMapID
{
	uint32 left;
	uint32 right;

	CollisionMapID(uint32 a = 0, uint32 b = 0)
	{
		if (a < b)	{ left = a; right = b; }
		else		{ left = b; right = a; }
	}

	// 64비트 단일 값으로 변환
	uint64 ToUInt64() const noexcept
	{
		return (static_cast<uint64>(left) << 32) | static_cast<uint64>(right);
	}

	// 동등 비교
	bool operator==(const CollisionMapID& other) const noexcept
	{
		return left == other.left && right == other.right;
	}
};

class Collider;
class OBBCollider;
class AABBCollider;
class LineCollider;
class Player;
class Actor;

class CollisionManager : public Singleton<CollisionManager>
{
	friend Singleton<CollisionManager>;
private:
	CollisionManager(){}

	vector<Collider*> _colliderList[(int32)ECollisionLayer::END];
	unordered_map<uint64, bool> _collisionMap;

	uint16 COLLISION_BIT_MASK_IGNORE[(int32)ECollisionLayer::END] = {};
	uint16 COLLISION_BIT_MASK_BLOCK[(int32)ECollisionLayer::END] = {};

	// 유효한 sendLayer 목록을 receiveLayer별로 캐시 (Update 루프 최적화용)
	vector<int32> _cachedSendLayers[(int32)ECollisionLayer::END];

	void RebuildLayerCache();
	void SetBitFlag(ECollisionLayer layer1, ECollisionLayer layer2, ECollisionResponse responseType, bool on);

	bool CheckCollision(Collider* receive, Collider* send, CollisionInfo& info);
	bool CheckBetweenAABB(AABBCollider* receive, AABBCollider* send, CollisionInfo& info);

	bool CheckLinetoAABB(Vector2 start, Vector2 end, Vector2 AABBmin, Vector2 AABBmax, CollisionInfo& info);
	bool CheckLinetoLine(Vector2 start1, Vector2 end1, Vector2 start2, Vector2 end2, CollisionInfo& info);

	bool CheckSeparatingAxis(pair<float, float> proj1, pair<float, float> proj2);

	// 세 점의 방향성을 계산 (시계반대방향, 시계방향, 일직선)
	int32 CCW(const Vector2& a, const Vector2& b, const Vector2& c);
	bool IsPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c);

	vector<Vector2> MakeAABBCorners(Vector2 pos, float halfWidth, float halfHeight);

public:
	void Init();
	void Update();
	void PostUpdate();
	void Render(HDC hdc);

	void AddCollider(Collider* collider);
	void DeleteCollider(Collider* collider);
	void ClearColliderList();

	const vector<Collider*>& GetPlacedColliders(ECollisionLayer layer) const { return _colliderList[layer]; }

	bool PlayerCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info);
	bool EnemyCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info);
	bool BulletCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info);
	bool AxeCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info);

	bool CheckOBBHitBox(Actor* attackActor, AttackInfo& info);
	bool CheckAABBHitBox(Actor* attackActor, Vector2 center, float width, float height);
	bool CheckOBBtoAABB(const vector<Vector2>& OBB, const vector<Vector2>& AABB);
	bool OverlapOnAxis(const vector<Vector2>& send, const vector<Vector2>& receive, Vector2 axis);

	CollisionInfo CheckAABBGroundCollision(const RECT& playerOldRect, const RECT& playerNewRect, Collider* groundCollider);
	
	CollisionInfo CheckAABBtoLinePlatformCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* lineCollider);
	CollisionInfo CheckAABBtoLineWallCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* wallCollider);
	CollisionInfo CheckAABBtoLineCeilingCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* wallCollider);
	CollisionInfo CheckAABBtoLineStairCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* stairCollider, bool wasStair, bool ignoreUpward = true);

	bool LineIntersectsLineSegment(Vector2 p1, Vector2 p2, Vector2 q1, Vector2 q2, Vector2& intersection, float& t, float& crossOut);
	bool GetYOnLineAtX(const Vector2& a, const Vector2& b, float x, float& outY);

	bool MatchColliderId(Collider* collider, vector<Collider*> list);
};

