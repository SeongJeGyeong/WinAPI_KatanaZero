#pragma once
#include "State.h"
#include "../Objects/Actors/Actor.h"

template<typename EnumType>
class StateMachine
{
public:
	StateMachine(Actor* owner) : _owner(owner) {}
	~StateMachine()
	{
		for (auto& state : _stateMap)
		{
			SAFE_DELETE(state.second);
		}
		_stateMap.clear();
		_currentState = nullptr;
		_owner = nullptr;
	}

private:
	Actor* _owner = nullptr;

	State<EnumType>* _currentState = nullptr;

	EnumType _currentStateType;
	map<EnumType, State<EnumType>*> _stateMap;

public:
	void Update(float deltaTime)
	{
		if (_currentState) _currentState->UpdateState(deltaTime);
	}

	void AddState(State<EnumType>* state)
	{
		if (state == nullptr) return;
		// 이미 맵에 존재함
		if (_stateMap.find(state->GetStateType()) != _stateMap.end()) return;

		_stateMap.emplace(state->GetStateType(), state);
	}

	void ChangeState(EnumType stateType)
	{
		if (_currentState && _currentState->GetStateType() == stateType) return;

		if (_currentState) _currentState->ExitState();

		auto it = _stateMap.find(stateType);
		if (it == _stateMap.end()) return;

		_currentState = it->second;
		_currentState->EnterState();
		_currentStateType = stateType;
	}

	EnumType GetCurrentStateType() const
	{
		return _currentStateType;
	}
};