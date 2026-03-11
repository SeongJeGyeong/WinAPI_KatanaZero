#pragma once
//#include <windef.h>

constexpr int GWinSizeX = 1280;
constexpr int GWinSizeY = 720;

constexpr int SubWinSizeX = 1280;
constexpr int SubWinSizeY = 720;

const int TILE_SIZE = 32;
const int TileMapWidth = 42;
const int TileMapHeight = 23;

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;
using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

#define SMALL_NUMBER			(1.e-8f)
#define PI						(3.141592f)

float RadianToDegree(float radian);
float DegreeToRadian(float degree);
bool IsInPoint(RECT rect, POINT pos);

enum ESFXType
{
	SFX_STRUGGLE,
	SFX_DEFAULT
};

enum EBGMType
{
	MAIN_MENU,
	STAGE,
	BOSS_ROOM
};

enum EButtonTextAlign
{
	LEFT,
	CENTER,
	RIGHT
};

enum ECollisionLayer
{
	PLAYER,
	ENEMY,
	BOSS,
	GROUND,
	PLATFORM,
	WALL,
	CEILING,
	STAIR,
	PLAYER_HITBOX,
	ENEMY_HITBOX,
	PORTAL,
	END,
};

enum ECollisionResponse
{
	C_IGNORE,
	C_OVERLAP,
	C_BLOCK
};

enum EColliderType
{
	AABB,
	OBB,
	LINE
};

enum ESceneType
{
	LOBBY,
	EDITOR,
	GAME,
	NONE,
};

enum ERenderLayer
{
	BACKGROUND,
	FOREGROUND,
	ACTOR,
	LAYER_END,
};

enum EColliderMode
{
	COL_BOX,
	COL_PLATFORM,
	COL_WALL_V,
	COL_WALL_H,
	COL_STAIR,
	COL_PORTAL,
	COL_END,
};

enum EPlayerState
{
	PLAYER_IDLE,
	PLAYER_IDLE_TO_RUN,
	PLAYER_RUN,
	PLAYER_RUN_TO_IDLE,
	PLAYER_JUMP,
	PLAYER_FALL,
	PLAYER_PRECROUCH,
	PLAYER_CROUCH,
	PLAYER_POSTCROUCH,
	PLAYER_ATTACK,
	PLAYER_ROLL,
	PLAYER_WALLSLIDE,
	PLAYER_WALLJUMP,

	PLAYER_HURT_BEGIN,
	PLAYER_HURT_LOOP,
	PLAYER_HURT_GROUND,
	PLAYER_HURT_RECOVER,

	PLAYER_STRUGGLE,
	PLAYER_FINISH,

	PLAYER_END
};

enum EEnemyState
{
	ENEMY_IDLE,
	ENEMY_WALK,
	ENEMY_TURN,
	ENEMY_RUN,
	ENEMY_AIM,
	ENEMY_HURT_FLY,
	ENEMY_HURT_GROUND,

	ENEMY_END
};

enum EBossState
{
	BOSS_IDLE,
	BOSS_READY,
	BOSS_TOBATTLE,
	BOSS_BLOCK,

	BOSS_PRELUNGE,
	BOSS_LUNGE,
	BOSS_LUNGEATTACK,

	BOSS_PREJUMP,
	BOSS_JUMP,
	BOSS_JUMPATTACK,
	BOSS_LAND,

	BOSS_THROWAXE,
	BOSS_TUGAXE,
	BOSS_RETURNAXE,

	BOSS_HURT,
	BOSS_RECOVER,

	BOSS_STRUGGLE,
	BOSS_FINISHED,
	BOSS_DEFEAT,
	BOSS_DIE,

	BOSS_END
};

struct ColliderInfo
{
	EColliderMode mode;
	POINT vStart;
	POINT vEnd;
};

struct Vector2
{
	// 좌표
	float x = 0;
	float y = 0;

	Vector2() : x(0), y(0) {}
	Vector2(float x, float y) : x(x), y(y) {}
	Vector2(POINT pt) : x((float)pt.x), y((float)pt.y) {}

	// 사칙연산 정의
	// +, - , *, / 
	Vector2 operator+(const Vector2& other)
	{
		Vector2 ret;
		ret.x = x + other.x;
		ret.y = y + other.y;
		return ret;
	}
	Vector2 operator-(const Vector2& other)
	{
		Vector2 ret;
		ret.x = x - other.x;
		ret.y = y - other.y;
		return ret;
	}
	Vector2 operator*(float ratio)
	{
		Vector2 ret;
		ret.x = x * ratio;
		ret.y = y * ratio;
		return ret;
	}

	void operator+=(const Vector2& other)
	{
		x += other.x;
		y += other.y;
	}
	void operator-=(const Vector2& other)
	{
		x -= other.x;
		y -= other.y;
	}
	void operator*=(float ratio)
	{
		x *= ratio;
		y *= ratio;
	}
	bool operator==(const Vector2& other) const
	{
		return (x == other.x && y == other.y);
	}

	// 벡터의 길이 구하기
	// 엔진 살펴보면, Legnth, LengthSquared (벡터는 보통 길이를 리턴하는 함수가 2종류)
	// sqrt(루트) 연산은 생각보다 비싸기 때문에, 꼭 진짜 길이가 필요한 경우가 아니면,
	// LengthSquared (제곱 길이) 버전을 사용하는게 더 좋다.
	float Length()
	{
		return ::sqrt(x * x + y * y);
	}

	// 길이의 제곱 버전
	float LengthSquared()
	{
		return (x * x + y * y);
	}

	// 자기자신을 정규화
	void Normalize()
	{
		// 벡터의 크기를 구해서, 크기만큼 나누기 해주면 된다.
		float length = Length();

		// 언리얼 엔진은 풀 오픈소스. 언리얼 참고해서 공부해도 좋습니다.
		//length 가 0이거나 엄청 작을수 있다
		if (length < SMALL_NUMBER)
			return;	// 굳이 정규화안하고 리턴

		// 정규화.
		x /= length;
		y /= length;
	}

	// 정규화된 복사된 벡터
	Vector2 GetNormalize()
	{
		Vector2 normalize = *this;
		normalize.Normalize();
		return normalize;
	}

	// 내적
	float Dot(const Vector2& other)
	{
		return x * other.x + y * other.y;
	}

	// radian 각도만큼 회전시키자.
	Vector2 Rotate(float radian)
	{
		float cosA = ::cosf(radian);
		float sinA = ::sinf(radian);

		return Vector2(x * cosA - y * sinA, x * sinA + y * cosA);
	}

	// 외적
	float Cross(const Vector2& other)
	{
		//2D 라서, 스칼라값만 반환을 한다. 
		// 법선 벡터가 필요한게 아니고, 부호를 판단하는 스칼라값이 필요하다.
		return x * other.y - y * other.x;
	}
};

struct AttackInfo
{
	ECollisionLayer _attackLayer;
	vector<class Collider*> _hitActors;
	Vector2 vAttackDir;
	float fWidth;
	float fHeight;
	float fAttackRadian = 0.f;
};

struct TileInfo
{
	int32 iTilesetIndex;
	Vector2 vTilePos;
};

struct CollisionInfo
{
	ECollisionLayer collisionLayer;
	bool isColliding;           // 바닥과 충돌했는지
	Vector2 collisionPoint;     // 충돌 지점
	Vector2 vHitNormal;            // 바닥의 법선 벡터 (위쪽 방향)
	float penetrationDepth;    // 바닥에 파묻힌 깊이
	class Actor* collisionActor;        // 충돌한 액터
	int32 hitCorner;
};

// 선분과 사각형의 충돌체크
bool LineIntersectsAABB(Vector2 p0, Vector2 p1, const RECT& rect, Vector2& outNormal, Vector2& outPos, float& t);
// 선분과 선분의 충돌체크
bool LineIntersectsLine(Vector2 p0, Vector2 p1, Vector2 q0, Vector2 q1, Vector2& outNormal, Vector2& outPos, float& t);
// 박스 회전
vector<Vector2> GetRotatedCorners(float centerX, float centerY, float radian, int imgWidth, int imgHeight);
// 축 투영
Vector2 ProjectionAxis(const vector<Vector2>& vertices, const Vector2& axis);