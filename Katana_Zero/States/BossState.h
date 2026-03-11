#pragma once
#include "State.h"

class Boss;
class Animator;

class BossState : public State<EBossState>
{
public:
	BossState(Boss* boss);

protected:
	Boss* _boss = nullptr;
	Animator* _animator = nullptr;
public:
	virtual EBossState GetStateType() override { return EBossState::BOSS_READY; };

	virtual void EnterState() override {}
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override {}
};

class BossState_Ready : public BossState
{
	using Super = BossState;
public:
	BossState_Ready(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_READY; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_ToBattle : public BossState
{
	using Super = BossState;
public:
	BossState_ToBattle(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_TOBATTLE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Idle : public BossState
{
	using Super = BossState;
private:
	bool bPrevMelee = false;
	bool bSwitchPattern = false;
	float fWaitTime = 0.f;
public:
	BossState_Idle(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_IDLE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_PreLunge : public BossState
{
	using Super = BossState;
public:
	BossState_PreLunge(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_PRELUNGE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Lunge : public BossState
{
	using Super = BossState;
public:
	BossState_Lunge(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_LUNGE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_LungeAttack : public BossState
{
	using Super = BossState;
public:
	BossState_LungeAttack(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_LUNGEATTACK; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_PreJump : public BossState
{
	using Super = BossState;
private:
	float fChargeTime = 0.f;
public:
	BossState_PreJump(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_PREJUMP; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Jump : public BossState
{
	using Super = BossState;
public:
	BossState_Jump(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_JUMP; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_JumpAttack : public BossState
{
	using Super = BossState;
public:
	BossState_JumpAttack(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_JUMPATTACK; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Land : public BossState
{
	using Super = BossState;
public:
	BossState_Land(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_LAND; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_ThrowAxe : public BossState
{
	using Super = BossState;
private:
	bool bDoOnce = false;
public:
	BossState_ThrowAxe(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_THROWAXE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_TugAxe : public BossState
{
	using Super = BossState;
public:
	BossState_TugAxe(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_TUGAXE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_ReturnAxe : public BossState
{
	using Super = BossState;
public:
	BossState_ReturnAxe(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_RETURNAXE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Block : public BossState
{
	using Super = BossState;
public:
	BossState_Block(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_BLOCK; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Hurt : public BossState
{
	using Super = BossState;
private:
	float fRecoverTime = 0.f;
public:
	BossState_Hurt(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_HURT; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Recorver : public BossState
{
	using Super = BossState;
public:
	BossState_Recorver(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_RECOVER; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Struggle : public BossState
{
	using Super = BossState;
private:
	float fStruggleTime = 0.f;
public:
	BossState_Struggle(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_STRUGGLE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Finished : public BossState
{
	using Super = BossState;
public:
	BossState_Finished(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_FINISHED; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Defeat : public BossState
{
	using Super = BossState;
public:
	BossState_Defeat(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_DEFEAT; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class BossState_Die : public BossState
{
	using Super = BossState;
public:
	BossState_Die(Boss* boss) : Super(boss) {}

	virtual EBossState GetStateType() override { return EBossState::BOSS_DIE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};