#pragma once

#include "../Utils/Singleton.h"

enum class KeyState
{
	None,
	Pressed,
	Down,
	Up,

	End
};

enum class KeyType
{
	LeftMouse = VK_LBUTTON,
	RightMouse = VK_RBUTTON,

	Up = VK_UP,
	Down = VK_DOWN,
	Left = VK_LEFT,
	Right = VK_RIGHT,
	SpaceBar = VK_SPACE,
	ESC = VK_ESCAPE,
	LeftShift = VK_LSHIFT,

	KEY_1 = '1',
	KEY_2 = '2',
	KEY_3 = '3',

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',
	L = 'L',
	Q = 'Q',
	E = 'E',
	T = 'T',

	F1 = VK_F1,
	F2 = VK_F2,
};

// 언리얼의 방식
constexpr int32 KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX) + 1;

struct KeyLog
{
	KeyState _state;
	KeyType _type;
	double _inputTime = 0.f;
};

struct MouseLog
{
	POINT _pos = {};
	double _inputTime = 0.f;
};

class InputManager : public Singleton<InputManager>
{
	friend Singleton<InputManager>;

private:
	InputManager(){}

	HWND _hwnd = 0;
	HWND _subHwnd = 0;
	vector<KeyState> _states;
	POINT _mousePos = {};
	POINT _subMousePos = {};

	queue<KeyLog> _keyLogs[KEY_TYPE_COUNT];
	queue<MouseLog> _mouseLogs;

	double _recordingTime;

	bool isReplay = false;

	KeyState GetState(KeyType key) 
	{
		return _states[static_cast<uint8>(key)];
	}

	void ResetStates();

public:
	void Init(HWND hwnd, HWND subHwnd);
	void Update(float deltaTime);
	void ReplayUpdate();

	// 누르고 있을 때
	bool GetButtonPressed(KeyType key) { return GetState(key) == KeyState::Pressed; }

	// 맨 처음 눌렀을 때
	bool GetButtonDown(KeyType key) { return GetState(key) == KeyState::Down; }

	// 맨 처음 눌렀다가 땔 때
	bool GetButtonUp(KeyType key) { return GetState(key) == KeyState::Up; }

	KeyState GetNextKeyState(KeyState prevState, bool isPressed);

	bool IsHeld(KeyState state);

	// 마우스 좌표 얻어오기
	POINT GetMousePos() { return _mousePos; }
	POINT GetSubMousePos() { return _subMousePos; }

	void ClearInputLog();

	bool GetIsReplay() { return isReplay; }
	void SetIsReplay(bool replay);

	void UpdateMousePositions(HWND hwnd, HWND subHwnd, POINT& mousePos, POINT& subMousePos);
};

