#include "pch.h"
#include "GameScene.h"
#include "../Objects/Actors/Player.h"
#include "../Components/TileRenderer.h"
#include "../Managers/ResourceManager.h"
#include "../Objects/Camera.h"
#include "../Objects/Actors/Enemy.h"
#include "../Objects/Actors/Bullet.h"
#include "../Objects/Actors/Boss.h"
#include "../Managers/CollisionManager.h"
#include "../Objects/Actors/Axe.h"
#include "../UI/UIImage.h"
#include "../UI/UIButton.h"
#include "../UI/UIBundle.h"
#include "../UI/UIProgressBar.h"
#include "../Managers/InputManager.h"
#include "../Game/Game.h"
#include "../Managers/TimeManager.h"
#include "../Resources/Texture.h"
#include "../Managers/SoundManager.h"
#include "../Managers/GamePlayManager.h"
#include "Components/Colliders/CollisionShape.h"

GameScene::GameScene(string mapName, string mapFileName, string nextStage, string bgmName)
{
	sMapName = mapName;
	_sceneCamera = new Camera();
	_sceneCamera->Init();

	fs::path directory = fs::current_path() / L"../GameResources/Json/";
	fs::path path = directory / mapFileName;
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		MessageBox(nullptr, L"Failed to open JSON file", L"Error", MB_OK);
		return;
	}

	json data = json::parse(file);

	if (data["TileInfo"].is_null())
	{
		iSceneSizeX = 1280;
		iSceneSizeY = 720;
	}
	else
	{
		iSceneSizeX = data["MapSize"][0];
		iSceneSizeY = data["MapSize"][1];
	}
	_sceneCamera->SetWorldSize(iSceneSizeX, iSceneSizeY);

	_tileRenderer = new TileRenderer();
	_tileRenderer->InitComponent(iSceneSizeX, iSceneSizeY);
	for (int32 i = 0; i < ERenderLayer::LAYER_END; ++i)
	{
		_tileRenderer->AddTileTexture(ResourceManager::GetInstance()->GetTileMapForIndex(i));
	}

	if (data["TileInfo"].is_null())
	{
		_tileRenderer->AddWholeMap(2);
	}
	else
	{
		LoadTiles(data["TileInfo"]);
	}

	LoadColliders(data["ColliderInfo"]);
	LoadActors(data["ActorInfo"]);
	file.close();

	fCurrentElapsedTime = 0.f;
	sNextStage = nextStage;

	if (bgmName == "song_katanazero")
	{
		SoundManager::GetInstance()->PlayBGM(EBGMType::STAGE, true);
	}
	else if (bgmName == "song_bossbattle")
	{
		SoundManager::GetInstance()->PlayBGM(EBGMType::BOSS_ROOM, true);
	}

	UIInit();
	TimeManager::GetInstance()->InitSlowMotionBattery();

	if(!InputManager::GetInstance()->GetIsReplay())
	{
		GamePlayManager::GetInstance()->SetIsCaptureStop(false);
	}
}

void GameScene::LoadTiles(json tileData)
{
	for (auto data : tileData["Background"])
	{
		Vector2 screenPos = { data[1].get<float>() * TILE_SIZE + TILE_SIZE * 0.5f, data[2].get<float>() * TILE_SIZE + TILE_SIZE * 0.5f};

		_tileRenderer->AddTileInfo(data[0].get<int>(), {screenPos.x, screenPos.y}, {data[3], data[4]});
	}

	for (auto data : tileData["Foreground"])
	{
		Vector2 screenPos = { data[1].get<float>() * TILE_SIZE + TILE_SIZE * 0.5f, data[2].get<float>() * TILE_SIZE + TILE_SIZE * 0.5f };

		_tileRenderer->AddTileInfo(data[0].get<int>(), {screenPos.x, screenPos.y}, {data[3], data[4]});
	}
}

void GameScene::LoadColliders(json colliderData)
{
	for (int32 type = 0; type < EColliderMode::COL_END; ++type)
	{
		string colliderType;
		switch ((EColliderMode)type)
		{
		case COL_BOX:
			colliderType = "Box";
			break;
		case COL_PLATFORM:
			colliderType = "Platform";
			break;
		case COL_WALL_V:
			colliderType = "Wall_V";
			break;
		case COL_WALL_H:
			colliderType = "Wall_H";
			break;
		case COL_STAIR:
			colliderType = "Stair";
			break;
		case COL_PORTAL:
			colliderType = "Portal";
			break;
		default:
			break;
		}

		for (auto data : colliderData[colliderType])
		{
			Vector2 startPoint = { data["Start"][0], data["Start"][1] };
			Vector2 endPoint = { data["End"][0], data["End"][1] };

			Actor* collider = new Actor();
			collider->Init({ (endPoint.x + startPoint.x) * 0.5f, (endPoint.y + startPoint.y) * 0.5f });
			if (type == EColliderMode::COL_BOX)
			{
				collider->CreateCollider<AABBCollisionShape>(ECollisionLayer::GROUND, endPoint.x - startPoint.x, endPoint.y - startPoint.y);
			}
			else if (type == EColliderMode::COL_PORTAL)
			{
				collider->CreateCollider<AABBCollisionShape>(ECollisionLayer::PORTAL, endPoint.x - startPoint.x, endPoint.y - startPoint.y);
				_portal = collider;
				_portal->SetIsActive(false);
				continue;
			}
			else
			{
				switch ((EColliderMode)type)
				{
				case COL_PLATFORM:
					collider->CreateCollider<LineCollisionShape>(ECollisionLayer::PLATFORM, startPoint, endPoint);
					break;
				case COL_WALL_V:
					collider->CreateCollider<LineCollisionShape>(ECollisionLayer::WALL, startPoint, endPoint);
					break;
				case COL_WALL_H:
					collider->CreateCollider<LineCollisionShape>(ECollisionLayer::CEILING, startPoint, endPoint);
					break;
				case COL_STAIR:
					collider->CreateCollider<LineCollisionShape>(ECollisionLayer::STAIR, startPoint, endPoint);
					break;
				}
			}
			_colliderList.push_back(collider);
		}
	}
}

void GameScene::LoadActors(json actorData)
{
	for (auto data : actorData["Actor"])
	{
		Vector2 screenPos = { (float)data[1] * TILE_SIZE + TILE_SIZE / 2, (float)data[2] * TILE_SIZE + TILE_SIZE / 2 };

		if (data[3].get<int>() == 2)
		{
			Enemy* enemy = new Enemy();
			enemy->Init({ screenPos.x, screenPos.y - 20.f });
			enemy->SetCamera(_sceneCamera);
			enemy->OnCreateBullet = bind(&GameScene::CreateBullet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			_EnemyList.push_back(enemy);
			enemy->OnDie = bind(&GameScene::EnemyDie, this);
			iRemainsEnemyCount++;
		}
		else if (data[3].get<int>() == 0)
		{
			_player = new Player();
			_player->Init({ screenPos.x, screenPos.y - 15.f });
			_sceneCamera->SetInitialPos({ screenPos.x, screenPos.y - 15.f });
			_player->SetPlayerCamera(_sceneCamera);
			_player->OnDyingMessage = [this](wstring message) { sGameOverText = message; _gameOverBox->SetOpen(true); };
			_player->OnClearStage = [this]() 
				{
					Game::GetInstance()->StageClear();
				};
		}
		else
		{
			_boss = new Boss();
			_boss->Init({ screenPos.x, screenPos.y - 40.f });
			_boss->OnSpawnAxe = bind(&GameScene::SpawnAxe, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			_boss->OnDie = bind(&GameScene::EnemyDie, this);
			iRemainsEnemyCount++;
		}
	}
}

void GameScene::Init()
{
	Super::Init();
	for (const auto& enemy : _EnemyList)
	{
		enemy->SetPlayer(_player);
	}
	if (_boss)
	{
		_boss->SetPlayer(_player);
	}
}

void GameScene::Destroy()
{
	Super::Destroy();

	for (auto& enemy : _EnemyList)
	{
		SAFE_DELETE(enemy);
	}
	_EnemyList.clear();
	for (auto& bullet : _BulletList)
	{
		SAFE_DELETE(bullet);
	}
	_BulletList.clear();
	for (auto& collider : _colliderList)
	{
		SAFE_DELETE(collider);
	}
	_colliderList.clear();

	SAFE_DELETE(_boss);
	SAFE_DELETE(_axe);
	SAFE_DELETE(_player);
	SAFE_DELETE(_sceneCamera);
}

void GameScene::Update(float deltaTime)
{
	if (GamePlayManager::GetInstance()->GetIsPaused())
	{
		_pauseUI.Update(deltaTime);
		return;
	}

	TimeManager* timeManager = TimeManager::GetInstance();
	fCurrentElapsedTime += timeManager->GetConstDeltaTime();
	float progress = (fTimeLimit - fCurrentElapsedTime) / fTimeLimit;
	_timerProgressBar->SetProgress(progress);
	_slowMotionBattery->SetBundleCount(timeManager->GetSlowMotionBatteryCount());

	if (_player)
	{
		if (progress <= 0.f && !_player->GetIsDead()) _player->Die(true);
		_player->Update(deltaTime);
	}

	if (_boss)
	{
		_boss->Update(deltaTime);
	}

	for (Enemy* enemy : _EnemyList)
	{
		enemy->Update(deltaTime);
	}

	for (Bullet* bullet : _BulletList)
	{
		bullet->Update(deltaTime);
	}

	if (_axe)
	{
		_axe->Update(deltaTime);
	}

	Super::Update(deltaTime);
}

void GameScene::PostUpdate(float deltaTime)
{
	if (InputManager::GetInstance()->GetButtonDown(KeyType::ESC))
	{
		GamePlayManager::GetInstance()->TogglePause();
	}

	if (GamePlayManager::GetInstance()->GetIsPaused()) return;

	Super::PostUpdate(deltaTime);

	if (_player)
	{
		_player->PostUpdate(deltaTime);
	}

	if (_boss)
	{
		_boss->PostUpdate(deltaTime);
	}

	for (Enemy* enemy : _EnemyList)
	{
		enemy->PostUpdate(deltaTime);
	}

	for (auto it = _BulletList.begin(); it != _BulletList.end();)
	{
		if ((*it)->GetIsDead())
		{
			SAFE_DELETE((*it));
			it = _BulletList.erase(it);
		}
		else
		{
			(*it)->PostUpdate(deltaTime);
			++it;
		}
	}

	if (_axe)
	{
		_axe->PostUpdate(deltaTime);
	}

	if (_player != nullptr && _player->GetIsDead())
	{
		fGameOverWait += deltaTime;
		if (fGameOverWait > 0.1f && InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
		{
			fGameOverWait = 0.f;
			OnRetryGame(sMapName);
			_UI.SetUIVisibility(false);
		}
	}
}

void GameScene::Render(HDC hdc)
{
	if (_tileRenderer)
	{
		_tileRenderer->RenderComponent(hdc);
	}

	for (Actor* collider : _colliderList)
	{
		collider->Render(hdc);
	}
	if (_portal)
	{
		_portal->Render(hdc);
	}

	if(TimeManager::GetInstance()->GetSlowMotion() && _slowMotionMask)
	{
		_slowMotionMask->RenderTexture(hdc, Vector2(GWinSizeX / 2, GWinSizeY / 2), GWinSizeX, GWinSizeY, TimeManager::GetInstance()->GetMaskAlpha());
	}

	for (Enemy* enemy : _EnemyList)
	{
		enemy->Render(hdc);
	}

	if (_boss)
	{
		_boss->Render(hdc);
	}

	if (_player)
	{
		_player->Render(hdc);
	}

	if (_axe)
	{
		_axe->Render(hdc);
	}

	for (Bullet* bullet : _BulletList)
	{
		bullet->Render(hdc);
	}

	GamePlayManager::GetInstance()->CaptureFrame(hdc, GWinSizeX, GWinSizeY);

	if (iRemainsEnemyCount <= 0)
	{
		Vector2 portalPos = _sceneCamera->ConvertScreenPos(_portal->GetPos());
		portalPos.x -= 20.f;
		if (portalPos.x > GWinSizeX - 50.f)
		{
			portalPos.x = GWinSizeX - 50.f;
		}
		if (portalPos.y > GWinSizeY)
		{
			portalPos.y = GWinSizeY - 20.f;
		}
		else if (portalPos.y < 90.f)
		{
			portalPos.y = 90.f;
		}
		_arrowToPortal->RenderTexture(hdc, portalPos, 62, 38);
	}

	Super::Render(hdc);

	if (_player->GetIsDead())
	{
		SetTextColor(hdc, RGB(0, 255, 255));
		::DrawText(hdc, sGameOverText.c_str(), sGameOverText.size(), &_gameOverRect, DT_CENTER);
	}

	if (GamePlayManager::GetInstance()->GetIsPaused())
	{
		_pauseUI.Render(hdc);

		SetTextColor(hdc, RGB(255, 255, 0));

		HFONT font = ResourceManager::GetInstance()->GetFont(4);
		HFONT oldFont = (HFONT)SelectObject(hdc, font);

		wstring pausestr = std::format(L"KATANA ZERO 일시정지");
		::TextOut(hdc, 50, 680, pausestr.c_str(), static_cast<int32>(pausestr.size()));

		SelectObject(hdc, oldFont);
	}
}

void GameScene::UIInit()
{
	UIImage* HUD_Background = _UI.CreateImage(Vector2(GWinSizeX * 0.5f, 23), "spr_hud", 1280, 46);
	UIImage* HUD_TimerHUD = _UI.CreateImage(Vector2(GWinSizeX * 0.5f - 10, 20), "spr_hud_timer", 224, 38);
	_timerProgressBar = _UI.CreateProgressBar(Vector2(GWinSizeX * 0.5f + 6, 16), "spr_timer", 188, 22);
	UIImage* HUD_BatteryHUD = _UI.CreateImage(Vector2(79, 25), "spr_hud_battery", 154, 38);
	_slowMotionBattery = _UI.CreateBundle(Vector2(28, 24), "spr_battery_part", 8, 20, TimeManager::GetInstance()->GetSlowMotionBatteryCount());
	_slowMotionBattery->SetMargin(2.f);

	_gameOverBox = _UI.CreateImage(Vector2(GWinSizeX * 0.5f, GWinSizeY * 0.5f - 15.f), "lobby_mask", 220, 150);

	if (_portal)
	{
		_arrowToPortal = ResourceManager::GetInstance()->GetTexture("spr_go_arrow");
	}

	_pauseUI.Init();
	UIImage* pauseBackground = _pauseUI.CreateImage(Vector2(GWinSizeX * 0.5f, GWinSizeY * 0.5f), "spr_pausemenu_bg_0");
	UIImage* pauseBackground2 = _pauseUI.CreateImage(Vector2(GWinSizeX * 0.5f, GWinSizeY * 0.5f + 336), "spr_pausemenu_bg_2");
	UIButton* resumeBtn = _pauseUI.CreateButton(Vector2(GWinSizeX * 0.5f, 120), "lobby_select_mask", L"이어하기", 1280, 50, 4);
	UIButton* retryBtn = _pauseUI.CreateButton(Vector2(GWinSizeX * 0.5f, 180), "lobby_select_mask", L"재시작", 1280, 50, 4);
	UIButton* mainMenuBtn = _pauseUI.CreateButton(Vector2(GWinSizeX * 0.5f, 240), "lobby_select_mask", L"메인 메뉴", 1280, 50, 4);

	resumeBtn->SetButtonTextAlign(EButtonTextAlign::LEFT);
	resumeBtn->SetMargin(20.f);
	retryBtn->SetButtonTextAlign(EButtonTextAlign::LEFT);
	retryBtn->SetMargin(20.f);
	mainMenuBtn->SetButtonTextAlign(EButtonTextAlign::LEFT);
	mainMenuBtn->SetMargin(20.f);

	resumeBtn->SetClickEvent([this]() { GamePlayManager::GetInstance()->TogglePause(); });
	retryBtn->SetClickEvent([this]() { OnRetryGame(sMapName); });
	mainMenuBtn->SetClickEvent([this]() { OnExitToMainMenu(); });

	_gameOverBox->SetOpen(false);

	_slowMotionMask = ResourceManager::GetInstance()->GetTexture("background_mask");
	_slowMotionMask->SetAlpha(0);
}

void GameScene::CreateBullet(Vector2 pos, Vector2 dir, float length, float radian)
{
	Bullet* bullet = new Bullet();
	bullet->Init(pos, dir, length, radian);
	_BulletList.push_back(bullet);
}

void GameScene::SpawnAxe(Vector2 pos, Actor* owner, Vector2 dir, bool throwOrSwing)
{
	if (_axe == nullptr)
	{
		_axe = new Axe();
	}
	_axe->Init(pos, owner, dir, throwOrSwing);
	_axe->SetIsActive(true);
	_axe->OnReturnAxe = bind(&Boss::ReturnAxe, _boss);
}

void GameScene::EnemyDie()
{
	--iRemainsEnemyCount;
	if (iRemainsEnemyCount <= 0)
	{
		_portal->SetIsActive(true);
		SoundManager::GetInstance()->PlaySFX("sound_ui_prompt_go_01");
	}
}
