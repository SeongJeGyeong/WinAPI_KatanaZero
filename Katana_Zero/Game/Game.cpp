#include "pch.h"
#include "Game.h"
#include "../Managers/InputManager.h"
#include "../Managers/TimeManager.h"
#include "../Managers/ResourceManager.h"
#include "../Scenes/Scene.h"
#include "../Scenes/LobbyScene.h"
#include "../Scenes/EditorScene.h"
#include "../Scenes/EditorSub.h"
#include "../Scenes/GameScene.h"
#include "../Managers/CollisionManager.h"
#include "../Objects/Camera.h"
#include "../Managers/SoundManager.h"
#include "../Managers/GamePlayManager.h"
#include "../UI/UIImage.h"

void Game::Init(HWND hwnd, HWND hsubwnd)
{
	_hwnd = hwnd;
	_hwndSub = hsubwnd;
	_hdc = ::GetDC(hwnd);

	// 더블 버퍼링 버퍼 추가
	::GetClientRect(hwnd, &_rect);
	_hdcBack = ::CreateCompatibleDC(_hdc);
	_bmpBack = ::CreateCompatibleBitmap(_hdc, _rect.right, _rect.bottom);

	// 여기다가 생성된 백버퍼 HDC와 게임사이즈에 맞는 텍스쳐를 연결한다.
	HBITMAP prev = (HBITMAP)::SelectObject(_hdcBack, _bmpBack); // DC와 BMP를 연결
	::DeleteObject(prev);

	_hdcGray = ::CreateCompatibleDC(_hdc);

	ZeroMemory(&_grayBmi, sizeof(_grayBmi));
	_grayBmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	_grayBmi.bmiHeader.biWidth = _rect.right;
	_grayBmi.bmiHeader.biHeight = -_rect.bottom;
	_grayBmi.bmiHeader.biPlanes = 1;
	_grayBmi.bmiHeader.biBitCount = 32;
	_grayBmi.bmiHeader.biCompression = BI_RGB;

	_bmpGray = ::CreateDIBSection(_hdc, &_grayBmi, DIB_RGB_COLORS, (void**)&_grayBits, nullptr, 0);
	if (_bmpGray != nullptr)
	{
		HBITMAP prevGray = (HBITMAP)::SelectObject(_hdcGray, _bmpGray);
		::DeleteObject(prevGray);
	}

	// 외부 폰트를 시스템에 임시로 등록
	fs::path directory = fs::current_path() / L"../GameResources/";
	AddFontResourceEx((directory / L"Fonts/DungGeunMo.ttf").c_str(), FR_PRIVATE, 0);

	InputManager::GetInstance()->Init(_hwnd, _hwndSub);
	ResourceManager::GetInstance()->Init(_hwnd, directory);
	SoundManager::GetInstance()->Init(directory);
	CollisionManager::GetInstance()->Init();
	TimeManager::GetInstance()->Init();
	GamePlayManager::GetInstance()->Init();

	ChangeLobbyScene();

	fs::path path = directory / L"Json/StageInfo.json";
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		MessageBox(nullptr, L"Failed to open JSON file", L"Error", MB_OK);
		return;
	}

	_stageInfo = json::parse(file);
	_stageClearImage = new UIImage(Vector2(GWinSizeX * 0.5f, GWinSizeY * 0.5f), "clear_fadeMask", GWinSizeX, GWinSizeY, 1.f, 0);
}

void Game::Destroy()
{
	InputManager::DestroyInstance();
	TimeManager::DestroyInstance();
	ResourceManager::DestroyInstance();
	CollisionManager::DestroyInstance();

	if (_bmpGray)
	{
		::DeleteObject(_bmpGray);
		_bmpGray = nullptr;
	}
	if (_hdcGray)
	{
		::DeleteDC(_hdcGray);
		_hdcGray = nullptr;
	}
	_grayBits = nullptr;

	if (_curScene)
	{
		_curScene->Destroy();
	}
	SAFE_DELETE(_curScene);
}

void Game::Update()
{
	// CurScene 변경 요청이 있으면, 현재 씬을 변경먼저 하고, 해당 씬 업데이트를 수행한다.
	if (_nextScene)
	{
		if (_curScene)
		{
			_curScene->Destroy();
			delete _curScene;
			_curScene = nullptr;
		}

		_curScene = _nextScene;
		_curScene->Init();

		_nextScene = nullptr;
	}

	TimeManager* timeManager = TimeManager::GetInstance();
	timeManager->Update();

	float realDeltaTime = timeManager->GetRealDeltaTime();
	if (realDeltaTime > 0.25f) realDeltaTime = 0.25f; // 큰 프레임 드랍 방어

	fTimeAccumulator += realDeltaTime;

	HWND hwnd = ::GetForegroundWindow();
	bool isMainActive = (_hwnd == hwnd);
	bool isSubActive = (_hwndSub == hwnd);

	int32 stepCount = 0;

	while (fTimeAccumulator >= fFixedTimeStep && stepCount < iMaxFixedSteps)
	{
		// 고정 스텝 진행 -> 내부에서 fDeltaTime/fConstDeltaTime 갱신
		timeManager->FixedUpdate(fFixedTimeStep);
		float deltaTime = timeManager->GetDeltaTime();

		SoundManager::GetInstance()->Update(deltaTime);
		InputManager::GetInstance()->Update(deltaTime);

		// KeyDown 이벤트는 fixed step 내부에서 처리해야 누락이 줄어듦
		if (InputManager::GetInstance()->GetButtonDown(KeyType::F1))
			bDebugMode = !bDebugMode;

		if (isMainActive && _curScene)
		{
			// 스테이지 클리어 연출
			if (bIsStageClear)
			{
				if (bStageClearFade) fStageClearWait += deltaTime;

				if (bStageClearFade && fStageClearWait >= 2.f)
				{
					bStageClearFade = false;
					fStageClearWait = 0.f;
					bIsStageClear = false;

					if (bSkipReplayAfterClear)
					{
						bSkipReplayAfterClear = false;
						GamePlayManager::GetInstance()->ClearBuffer();
						ChangeGameScene(_nextStageName);
					}
					else
					{
						ReplayGameScene();
					}

					fTimeAccumulator = 0.f;
					break;
				}

				_stageClearImage->ImageFade(bStageClearFade, deltaTime * 255.f);

				fTimeAccumulator -= fFixedTimeStep;
				stepCount++;
				continue;
			}

			if (GamePlayManager::GetInstance()->GetIsRewinding())
			{
				fTimeAccumulator = 0.f;
				break;
			}

			_curScene->Update(deltaTime);

			CollisionManager::GetInstance()->Update();
			_curScene->PostUpdate(deltaTime);
			CollisionManager::GetInstance()->PostUpdate();
		}

		fTimeAccumulator -= fFixedTimeStep;
		stepCount++;
	}

	if (stepCount >= iMaxFixedSteps)
		fTimeAccumulator = 0.f;

	if (isSubActive && _subWindow)
		_subWindow->Update();
}

void Game::Render()
{
	if (GamePlayManager::GetInstance()->GetIsRewinding())
	{
		if (GamePlayManager::GetInstance()->RenderRewind(_hdcBack))
		{
			ChangeGameScene(_currentStageName);
		}
	}
	else
	{
		if (_curScene)
		{
			_curScene->Render(_hdcBack);
		}

		if (bIsStageClear)
		{
			_stageClearImage->Render(_hdcBack);
		}

		if (_subWindow)
		{
			_subWindow->Render();
		}

		CollisionManager::GetInstance()->Render(_hdcBack);

		//uint32 fps = TimeManager::GetInstance()->GetFps();
		//float deltaTime = TimeManager::GetInstance()->GetDeltaTime();
		//SetTextColor(_hdcBack, RGB(255, 255, 255));

		//wstring str2 = std::format(L"FPS({0}), DT({1})", fps, deltaTime);
		//::TextOut(_hdcBack, 200, 10, str2.c_str(), static_cast<int32>(str2.size()));
	}

	if (InputManager::GetInstance()->GetIsReplay() && _hdcGray != nullptr && _bmpGray != nullptr && _grayBits != nullptr)
	{
		::BitBlt(_hdcGray, 0, 0, _rect.right, _rect.bottom, _hdcBack, 0, 0, SRCCOPY);

		const int width = _rect.right;
		const int height = _rect.bottom;
		const int pixelCount = width * height;
		BYTE* p = _grayBits;

		for (int i = 0; i < pixelCount; ++i)
		{
			const int idx = i * 4;
			const BYTE b = p[idx + 0];
			const BYTE g = p[idx + 1];
			const BYTE r = p[idx + 2];
			const BYTE gray = (BYTE)((r * 299 + g * 587 + b * 114) / 1000);

			p[idx + 0] = gray;
			p[idx + 1] = gray;
			p[idx + 2] = gray;
		}

		::BitBlt(_hdc, 0, 0, _rect.right, _rect.bottom, _hdcGray, 0, 0, SRCCOPY);
	}
	else
	{
		// 여분 도화지에 렌더링 끝.
		// 한번 그림을 다 그렸으니, 이제는 프론트 버퍼에 복사.
		::BitBlt(_hdc, 0, 0, _rect.right, _rect.bottom, _hdcBack, 0, 0, SRCCOPY); // 비트 블릿 : 고속 복사
	}

	// 프론트 버퍼에 복사가 끝났으면, 백버퍼는 초기화
	::PatBlt(_hdcBack, 0, 0, _rect.right, _rect.bottom, _background);
}

void Game::RewindGameScene(string mapName)
{
	GamePlayManager::GetInstance()->StartRewind();
}

void Game::ReplayGameScene()
{
	InputManager::GetInstance()->SetIsReplay(true);
	ChangeGameScene(_currentStageName);
}

void Game::ChangeGameScene(string mapName)
{
	if (_stageInfo[mapName].is_null()) return;

	if (_nextScene)
	{
		delete _nextScene;
		_nextScene = nullptr;
	}

	CollisionManager::GetInstance()->ClearColliderList();
	if (!InputManager::GetInstance()->GetIsReplay())
	{
		InputManager::GetInstance()->ClearInputLog();
	}

	_currentStageName = mapName;
	_nextStageName = _stageInfo[mapName]["NextStage"];

	_nextScene = new GameScene(mapName, _stageInfo[mapName]["FileName"], _stageInfo[mapName]["NextStage"], _stageInfo[mapName]["BGM"]);
	GameScene* gameScene = static_cast<GameScene*>(_nextScene);
	gameScene->OnRetryGame = [this](string mapName) { RewindGameScene(mapName); };
	gameScene->OnExitToMainMenu = [this](){ ChangeLobbyScene(); };
	_background = BLACKNESS;

	HCURSOR newCurosr = ResourceManager::GetInstance()->GetCursor();
	SetClassLongPtr(_hwnd, GCLP_HCURSOR, (LONG_PTR)newCurosr);
}

void Game::ChangeLobbyScene()
{
	if (_nextScene)
	{
		delete _nextScene;
		_nextScene = nullptr;
	}

	_nextScene = new LobbyScene();
	LobbyScene* lobbyScene = static_cast<LobbyScene*>(_nextScene);
	lobbyScene->OnStartGame = [this](string mapName) { ChangeGameScene(mapName); };
	lobbyScene->OnOpenEditor = [this]() { ChangeEditorScene(); };
	lobbyScene->OnExitGame = [this]() { ExitGame(); };
	_background = BLACKNESS;
	GamePlayManager::GetInstance()->SetIsPaused(false);
	GamePlayManager::GetInstance()->SetIsCaptureStop(true);
}

void Game::ChangeEditorScene()
{
	if (_nextScene)
	{
		delete _nextScene;
		_nextScene = nullptr;
	}

	_nextScene = new EditorScene();
	_background = WHITENESS;
	SoundManager::GetInstance()->StopBGM();
	_subWindow = new EditorSub();
	_subWindow->Init();
	ShowWindow(_hwndSub, 10);
	UpdateWindow(_hwndSub);
}

void Game::ExitGame()
{
	DestroyWindow(_hwndSub);
	DestroyWindow(_hwnd);
}

Vector2 Game::GetCurrentSceneSize()
{
	if (_curScene)
	{
		return _curScene->GetSceneSize();
	}

	return Vector2();
}

Camera* Game::GetCurrentSceneCamera()
{
	if (_curScene)
	{
		return _curScene->GetSceneCamera();
	}

	return nullptr;
}

void Game::ShakeSceneCamera()
{
	if (_curScene)
	{
		_curScene->GetSceneCamera()->SetCameraShake(true);
	}
}

Vector2 Game::ConvertCurSceneScreenPos(Vector2 worldPos)
{
	if (_curScene == nullptr) return Vector2();

	Camera* camera = _curScene->GetSceneCamera();
	if (camera == nullptr) return Vector2();

	return camera->ConvertScreenPos(worldPos);
}

void Game::StageClear()
{
	if (InputManager::GetInstance()->GetIsReplay())
	{
		InputManager::GetInstance()->SetIsReplay(false);
		GamePlayManager::GetInstance()->ClearBuffer();
		ChangeGameScene(_nextStageName);
	}
	else
	{
		// 보스 스테이지 클리어 시
		if (_currentStageName == "BossRoom")
		{
			bSkipReplayAfterClear = true;
		}

		GamePlayManager::GetInstance()->SetIsCaptureStop(true);
		fStageClearWait = 0.f;
		bStageClearFade = true;
		bIsStageClear = true;
		_stageClearImage->SetAlpha(0);
		_stageClearImage->SetOpen(true);
	}
}
