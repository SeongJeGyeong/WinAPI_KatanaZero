#include "pch.h"
#include "InputManager.h"

void InputManager::Init(HWND hwnd, HWND subHwnd)
{
	_hwnd = hwnd;
	_subHwnd = subHwnd;
	_states.resize(KEY_TYPE_COUNT, KeyState::None);
	_recordingTime = 0.0;
}

void InputManager::ResetStates()
{
	for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		_states[key] = KeyState::None;
	}
}

void InputManager::Update(float deltaTime)
{
	if (isReplay)
	{
		ReplayUpdate();
		return;
	}

	_recordingTime += deltaTime;
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (!::GetKeyboardState(asciiKeys)) return;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		// 키가 눌린 상태이고, 이전 프레임에도 동일한 키를 누른 상태였는지 체크
		_states[key] = GetNextKeyState(_states[key], (asciiKeys[key] & 0x80) != 0);

		KeyLog log;
		log._state = _states[key];
		log._type = (KeyType)key;
		log._inputTime = _recordingTime;
		_keyLogs[key].push(log);
	}

	UpdateMousePositions(_hwnd, _subHwnd, _mousePos, _subMousePos);

	MouseLog mouseLog = { _mousePos, _recordingTime };
	_mouseLogs.push(mouseLog);
}

void InputManager::ReplayUpdate()
{
	bool hasRemainingLog = false;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		if (_keyLogs[key].empty())
		{
			_states[key] = KeyState::None;
			continue;
		}

		_states[key] = _keyLogs[key].front()._state;
		_keyLogs[key].pop();
		// 둘 중 하나라도 true면 true
		hasRemainingLog |= !_keyLogs[key].empty();
	}

	if (!_mouseLogs.empty())
	{
		_mousePos = _mouseLogs.front()._pos;
		_mouseLogs.pop();
	}

	hasRemainingLog |= !_mouseLogs.empty();

	if (!hasRemainingLog)
	{
		isReplay = false;
		ClearInputLog();

		ResetStates();
	}
}

KeyState InputManager::GetNextKeyState(KeyState prevState, bool isPressed)
{
	if (isPressed) return IsHeld(prevState) ? KeyState::Pressed : KeyState::Down;

	return IsHeld(prevState) ? KeyState::Up : KeyState::None;
}

bool InputManager::IsHeld(KeyState state)
{
	return state == KeyState::Down || state == KeyState::Pressed;
}

void InputManager::ClearInputLog()
{
	for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
		_keyLogs[key] = queue<KeyLog>();

	_mouseLogs = queue<MouseLog>();
	_recordingTime = 0.0;
}

void InputManager::SetIsReplay(bool replay)
{
	isReplay = replay;
	if (!isReplay) return;

	_recordingTime = 0.0;

	ResetStates();
}

void InputManager::UpdateMousePositions(HWND hwnd, HWND subHwnd, POINT& mousePos, POINT& subMousePos)
{
	POINT screenPos = {};
	if (!::GetCursorPos(&screenPos)) return;

	mousePos = screenPos;
	subMousePos = screenPos;

	::ScreenToClient(hwnd, &mousePos);
	::ScreenToClient(subHwnd, &subMousePos);
}