#pragma once
#include "State.h"

class Enemy;

class EnemyState : public State<EEnemyState>
{
public:
	EnemyState(Enemy* enemy) : _enemy(enemy) {}

protected:
	Enemy* _enemy = nullptr;

public:
	virtual EEnemyState GetStateType() override { return EEnemyState::ENEMY_IDLE; };

	virtual void EnterState() override {}
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override {}
};

class EnemyState_Idle : public EnemyState
{
	using Super = EnemyState;
private:
	float fWaitTime = 0;

public:
	EnemyState_Idle(Enemy* enemy) : Super(enemy) {}

	virtual EEnemyState GetStateType() override { return EEnemyState::ENEMY_IDLE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class EnemyState_Walk : public EnemyState
{
	using Super = EnemyState;
private:
	float fPatrolTime = 0;
	float fFootStepPlay = 0.f;

public:
	EnemyState_Walk(Enemy* enemy) : Super(enemy) {}

	virtual EEnemyState GetStateType() override { return EEnemyState::ENEMY_WALK; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class EnemyState_Turn : public EnemyState
{
	using Super = EnemyState;
public:
	EnemyState_Turn(Enemy* enemy) : Super(enemy) {}

	virtual EEnemyState GetStateType() override { return EEnemyState::ENEMY_TURN; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class EnemyState_Run : public EnemyState
{
	using Super = EnemyState;
private:
	float fFootStepPlay = 0.f;

public:
	EnemyState_Run(Enemy* enemy) : Super(enemy) {}

	virtual EEnemyState GetStateType() override { return EEnemyState::ENEMY_RUN; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class EnemyState_Aim : public EnemyState
{
	using Super = EnemyState;
private:
	float fReloadTime = 0.f;

public:
	EnemyState_Aim(Enemy* enemy) : Super(enemy) {}

	virtual EEnemyState GetStateType() override { return EEnemyState::ENEMY_AIM; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class EnemyState_Hurt_Fly : public EnemyState
{
	using Super = EnemyState;
public:
	EnemyState_Hurt_Fly(Enemy* enemy) : Super(enemy) {}

	virtual EEnemyState GetStateType() override { return EEnemyState::ENEMY_HURT_FLY; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class EnemyState_Hurt_Ground : public EnemyState
{
	using Super = EnemyState;
public:
	EnemyState_Hurt_Ground(Enemy* enemy) : Super(enemy) {}

	virtual EEnemyState GetStateType() override { return EEnemyState::ENEMY_HURT_GROUND; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};