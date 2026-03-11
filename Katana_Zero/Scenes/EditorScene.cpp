#include "pch.h"
#include "EditorScene.h"
#include "../Managers/InputManager.h"
#include "../Game/Game.h"
#include "EditorSub.h"
#include "../Resources/Texture.h"
#include "../Managers/ResourceManager.h"
#include "../Objects/Camera.h"

void EditorScene::Init()
{
	Super::Init();
	_sceneType = ESceneType::EDITOR;

	iSceneSizeX = 3840;
	iSceneSizeY = 2160;
	iWorldGridMaxX = iSceneSizeX / TILE_SIZE;
	iWorldGridMaxY = iSceneSizeY / TILE_SIZE;
	iHalfTileSize = TILE_SIZE / 2;

	_tileMapList = ResourceManager::GetInstance()->GetTileMapList();

	_subWindow = Game::GetInstance()->GetSubWindow();

	_sceneCamera = new Camera();
	_sceneCamera->Init();
	_sceneCamera->SetWorldSize(iSceneSizeX, iSceneSizeY);
	Vector2 size = _sceneCamera->GetCameraSize();
	_sceneCamera->SetPos({ size.x / 2, size.y / 2 });
}

void EditorScene::Destroy()
{
	Super::Destroy();
	ShowWindow(Game::GetInstance()->GetHwndSub(), SW_HIDE);
}

void EditorScene::Update(float deltaTime)
{
	if (_sceneCamera)
	{
		Vector2 cameraPos = _sceneCamera->GetPos();
		if (InputManager::GetInstance()->GetButtonPressed(KeyType::W))
		{
			cameraPos.y -= 200.f * deltaTime;
		}
		if (InputManager::GetInstance()->GetButtonPressed(KeyType::S))
		{
			cameraPos.y += 200.f * deltaTime;
		}
		if (InputManager::GetInstance()->GetButtonPressed(KeyType::A))
		{
			cameraPos.x -= 200.f * deltaTime;
		}
		if (InputManager::GetInstance()->GetButtonPressed(KeyType::D))
		{
			cameraPos.x += 200.f * deltaTime;
		}
		_sceneCamera->SetTargetPos(cameraPos);
	}

	if (InputManager::GetInstance()->GetButtonDown(KeyType::KEY_1))
	{
		eMode = EditorMode::TILEMODE;
	}
	else if (InputManager::GetInstance()->GetButtonDown(KeyType::KEY_2))
	{
		eMode = EditorMode::COLLIDERMODE;
		eColMode = EColliderMode::COL_BOX;
	}

	if (InputManager::GetInstance()->GetButtonUp(KeyType::ESC))
	{
		Game::GetInstance()->ChangeLobbyScene();
	}

	switch (eMode)
	{
	case TILEMODE:
		TileModeUpdate();
		break;
	case COLLIDERMODE:
		ColliderModeUpdate();
		break;
	default:
		break;
	}

	if (InputManager::GetInstance()->GetButtonUp(KeyType::F1))
	{
		SaveMap();
	}
	if (InputManager::GetInstance()->GetButtonUp(KeyType::F2))
	{
		LoadMap();
	}

	Super::Update(deltaTime);
}

void EditorScene::Render(HDC hdc)
{
	Super::Render(hdc);

	DrawGird(hdc);
	DrawTile(hdc);
	DrawCollider(hdc);

	SetTextAlign(hdc, TA_LEFT);
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkColor(hdc, RGB(255, 255, 255));

	POINT mousePos = InputManager::GetInstance()->GetMousePos();
	POINT WorldMouse = _sceneCamera->ConvertWorldPos(mousePos);
	wstring mstr = std::format(L"World Mouse Pos ({0}, {1})", WorldMouse.x, WorldMouse.y);
	::TextOut(hdc, 300, 30, mstr.c_str(), static_cast<int32>(mstr.size()));

	wstring str;
	if (eMode == EditorMode::TILEMODE)
	{
		str = L"MODE : Tile Mode";
	}
	else
	{
		str = L"MODE : Collider Mode";
		wstring colStr;
		switch (eColMode)
		{
		case COL_BOX:
			colStr = L"COLLIDER : Box";
			break;
		case COL_PLATFORM:
			colStr = L"COLLIDER : PlatForm";
			break;
		case COL_WALL_V:
			colStr = L"COLLIDER : Vertical Wall";
			break;
		case COL_WALL_H:
			colStr = L"COLLIDER : Horizontal Wall";
			break;
		case COL_STAIR:
			colStr = L"COLLIDER : Stair";
			break;
		case COL_PORTAL:
			colStr = L"COLLIDER : Portal";
			break;
		default:
			break;
		}
		::TextOut(hdc, 20, 70, colStr.c_str(), static_cast<int32>(colStr.size()));
	}
	::TextOut(hdc, 20, 50, str.c_str(), static_cast<int32>(str.size()));

	SetTextColor(hdc, RGB(255, 255, 255));
}

void EditorScene::DrawGird(HDC hdc)
{
	Vector2 cameraPos = _sceneCamera->GetPos();
	Vector2 cameraSize = _sceneCamera->GetCameraSize();
	float cameraPosXMin = cameraPos.x - (cameraSize.x / 2);
	float cameraPosXMax = cameraPos.x + (cameraSize.x / 2);
	float cameraPosYMin = cameraPos.y - (cameraSize.y / 2);
	float cameraPosYMax = cameraPos.y + (cameraSize.y / 2);

	int32 xMn = cameraPosXMin / TILE_SIZE;
	int32 xMx = cameraPosXMax / TILE_SIZE;
	int32 yMn = cameraPosYMin / TILE_SIZE;
	int32 yMx = cameraPosYMax / TILE_SIZE;

	for (int32 i = xMn + 1; i <= xMx; ++i)
	{
		float screenX = i * TILE_SIZE - cameraPosXMin;
		::MoveToEx(hdc, screenX, 0, nullptr);
		::LineTo(hdc, screenX, iSceneSizeY);
	}
	for (int32 i = yMn; i <= yMx; ++i)
	{
		float screenY = i * TILE_SIZE - cameraPosYMin;
		::MoveToEx(hdc, 0, screenY, nullptr);
		::LineTo(hdc, iSceneSizeX, screenY);
	}
}

void EditorScene::DrawTile(HDC hdc)
{
	for (int i = 0; i < ERenderLayer::LAYER_END; ++i)
	{
		for (auto it : _tileInfoMap[i])
		{
			if (it.second.vTilePos == Vector2(-1, -1)) continue;

			// 그리드의 인덱스를 그리드 좌표로 변환
			Vector2 gridPos = ConvertIndexToWorldGrid(it.first);
			// 그리드 좌표를 실제 화면 좌표로 변환
			Vector2 worldPos = { gridPos.x * TILE_SIZE + iHalfTileSize, gridPos.y * TILE_SIZE + iHalfTileSize };
			// 실제 화면 좌표를 카메라에 출력될 좌표로 변환
			Vector2 screenPos = _sceneCamera->ConvertScreenPos(worldPos);
			_tileMapList[it.second.iTilesetIndex]->RenderTexture(hdc, screenPos.x - iHalfTileSize, screenPos.y - iHalfTileSize, TILE_SIZE, TILE_SIZE, it.second.vTilePos.x * TILE_SIZE, it.second.vTilePos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
		}
	}
}

void EditorScene::DrawCollider(HDC hdc)
{
	HPEN redPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	HPEN greenPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
	HPEN bluePen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
	HPEN magPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 255));
	HPEN cyPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 255));
	HPEN hOldPen = (HPEN)SelectObject(hdc, redPen);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));

	for (auto info : _colliderList)
	{
		switch (info.mode)
		{
		case COL_BOX:
			(HPEN)SelectObject(hdc, redPen);
			break;
		case COL_PLATFORM:
			(HPEN)SelectObject(hdc, magPen);
			break;
		case COL_WALL_V:
			(HPEN)SelectObject(hdc, cyPen);
			break;
		case COL_WALL_H:
			(HPEN)SelectObject(hdc, cyPen);
			break;
		case COL_STAIR:
			(HPEN)SelectObject(hdc, bluePen);
			break;
		case COL_PORTAL:
			(HPEN)SelectObject(hdc, greenPen);
			break;
		default:
			break;
		}

		if (info.mode == EColliderMode::COL_BOX || info.mode == EColliderMode::COL_PORTAL)
		{
			POINT startPoint = _sceneCamera->ConvertScreenPos(info.vStart);
			POINT endPoint = _sceneCamera->ConvertScreenPos(info.vEnd);
			Rectangle(hdc, startPoint.x, startPoint.y, endPoint.x, endPoint.y);
		}
		else
		{
			POINT startPoint = _sceneCamera->ConvertScreenPos(info.vStart);
			POINT endPoint = _sceneCamera->ConvertScreenPos(info.vEnd);
			::MoveToEx(hdc, startPoint.x, startPoint.y, nullptr);
			::LineTo(hdc, endPoint.x, endPoint.y);
		}

	}

	if (_startPoint.x != INT_MAX && _startPoint.y != INT_MAX)
	{
		POINT startPoint = _sceneCamera->ConvertScreenPos(_startPoint);
		if (eColMode == EColliderMode::COL_BOX || eColMode == EColliderMode::COL_PORTAL)
		{
			RednerBox(hdc, startPoint, eColMode);
		}
		else
		{
			RednerLine(hdc, startPoint, eColMode);
		}
	}

	SelectObject(hdc, hOldPen);
	DeleteObject(redPen);
	DeleteObject(bluePen);
	DeleteObject(greenPen);
	DeleteObject(magPen);
	DeleteObject(cyPen);
}

void EditorScene::SaveMap()
{
	HWND hwnd = Game::GetInstance()->GetHwnd();

	OPENFILENAME ofn;
	wchar_t szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = L"JSON (*.json)\0*.json\0모든 파일 (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = L"tilemap";

	//// 파일 이름이 선택되었으면 저장
	if (GetSaveFileName(&ofn)) 
	{
		json jsonfile;
		std::ofstream file(szFileName);

		if (file.is_open()) 
		{
			json tileData;
			json actorData;
			Vector2 minPos = Vector2(iSceneSizeX, iSceneSizeY);
			Vector2 maxPos = Vector2(0, 0);
			// 타일 데이터 저장
			for (int32 i = 0; i < ERenderLayer::LAYER_END; i++)
			{
				if (_tileInfoMap[i].empty()) continue;

				json TilesByLayer;
				for (auto it : _tileInfoMap[i])
				{
					if (it.second.iTilesetIndex == -1) continue;
					Vector2 pos = ConvertIndexToWorldGrid(it.first);
					Vector2 screenPos = { pos.x * TILE_SIZE + TILE_SIZE / 2, pos.y * TILE_SIZE + TILE_SIZE / 2 };
					if (minPos.x > screenPos.x) minPos.x = screenPos.x;
					if (minPos.y > screenPos.y) minPos.y = screenPos.y;
					if (maxPos.x < screenPos.x) maxPos.x = screenPos.x;
					if (maxPos.y < screenPos.y) maxPos.y = screenPos.y;

					TilesByLayer[to_string(it.first)].push_back(it.second.iTilesetIndex);
					TilesByLayer[to_string(it.first)].push_back(pos.x);
					TilesByLayer[to_string(it.first)].push_back(pos.y);
					TilesByLayer[to_string(it.first)].push_back(it.second.vTilePos.x);
					TilesByLayer[to_string(it.first)].push_back(it.second.vTilePos.y);
				}

				string tileType;

				switch ((ERenderLayer)i)
				{
				case ERenderLayer::BACKGROUND:
					tileType = "Background";
					tileData[tileType] = TilesByLayer;
					break;
				case ERenderLayer::FOREGROUND:
					tileType = "Foreground";
					tileData[tileType] = TilesByLayer;
					break;
				case ERenderLayer::ACTOR:
					tileType = "Actor";
					actorData[tileType] = TilesByLayer;
					break;
				default:
					break;
				}
			}
			jsonfile["TileInfo"] = tileData;
			jsonfile["ActorInfo"] = actorData;

			json colliderData;
			for (int32 i = 0; i < _colliderList.size(); i++)
			{
				json ColliderByType;
				string colliderType;

				switch (_colliderList[i].mode)
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
				default:
					break;
				}
				ColliderByType["Start"] = {_colliderList[i].vStart.x, _colliderList[i].vStart.y};
				ColliderByType["End"] = { _colliderList[i].vEnd.x , _colliderList[i].vEnd.y };
				colliderData[colliderType].push_back(ColliderByType);
			}
			jsonfile["ColliderInfo"] = colliderData;
			jsonfile["MapSize"] = { maxPos.x + (TILE_SIZE * 0.5f), maxPos.y + (TILE_SIZE * 0.5f)};
			jsonfile["MinPosition"] = { minPos.x, minPos.y };
			jsonfile["MaxPosition"] = { maxPos.x, maxPos.y };

			file << std::setw(4) << jsonfile;
			file.close();
			MessageBox(hwnd, L"타일맵이 저장되었습니다.", L"저장 완료", MB_OK | MB_ICONINFORMATION);
		}
		else 
		{
			MessageBox(hwnd, L"파일을 저장할 수 없습니다.", L"오류", MB_OK | MB_ICONERROR);
		}
	}
}

void EditorScene::LoadMap()
{
	HWND hwnd = Game::GetInstance()->GetHwnd();

	OPENFILENAME ofn;
	wchar_t szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = L"JSON (*.json)\0*.json\0모든 파일 (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = L"tilemap";

	// 파일 이름이 선택되었으면 로드
	if (GetOpenFileName(&ofn)) {
		std::wstring wFileName = szFileName;
		std::wstring fileName(wFileName.begin(), wFileName.end());

		std::ifstream file(fileName);
		if (file.is_open()) 
		{
			json mapData = json::parse(file);

			json tileData = mapData["TileInfo"];

			for (int32 type = 0; type < ERenderLayer::LAYER_END; ++type)
			{
				_tileInfoMap[type].clear();
				string tileType;

				switch ((ERenderLayer)type)
				{
				case ERenderLayer::BACKGROUND:
					tileType = "Background";
					break;
				case ERenderLayer::FOREGROUND:
					tileType = "Foreground";
					break;
				case ERenderLayer::ACTOR:
					tileType = "Actor";
					break;
				default:
					break;
				}

				for (auto data : tileData[tileType])
				{
					int32 index = ConvertWorldGridToIndex({ data[1], data[2] });
					_tileInfoMap[type][index] = { data[0], {data[3], data[4]} };
				}
			}

			json colliderData = mapData["ColliderInfo"];
			_colliderList.clear();
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
					ColliderInfo info;
					info.mode = (EColliderMode)type;
					info.vStart.x = data["Start"][0];
					info.vStart.y = data["Start"][1];
					info.vEnd.x = data["End"][0];
					info.vEnd.y = data["End"][1];

					_colliderList.push_back(info);
				}
			}

			json actorData = mapData["ActorInfo"];

			_tileInfoMap[ERenderLayer::ACTOR].clear();

			for (auto data : actorData["Actor"])
			{
				int32 index = ConvertWorldGridToIndex({ data[1], data[2] });
				_tileInfoMap[ERenderLayer::ACTOR][index] = { data[0], {data[3], data[4]} };
			}

			file.close();

			MessageBox(hwnd, L"타일맵이 로드되었습니다.", L"로드 완료", MB_OK | MB_ICONINFORMATION);
		}
		else {
			MessageBox(hwnd, L"파일을 로드할 수 없습니다.", L"오류", MB_OK | MB_ICONERROR);
		}
	}
}

void EditorScene::TileModeUpdate()
{
	if (InputManager::GetInstance()->GetButtonPressed(KeyType::LeftMouse))
	{
		if (eMode == EditorMode::TILEMODE)
		{
			Vector2 selectedTilePos = _subWindow->GetSelectedTexturePos();
			if (selectedTilePos.x == -1 || selectedTilePos.y == -1) return;

			POINT mousePos = InputManager::GetInstance()->GetMousePos();
			POINT wolrdPos = _sceneCamera->ConvertWorldPos(mousePos);
			// 클릭한 그리드 위치 계산
			int32 x = wolrdPos.x / TILE_SIZE;
			int32 y = wolrdPos.y / TILE_SIZE;
			int32 index = ConvertWorldGridToIndex({ (float)x, (float)y });

			_tileInfoMap[_subWindow->GetSelectedLayer()][index] = { _subWindow->GetSelectedTileSet(), selectedTilePos };
		}
	}
	else if (InputManager::GetInstance()->GetButtonPressed(KeyType::RightMouse))
	{
		POINT mousePos = InputManager::GetInstance()->GetMousePos();
		POINT wolrdPos = _sceneCamera->ConvertWorldPos(mousePos);
		// 클릭한 그리드 위치 계산
		int32 x = wolrdPos.x / TILE_SIZE;
		int32 y = wolrdPos.y / TILE_SIZE;
		int32 index = ConvertWorldGridToIndex({ (float)x, (float)y });
		_tileInfoMap[_subWindow->GetSelectedLayer()][index] = { -1, {-1, -1} };
	}
}

void EditorScene::ColliderModeUpdate()
{
	if (InputManager::GetInstance()->GetButtonDown(KeyType::Q))
	{
		if (eColMode != 0) eColMode = (EColliderMode)(eColMode - 1);
	}
	else if (InputManager::GetInstance()->GetButtonDown(KeyType::E))
	{
		if (eColMode != EColliderMode::COL_END - 1) eColMode = (EColliderMode)(eColMode + 1);
	}

	if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
	{
		POINT mousePos = InputManager::GetInstance()->GetMousePos();
		POINT wolrdPos = _sceneCamera->ConvertWorldPos(mousePos);
		// 클릭한 그리드 위치 계산
		int32 x = wolrdPos.x / TILE_SIZE;
		int32 y = wolrdPos.y / TILE_SIZE;

		_startPoint = { x * TILE_SIZE, y * TILE_SIZE };
	}
	else if (InputManager::GetInstance()->GetButtonUp(KeyType::LeftMouse))
	{
		POINT mousePos = InputManager::GetInstance()->GetMousePos();
		POINT wolrdPos = _sceneCamera->ConvertWorldPos(mousePos);
		// 클릭한 그리드 위치 계산
		int32 x = wolrdPos.x / TILE_SIZE;
		int32 y = wolrdPos.y / TILE_SIZE;

		POINT endPoint = _startPoint;
		switch (eColMode)
		{
		case COL_BOX:
		case COL_PORTAL:
			endPoint = { x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE + TILE_SIZE };
			break;
		case COL_PLATFORM:
			endPoint = { x * TILE_SIZE + TILE_SIZE, _startPoint.y};
			break;
		case COL_WALL_V:
			endPoint = { _startPoint.x, y * TILE_SIZE + TILE_SIZE };
			break;
		case COL_WALL_H:
			endPoint = { x * TILE_SIZE + TILE_SIZE, _startPoint.y };
			break;
		case COL_STAIR:
			endPoint = { x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE + TILE_SIZE };
			break;
		default:
			break;
		}

		// 시작지점과 끝지점이 같으면 콜라이더를 만들지 않는다.
		if (_startPoint.x == endPoint.x && _startPoint.y == endPoint.y)
		{
			_startPoint = { INT_MAX, INT_MAX };
			return;
		}

		ColliderInfo info;
		info.mode = eColMode;
		info.vStart = _startPoint;
		info.vEnd = endPoint;
		_colliderList.push_back(info);
		_startPoint = { INT_MAX , INT_MAX };
	}
	else if (InputManager::GetInstance()->GetButtonUp(KeyType::RightMouse))
	{
		if(!_colliderList.empty()) _colliderList.pop_back();
	}
}

void EditorScene::RednerBox(HDC hdc, POINT startPoint, EColliderMode mode)
{
	unsigned long color = (mode == EColliderMode::COL_BOX) ? RGB(255, 0, 0) : RGB(0, 255, 0);
	HPEN redPen = CreatePen(PS_SOLID, 3, color);
	HPEN hOldPen = (HPEN)SelectObject(hdc, redPen);

	POINT mousePos = InputManager::GetInstance()->GetMousePos();
	int32 x = mousePos.x / TILE_SIZE;
	int32 y = mousePos.y / TILE_SIZE;
	POINT endPoint = { x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE + TILE_SIZE };
	int32 marginX = (endPoint.x - startPoint.x) % TILE_SIZE;
	int32 marginY = (endPoint.y - startPoint.y) % TILE_SIZE;

	Rectangle(hdc, startPoint.x, startPoint.y, endPoint.x - marginX, endPoint.y - marginY);
	SelectObject(hdc, hOldPen);
	DeleteObject(redPen);
}

void EditorScene::RednerLine(HDC hdc, POINT startPoint, EColliderMode mode)
{
	POINT mousePos = InputManager::GetInstance()->GetMousePos();
	int32 x = mousePos.x / TILE_SIZE;
	int32 y = mousePos.y / TILE_SIZE;
	POINT endPoint = startPoint;

	HPEN mPen = nullptr;
	HPEN oldPen = nullptr;
	switch (mode)
	{
	case COL_PLATFORM:
		mPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 255));
		oldPen = (HPEN)SelectObject(hdc, mPen);
		endPoint = { x * TILE_SIZE + TILE_SIZE, startPoint.y};
		break;
	case COL_WALL_V:
		mPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 255));
		oldPen = (HPEN)SelectObject(hdc, mPen);
		endPoint = { startPoint.x, y * TILE_SIZE + TILE_SIZE };
		break;
	case COL_WALL_H:
		mPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 255));
		oldPen = (HPEN)SelectObject(hdc, mPen);
		endPoint = { x * TILE_SIZE + TILE_SIZE, startPoint.y };
		break;
	case COL_STAIR:
		mPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
		oldPen = (HPEN)SelectObject(hdc, mPen);
		endPoint = { x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE + TILE_SIZE };
		break;
	default:
		break;
	}
	::MoveToEx(hdc, startPoint.x, startPoint.y, nullptr);
	::LineTo(hdc, endPoint.x, endPoint.y);

	SelectObject(hdc, oldPen);
	DeleteObject(mPen);
}
