#pragma once

template <typename EnumType>
class State
{
public:
	virtual ~State() {}

	virtual EnumType GetStateType() = 0;
	virtual void EnterState() = 0;
	virtual void UpdateState(float deltaTime) = 0;
	virtual void ExitState() = 0;
};