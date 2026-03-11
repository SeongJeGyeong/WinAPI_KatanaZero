#pragma once
#include "State.h"

class Player;
class PlayerMovementComponent;
class Animator;

class PlayerState : public State<EPlayerState>
{
public:
	function<void(float)> OnInputUpdate;

	PlayerState(Player* player);
	virtual ~PlayerState() {}

protected:
	Player* _player = nullptr;
	PlayerMovementComponent* _movementComponent = nullptr;
	Animator* _animator = nullptr;

public:
	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_IDLE; };

	virtual void EnterState() override {}
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override {}
};

class PlayerState_Idle : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Idle(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_IDLE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Idle_to_Run : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Idle_to_Run(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_IDLE_TO_RUN; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Run : public PlayerState
{
	using Super = PlayerState;
private:
	float fFootStepPlay = 0.f;
public:
	PlayerState_Run(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_RUN; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Run_to_Idle : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Run_to_Idle(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_RUN_TO_IDLE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_PreCrouch : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_PreCrouch(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_PRECROUCH; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Crouch : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Crouch(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_CROUCH; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_PostCrouch : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_PostCrouch(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_POSTCROUCH; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Jump: public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Jump(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_JUMP; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Fall : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Fall(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_FALL; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Attack : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Attack(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_ATTACK; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Roll : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Roll(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_ROLL; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Struggle : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Struggle(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_STRUGGLE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_HurtBegin : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_HurtBegin(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_HURT_BEGIN; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_HurtFly : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_HurtFly(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_HURT_LOOP; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_HurtGround : public PlayerState
{
	using Super = PlayerState;
private:
	float fRecoverTime = 0.f;
public:
	PlayerState_HurtGround(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_HURT_GROUND; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Recover : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Recover(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_HURT_RECOVER; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Finish : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Finish(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_FINISH; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_WallSlide : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_WallSlide(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_WALLSLIDE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_WallJump : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_WallJump(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_WALLJUMP; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};