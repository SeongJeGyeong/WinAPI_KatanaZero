#pragma once
#include "../Utils/Singleton.h"

class Scene;
class EditorSub;
class Camera;
class Player;
class UIImage;

class Game : public Singleton<Game>
{
	// protected, private 멤버에도 접근 가능하게 만들어줌
	friend Singleton<Game>;

protected:
	// 생성자 외부에서 사용 못하게 protected에 선언
	Game() {}

private:
	HWND	_hwnd;		// 윈도우 핸들
	HWND	_hwndSub;	// 서브 에디터창 핸들

	HDC		_hdc;		// 화면에 그려지는 정보(프론트 버퍼)

	// 더블 버퍼링
	RECT	_rect;			// 버퍼의 영역
	HDC		_hdcBack = {};	// 백 버퍼
	HBITMAP	_bmpBack = {};	// bitmap
	DWORD	_background = BLACKNESS;

	Scene* _curScene = nullptr;		// 현재 활성화된 씬만 업데이트/그려준다.
	Scene* _nextScene = nullptr;	// 현재 씬이 모두 끝나고 다음 틱에 변경
	EditorSub* _subWindow = nullptr;

	bool bDebugMode = false;

	json _stageInfo;
	string _currentStageName;
	string _nextStageName;

	UIImage* _stageClearImage = nullptr;
	float fStageClearWait = 0.f;
	bool bStageClearFade = false;
	bool bIsStageClear = false;
	bool bSkipReplayAfterClear = false;

	// 리플레이용 버퍼
	HDC		_hdcGray = nullptr;
	HBITMAP	_bmpGray = nullptr;
	BYTE* _grayBits = nullptr;
	BITMAPINFO _grayBmi = {};

	float fFixedTimeStep = 1.f / 60.f;
	float fTimeAccumulator = 0.f;
	int32 iMaxFixedSteps = 8;

public:
	void Init(HWND hwnd, HWND hsubwnd);
	void Destroy();

	void Update();
	void Render();

	void ChangeGameScene(string mapName);
	void ChangeLobbyScene();
	void ChangeEditorScene();
	void ExitGame();
	void RewindGameScene(string mapName);
	void ReplayGameScene();

	HWND GetHwnd() { return _hwnd; }
	HWND GetHwndSub() { return _hwndSub; }
	Scene* GetCurrScene() { return _curScene; }

	bool GetDebugMode() { return bDebugMode; }
	EditorSub* GetSubWindow() { return _subWindow; }

	Vector2 GetCurrentSceneSize();
	Camera* GetCurrentSceneCamera();
	void ShakeSceneCamera();
	Vector2 ConvertCurSceneScreenPos(Vector2 worldPos);

	void StageClear();
};

