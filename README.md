# 소개
WinAPI를 활용해 Katana Zero를 모작한 프로젝트입니다.

+ 개발 환경 : WinAPI, nlohmann(Json Library), XAudio2(Audio API), SVN
+ 플랫폼 : PC
+ 개발 인원 : 1

# 주요 구현 기능

## 1. 클래스 구조
<details>
<summary><b>보기</b></summary>
  <details>
    <summary><b>Object</b></summary>
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Objects/Object.h#L1-L24
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Objects/Camera.h#L1-L40
  </details>
  <details>
    <summary><b>Actor</b></summary>
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Objects/Actors/Actor.h#L1-L68
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Objects/Actors/Player.h#L1-L125
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Objects/Actors/Enemy.h#L1-L75
  </details>
  <details>
    <summary><b>Component</b></summary>
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/Component.h#L1-L44
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/InputComponent.h#L1-L40
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/PlayerMovementComponent.h#L1-L93
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/EffectorComponent.h#L1-L70
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/Animator.h#L1-L51
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/CameraComponent.h#L1-L24
  </details>
  <details>
    <summary><b>Collider</b></summary>
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/Colliders/Collider.h#L1-L67
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/Colliders/AABBCollider.h#L1-L26
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/Colliders/LineCollider.h#L1-L18
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/Colliders/MovingLineCollider.h#L1-L17
  </details>
  <details>
    <summary><b>ComponentContainer</b></summary>
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Utils/ComponentContainer.h#L1-L38
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Utils/ComponentContainer.cpp#L1-L29
  </details>
</details>

## 2. 입력 & 액션 시스템
<details>
  <summary><b>보기</b></summary>
  <details>
    <summary><b>Input Manager, Component</b></summary>
      https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/InputManager.h#L1-L116
      https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/InputManager.cpp#L1-L126
      https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/InputComponent.cpp#L1-L334
  </details>
  <details>
    <summary><b>StateMachine</b></summary>
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/States/StateMachine.h#L1-L62
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/States/PlayerState.h#L1-L259
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/States/PlayerState.cpp#L1-L531
  </details>
</details>

## 3. 애니메이션 & 이펙트 시스템
<details>
  <summary><b>보기</b></summary>
  <details>
    <summary><b>Resource</b></summary>
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/ResourceManager.h#L1-L35
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/ResourceManager.cpp#L1-L174
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Resources/Texture.h#L1-L32
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Resources/Texture.cpp#L1-L131
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Resources/Sprite.h#L1-L32
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Resources/Sprite.cpp#L1-L191
  </details>
  <details>
    <summary><b>Animator & Effector</b></summary>
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/Animator.cpp#L1-L91
    https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Components/EffectorComponent.cpp#L1-L252
  </details>
</details>

## 4. 슬로모션
<details>
  <summary><b>보기</b></summary>
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/TimeManager.h#L1-L72
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/TimeManager.cpp#L1-L138
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Game/Game.cpp#L100-L202
</details>

## 5. 되감기
<details>
  <summary><b>보기</b></summary>
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/GamePlayManager.h#L1-L46
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/GamePlayManager.cpp#L1-L71
</details>

## 6. 리플레이
<details>
  <summary><b>보기</b></summary>
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/InputManager.cpp#L20-L26
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/InputManager.cpp#L50-L83
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Game/Game.cpp#L240-L269
</details>


## 7. 충돌 시스템
<details>
  <summary><b>보기</b></summary>
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/CollisionManager.h#L1-L100
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Managers/CollisionManager.cpp#L1-L1045
  https://github.com/SeongJeGyeong/WinAPI_KatanaZero/blob/be2394ca691c3413c53058de85bf2bb47cc77aae/Katana_Zero/Objects/Actors/Player.cpp#L575-L662
</details>

