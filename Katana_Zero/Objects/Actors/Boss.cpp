#include "pch.h"
#include "Boss.h"
#include "Game/Game.h"
#include "Managers/ResourceManager.h"
#include "Managers/InputManager.h"
#include "Managers/CollisionManager.h"
#include "Managers/TimeManager.h"
#include "Components/Colliders/Collider.h"
#include "Components/Animator.h"
#include "Components/EffectorComponent.h"
#include "Components/BossMovementComponent.h"
#include "States/StateMachine.h"
#include "States/BossState.h"
#include "Resources/Texture.h"
#include "Player.h"

Boss::~Boss()
{
	SAFE_DELETE(_stateMachine);
}

void Boss::Init(Vector2 pos)
{
	Super::Init(pos);

	CreateCollider<AABBCollisionShape>(ECollisionLayer::ENEMY, 50.f, 100.f);
	_components.AddComponent<Animator>();
	_components.AddComponent<EffectorComponent>();
	_components.AddComponent<BossMovementComponent>();

	Animator* animator = _components.GetComponent<Animator>();
	if (animator != nullptr)
	{
		animator->SetOwner(this);
		animator->InitComponent(EBossState::BOSS_END);
		animator->AddAnimation(EBossState::BOSS_READY, "spr_kissyface_casual_idle", {0, 0});
		animator->AddAnimation(EBossState::BOSS_TOBATTLE, "spr_kissyface_tobattle", {-12, -14});
		animator->AddAnimation(EBossState::BOSS_IDLE, "spr_kissyface_idle", {0, -1});
		animator->AddAnimation(EBossState::BOSS_BLOCK, "spr_kissyface_block", { 0, 2 });
		animator->AddAnimation(EBossState::BOSS_HURT, "spr_kissyface_hurt", { 10, 3 });
		animator->AddAnimation(EBossState::BOSS_RECOVER, "spr_kissyface_recover", { 14, 3 });
		animator->AddAnimation(EBossState::BOSS_STRUGGLE, "spr_kissyface_struggle", { 0, 13 });
		animator->AddAnimation(EBossState::BOSS_FINISHED, "spr_kissyface_die", { 21, -7 });
		animator->AddAnimation(EBossState::BOSS_DEFEAT, "spr_kissyface_dead", { 16, 23 });
		animator->AddAnimation(EBossState::BOSS_DIE, "spr_kissyface_nohead", { 16, 29 });
		animator->AddAnimation(EBossState::BOSS_PRELUNGE, "spr_kissyface_prelunge", { 16, 5 });
		animator->AddAnimation(EBossState::BOSS_LUNGE, "spr_kissyface_lunge", { 50, 0 });
		animator->AddAnimation(EBossState::BOSS_LUNGEATTACK, "spr_kissyface_lungeattack", { 50, -5 });
		animator->AddAnimation(EBossState::BOSS_PREJUMP, "spr_kissyface_prejump", { 0, 0 });
		animator->AddAnimation(EBossState::BOSS_JUMP, "spr_kissyface_jump", { 0, -20 });
		animator->AddAnimation(EBossState::BOSS_JUMPATTACK, "spr_kissyface_jump_swing", { 0, 0 });
		animator->AddAnimation(EBossState::BOSS_LAND, "spr_kissyface_landattack", { 0, 0 });

		animator->AddAnimation(EBossState::BOSS_THROWAXE, "spr_kissyface_throw", { 2, -5 });
		animator->AddAnimation(EBossState::BOSS_TUGAXE, "spr_kissyface_tug", { 47, 9 });
		animator->AddAnimation(EBossState::BOSS_RETURNAXE, "spr_kissyface_returnaxe", { -10, 7 });
	}

	EffectorComponent* effector = _components.GetComponent<EffectorComponent>();
	if (effector != nullptr)
	{
		effector->InitComponent();
		effector->SetOwner(this);
	}

	_components.GetComponent<BossMovementComponent>()->InitComponent(this);

	_stateMachine = new StateMachine<EBossState>(this);
	_stateMachine->AddState(new BossState_Ready(this));
	_stateMachine->AddState(new BossState_ToBattle(this));
	_stateMachine->AddState(new BossState_Idle(this));
	_stateMachine->AddState(new BossState_PreLunge(this));
	_stateMachine->AddState(new BossState_Lunge(this));
	_stateMachine->AddState(new BossState_LungeAttack(this));
	_stateMachine->AddState(new BossState_PreJump(this));
	_stateMachine->AddState(new BossState_Jump(this));
	_stateMachine->AddState(new BossState_JumpAttack(this));
	_stateMachine->AddState(new BossState_Land(this));
	_stateMachine->AddState(new BossState_ThrowAxe(this));
	_stateMachine->AddState(new BossState_TugAxe(this));
	_stateMachine->AddState(new BossState_ReturnAxe(this));
	_stateMachine->AddState(new BossState_Block(this));
	_stateMachine->AddState(new BossState_Hurt(this));
	_stateMachine->AddState(new BossState_Recorver(this));
	_stateMachine->AddState(new BossState_Struggle(this));
	_stateMachine->AddState(new BossState_Finished(this));
	_stateMachine->AddState(new BossState_Defeat(this));
	_stateMachine->AddState(new BossState_Die(this));
	_stateMachine->ChangeState(EBossState::BOSS_READY);

	_components.GetComponent<BossMovementComponent>()->InitComponent(this);

	animator->SetFlipped(true);
	vFrontDir = { -1, 0 };

	_progressBackground = ResourceManager::GetInstance()->GetTexture("progress_background");
	_progressCurrent = ResourceManager::GetInstance()->GetTexture("progress_gauge");
	_progressPrev = ResourceManager::GetInstance()->GetTexture("progress_prevgauge");
}

void Boss::Update(float deltaTime)
{
	if (TimeManager::GetInstance()->GetHitStop()) return;
	_stateMachine->Update(deltaTime);
	if (GetCurrentState() == EBossState::BOSS_STRUGGLE && InputManager::GetInstance()->GetButtonPressed(KeyType::LeftMouse))
	{
		if (fStruggleGauge < fPrevGauge)
		{
			fStruggleGauge += 60 * deltaTime;
		}
		else
		{
			fStruggleGauge += 10 * deltaTime;
		}
	}
	if (fStruggleGauge >= 100.f)
	{
		Defeat();
	}

	if (bAttack)
	{
		Vector2 pos = GetPos();
		pos.x += vFrontDir.x * 50.f;
		bAttack = CollisionManager::GetInstance()->CheckAABBHitBox(this, pos, 150.f ,100.f);
	}
}

void Boss::PostUpdate(float deltaTime)
{
	SetPos(_components.GetComponent<BossMovementComponent>()->GetNewPos());
	_components.GetComponent<BossMovementComponent>()->SetAcceleration({ 0, 0 });
}

void Boss::Render(HDC hdc)
{
	_components.RenderComponents(hdc);
	Super::Render(hdc);
	if (GetCurrentState() == EBossState::BOSS_STRUGGLE)
	{
		Vector2 pos = GetPos();
		pos.y -= 50.f;
		pos.x -= 50.f;
		_progressBackground->RenderProgress(hdc, pos, 1.f, 100, 10);
		_progressPrev->RenderProgress(hdc, pos, fPrevGauge * 0.01f, (int32)fPrevGauge, 10);
		_progressCurrent->RenderProgress(hdc, pos, fStruggleGauge * 0.01f,(int32)fStruggleGauge, 10);
	}

	/*if (GetCurrentState() == EBossState::BOSS_LUNGE || GetCurrentState() == EBossState::BOSS_LUNGEATTACK)
	{
		Vector2 pos = Game::GetInstance()->ConvertCurSceneScreenPos(GetPos());

		pos.x += vFrontDir.x * 50.f;

		int32 left = pos.x - 150 * 0.5f;
		int32 right = pos.x + 150 * 0.5f;
		int32 top = pos.y - 100 * 0.5f;
		int32 bottom = pos.y + 100 * 0.5f;

		HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		HPEN oldPen = (HPEN)SelectObject(hdc, redPen);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, left, top, right, bottom);
		(HPEN)SelectObject(hdc, oldPen);
		DeleteObject(redPen);
	}*/
}

int32 Boss::GetCurrentState()
{
	return _stateMachine->GetCurrentStateType();
}

void Boss::ChangeState(int32 stateType)
{
	_stateMachine->ChangeState(static_cast<EBossState>(stateType));
}

bool Boss::ToBattle()
{
	float length = (_targetPlayer->GetPos() - GetPos()).LengthSquared();
	return length <= fDetectRange * fDetectRange;
}

void Boss::StartStruggle()
{
	ChangeState(EBossState::BOSS_STRUGGLE);
	Vector2 pos = GetPos();
	_targetPlayer->SetPos({ pos.x + (vFrontDir.x * 50.f), pos.y });
	_targetPlayer->ChangeState(EPlayerState::PLAYER_STRUGGLE);
}

void Boss::EndStruggle()
{
	_targetPlayer->ChangeState(EPlayerState::PLAYER_HURT_BEGIN);
	_targetPlayer->AddForce({ vFrontDir.x * 2000.f, -150.f });
	if (fStruggleGauge > fPrevGauge) fPrevGauge = fStruggleGauge;
	fStruggleGauge = 0.f;
	SetCanStruggling(false);
}

void Boss::Defeat()
{
	fPrevGauge = 0.f;
	fStruggleGauge = 0.f;
	ChangeState(EBossState::BOSS_FINISHED);
	Vector2 pos = GetPos();
	_targetPlayer->SetPos({ pos.x + (vFrontDir.x * 60.f), pos.y + 25.f });
	_targetPlayer->ChangeState(EPlayerState::PLAYER_FINISH);
}

void Boss::EndFinishMotion()
{
	_targetPlayer->ChangeState(EPlayerState::PLAYER_IDLE);
}

void Boss::Die()
{
	CollisionManager::GetInstance()->DeleteCollider(GetCollider());
	ChangeState(EBossState::BOSS_DIE);
}

void Boss::UpdateDirection()
{
	Vector2 dir = _targetPlayer->GetPos() - GetPos();
	if (dir.x >= 0)
	{
		vFrontDir.x = 1;
		_components.GetComponent<Animator>()->SetFlipped(false);
	}
	else
	{
		vFrontDir.x = -1;
		_components.GetComponent<Animator>()->SetFlipped(true);
	}
}

void Boss::ThrowAxe()
{
	if (GetCurrentState() == EBossState::BOSS_THROWAXE)
	{
		Vector2 pos = GetPos();
		pos.x += vFrontDir.x * 100.f;
		pos.y -= 15.f;
		OnSpawnAxe(pos, this, vFrontDir, true);
	}
	else
	{
		Vector2 pos = GetPos();
		pos.x += 100.f;
		pos.y -= 15.f;
		OnSpawnAxe(pos, this, vFrontDir, false);
	}
}

void Boss::ReturnAxe()
{
	if (GetCurrentState() == EBossState::BOSS_TUGAXE)
	{
		ChangeState(EBossState::BOSS_RETURNAXE);
	}
}

void Boss::Lunge()
{
	Vector2 targetPos = _targetPlayer->GetPos();
	targetPos.x += -vFrontDir.x * 80.f;
	_components.GetComponent<BossMovementComponent>()->ParabolicJump(targetPos);
	bAttack = true;
}

void Boss::Hurt(Vector2 attackDir)
{
	if ((attackDir.x > 0 && vFrontDir.x > 0) || (attackDir.x < 0 && vFrontDir.x < 0))
	{
		vFrontDir.x = -vFrontDir.x;
		_components.GetComponent<Animator>()->FlipAnimation();
	}

	ChangeState(EBossState::BOSS_HURT);
	Vector2 knockbackDir = { -vFrontDir.x * 1000.f, -500.f };
	_components.GetComponent<BossMovementComponent>()->SetVelocity(knockbackDir);

	TimeManager::GetInstance()->SetHitStop();
	Game::GetInstance()->ShakeSceneCamera();
}

void Boss::TakeDamage(Actor* damageCauser, const Vector2& attackDirection)
{
	if (bCanStruggling)
	{
		StartStruggle();
		return;
	}
	if (GetCurrentState() == EBossState::BOSS_DEFEAT)
	{
		Die();
		return;
	}

	if (GetCurrentState() == EBossState::BOSS_JUMP || GetCurrentState() == EBossState::BOSS_JUMPATTACK || GetCurrentState() == EBossState::BOSS_TUGAXE)
	{
		Hurt(attackDirection);
	}
	else if ((attackDirection.x > 0 && vFrontDir.x > 0) || (attackDirection.x < 0 && vFrontDir.x < 0))
	{
		Hurt(attackDirection);
	}
	else if (GetCurrentState() == EBossState::BOSS_LUNGE || GetCurrentState() == EBossState::BOSS_LUNGEATTACK)
	{
		Vector2 reflectVector = { -attackDirection.x, -attackDirection.y };
		_targetPlayer->AddForce(reflectVector * 500.f);
		_targetPlayer->AttackBlocked();
	}
	else
	{
		Vector2 reflectVector = { -attackDirection.x, -attackDirection.y };
		_targetPlayer->AddForce(reflectVector * 500.f);
		_targetPlayer->AttackBlocked();
		ChangeState(EBossState::BOSS_BLOCK);
		Game::GetInstance()->ShakeSceneCamera();
	}
}

void Boss::OnCollisionBeginOverlap(const CollisionInfo& info)
{
	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
		ProcessGroundCollision(info);
		break;
	case ECollisionLayer::WALL:
		ProcessWallCollision(info);
		break;
	case ECollisionLayer::PLAYER_HITBOX:
		TakeDamage(info.collisionActor, info.vHitNormal);
		break;
	default:
		break;
	}
}

void Boss::OnCollisionStayOverlap(const CollisionInfo& info)
{
	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
		ProcessGroundCollision(info);
		break;
	case ECollisionLayer::WALL:
		ProcessWallCollision(info);
		break;
	default:
		break;
	}
}

void Boss::OnCollisionEndOverlap(const CollisionInfo& info)
{
	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
		_components.GetComponent<BossMovementComponent>()->SetOnGround(false);
		break;
	default:
		break;
	}
}

void Boss::ProcessGroundCollision(const CollisionInfo& collisionInfo)
{
	BossMovementComponent* movementComp = _components.GetComponent<BossMovementComponent>();

	switch (collisionInfo.hitCorner)
	{
	case 0:
		if (movementComp->GetVelocity().y < 0.f) break;
		movementComp->SetNewPosY(collisionInfo.collisionPoint.y - GetCollider()->GetHeight() * 0.5f);
		movementComp->SetVelocityY(0.f);
		movementComp->SetOnGround(true);
		break;
	case 3:
		movementComp->SetNewPosY(collisionInfo.collisionPoint.y + GetCollider()->GetHeight() * 0.5f);
		movementComp->SetVelocityY(0.f);
		break;
	case 1:
		{
			float halfWidth = GetCollider()->GetWidth() * 0.5f;
			movementComp->SetNewPosX(collisionInfo.collisionPoint.x + halfWidth);
			movementComp->SetVelocityX(0.f);
			break;
		}
	case 2:
		{
			float halfWidth = GetCollider()->GetWidth() * 0.5f;
			movementComp->SetNewPosX(collisionInfo.collisionPoint.x - halfWidth);
			movementComp->SetVelocityX(0.f);
			break;
		}
	}
}

void Boss::ProcessWallCollision(const CollisionInfo& collisionInfo)
{
	BossMovementComponent* movementComp = _components.GetComponent<BossMovementComponent>();

	Vector2 velocity = movementComp->GetVelocity();

	if ((velocity.x > 0.f && collisionInfo.vHitNormal.x > 0) || (velocity.x < 0.f && collisionInfo.vHitNormal.x < 0)) return;

	float halfWidth = GetCollider()->GetWidth() * 0.5f;
	// 벽 방향에 따른 위치 조정
	if (collisionInfo.vHitNormal.x < 0) // 오른쪽 벽
	{
		movementComp->SetNewPosX(collisionInfo.collisionPoint.x - halfWidth);
	}
	else if (collisionInfo.vHitNormal.x > 0)// 왼쪽 벽
	{
		movementComp->SetNewPosX(collisionInfo.collisionPoint.x + halfWidth);
	}

	movementComp->SetVelocityX(0.f);
}

float Boss::CheckDistance()
{
	Vector2 dir = _targetPlayer->GetPos() - GetPos();
	return dir.Length();
}

Vector2 Boss::GetNewPos()
{
	return _components.GetComponent<BossMovementComponent>()->GetNewPos();
}

bool Boss::TargetNotFound()
{
	return (_targetPlayer == nullptr || _targetPlayer->GetIsDead());
}

void Boss::SetCanStruggling(bool struggling)
{
	bCanStruggling = struggling;
	if (bCanStruggling)
	{
		Vector2 pos = GetPos();
		pos.y -= 70.f;
		GetComponent<EffectorComponent>()->PlayEffect("spr_left_click_anim", false, 0.f, 1.f, false, pos);
	}
	else
	{
		GetComponent<EffectorComponent>()->RemoveLoopEffect("spr_left_click_anim", false);
	}
}
