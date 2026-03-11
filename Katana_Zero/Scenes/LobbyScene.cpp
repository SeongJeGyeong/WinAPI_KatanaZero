#include "pch.h"
#include "LobbyScene.h"
#include "../UI/UIButton.h"
#include "../Managers/ResourceManager.h"
#include "../Managers/SoundManager.h"

void LobbyScene::Init()
{
	Super::Init();
	_sceneType = ESceneType::LOBBY;

	_UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 - 350), "spr_title_background_0", 1280, 1440);
	_UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 - 350), "spr_title_fence_0", 1280, 1440);
	_UI.CreateImage(Vector2(GWinSizeX / 2 - 55.f, GWinSizeY / 2 - 50.f), "spr_titlegraphic_big_1", 0, 0, 1.5f);
	_UI.CreateImage(Vector2(GWinSizeX / 2 + 160.f, GWinSizeY / 2 - 51.f), "spr_titlegraphic_big_2", 0, 0, 1.5f);
	_UI.CreateImage(Vector2(GWinSizeX / 2 + 10.f, GWinSizeY / 2 - 115.f), "spr_titlegraphic_big2_0", 0, 0, 1.5f);
	_UI.CreateAnimImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 105.f), "spr_title_plants");
	_UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 300.f), "spr_title_grass_0", 0, 0, 2.f);
	_UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 150), "lobby_mask", 530, 200);

	UIButton* GameStartButton = _UI.CreateButton(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 100), "lobby_select_mask", L"게임 시작", 0, 0);
	UIButton* mapEditorButton = _UI.CreateButton(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 150), "lobby_select_mask", L"에디터 열기", 0, 0);
	UIButton* ExitButton = _UI.CreateButton(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 200), "lobby_select_mask", L"게임 종료", 0, 0);

	// 함수 포인터 연결
	GameStartButton->SetClickEvent([this]() { OnStartGame("Stage1"); });
	mapEditorButton->SetClickEvent([this]() { OnOpenEditor(); });
	ExitButton->SetClickEvent([this]() { OnExitGame(); });

	SoundManager::GetInstance()->PlayBGM(EBGMType::MAIN_MENU, true);
}

void LobbyScene::Destroy()
{
	Super::Destroy();
}

void LobbyScene::Update(float deltaTime)
{
	Super::Update(deltaTime);
}

void LobbyScene::Render(HDC hdc)
{
	Super::Render(hdc);
	SetBkColor(hdc, RGB(0, 0, 0));
	HFONT font = ResourceManager::GetInstance()->GetFont(0);
	SelectObject(hdc, font);
}