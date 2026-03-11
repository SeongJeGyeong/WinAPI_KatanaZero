#include "pch.h"
#include "EnemyState.h"
#include "../Objects/Actors/Enemy.h"
#include "../Components/Animator.h"
#include "../Components/EnemyMovementComponent.h"
#include "../Managers/SoundManager.h"

void EnemyState::UpdateState(float deltaTime)
{
	if (_enemy == nullptr) return;
	_enemy->UpdateAttachedComponents(deltaTime);
}

///////////////// Idle /////////////////
void EnemyState_Idle::EnterState()
{
	_enemy->GetComponent<Animator>()->SetAnimation(EEnemyState::ENEMY_IDLE);
}

void EnemyState_Idle::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);

	if (_enemy->GetDetected())
	{
		fWaitTime = 0.f;
		_enemy->ChangeState(EEnemyState::ENEMY_RUN);
	}

	fWaitTime += deltaTime;
	if (fWaitTime > 1.f)
	{
		fWaitTime = 0.f;
		_enemy->ChangeState(EEnemyState::ENEMY_TURN);
	}
}

void EnemyState_Idle::ExitState()
{
}

///////////////// Walk /////////////////
void EnemyState_Walk::EnterState()
{
	_enemy->GetComponent<Animator>()->SetAnimation(EEnemyState::ENEMY_WALK);
}

void EnemyState_Walk::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);

	if (_enemy->GetDetected())
	{
		fPatrolTime = 0.f;
		_enemy->ChangeState(EEnemyState::ENEMY_RUN);
	}

	fPatrolTime += deltaTime;
	if (fPatrolTime > 3.f)
	{
		fPatrolTime = 0.f;
		_enemy->ChangeState(EEnemyState::ENEMY_IDLE);
	}
	else
	{
		Vector2 dir = _enemy->GetFrontDir();
		_enemy->GetComponent<EnemyMovementComponent>()->SetVelocityX(dir.x * 100.f);
	}

	fFootStepPlay += deltaTime;
	if (fFootStepPlay > 0.3f)
	{
		fFootStepPlay = 0.f;
		SoundManager::GetInstance()->PlayEnemyWalk();
	}

}

void EnemyState_Walk::ExitState()
{
}

///////////////// Turn /////////////////
void EnemyState_Turn::EnterState()
{
	_enemy->GetComponent<Animator>()->SetAnimation(EEnemyState::ENEMY_TURN);
	Vector2 dir = _enemy->GetFrontDir();
	if (dir.x > 0.f)
	{
		_enemy->GetComponent<Animator>()->SetFlipped(true);
	}
	else
	{
		_enemy->GetComponent<Animator>()->SetFlipped(false);
	}
}

void EnemyState_Turn::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	Animator* animator = _enemy->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		if (_enemy->GetDetected())
		{
			_enemy->ChangeState(EEnemyState::ENEMY_RUN);
		}
		else
		{
			_enemy->ChangeState(EEnemyState::ENEMY_WALK);
		}
	}
}

void EnemyState_Turn::ExitState()
{
	_enemy->TurnFrontDir();
}

///////////////// Run /////////////////
void EnemyState_Run::EnterState()
{
	_enemy->GetComponent<Animator>()->SetAnimation(EEnemyState::ENEMY_RUN);
}

void EnemyState_Run::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);

	if (_enemy->InShotRange())
	{
		_enemy->ChangeState(EEnemyState::ENEMY_AIM);
		return;
	}

	if (_enemy->PlayerIsBack())
	{
		_enemy->ChangeState(EEnemyState::ENEMY_TURN);
	}
	else
	{
		Vector2 dir = _enemy->GetFrontDir();
		_enemy->GetComponent<EnemyMovementComponent>()->SetVelocityX(dir.x * 400.f);
	}

	fFootStepPlay += deltaTime;
	if (fFootStepPlay > 0.35f)
	{
		fFootStepPlay = 0.f;
		SoundManager::GetInstance()->PlayEnemyRun();
	}
}

void EnemyState_Run::ExitState()
{
}

///////////////// Aim /////////////////
void EnemyState_Aim::EnterState()
{
	_enemy->GetComponent<Animator>()->SetAnimation(EEnemyState::ENEMY_AIM);
}

void EnemyState_Aim::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);

	if (!_enemy->InShotRange())
	{
		fReloadTime = 0.f;
		_enemy->ChangeState(EEnemyState::ENEMY_RUN);
		return;
	}

	if (_enemy->PlayerIsBack())
	{
		fReloadTime = 0.f;
		_enemy->ChangeState(EEnemyState::ENEMY_TURN);
		return;
	}

	fReloadTime += deltaTime;
	if (fReloadTime >= 1.f)
	{
		fReloadTime = 0.f;
		_enemy->ShotBullet();
	}
}

void EnemyState_Aim::ExitState()
{
}

///////////////// Hurt Fly /////////////////
void EnemyState_Hurt_Fly::EnterState()
{
	_enemy->GetComponent<Animator>()->SetAnimation(EEnemyState::ENEMY_HURT_FLY);
}

void EnemyState_Hurt_Fly::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	EnemyMovementComponent* movementComp = _enemy->GetComponent<EnemyMovementComponent>();
	if (movementComp->GetVelocity().x <= 1.f && 
		(movementComp->GetOnGround() || movementComp->GetOnStair() || movementComp->GetOnPlatform()))
	{
		_enemy->ChangeState(EEnemyState::ENEMY_HURT_GROUND);
	}
}

void EnemyState_Hurt_Fly::ExitState()
{
}

///////////////// Hurt Ground /////////////////
void EnemyState_Hurt_Ground::EnterState()
{
	_enemy->GetComponent<Animator>()->SetAnimation(EEnemyState::ENEMY_HURT_GROUND);
}

void EnemyState_Hurt_Ground::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
}

void EnemyState_Hurt_Ground::ExitState()
{
}
