# Crystal

> DirectX 12 기반의 자체 게임 엔진과 우주 슈팅 게임

PBR 렌더링, Behavior Tree AI, 멀티스레드 물리를 구현한 
C++ 게임 엔진 프로젝트입니다.

![C++](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus)
![DirectX](https://img.shields.io/badge/DirectX-12-107C10?logo=microsoft)
![Visual Studio](https://img.shields.io/badge/Visual_Studio-2022-5C2D91?logo=visualstudio)
![Windows](https://img.shields.io/badge/Windows-10/11-0078D6?logo=windows)

---

## Demo

https://github.com/user-attachments/assets/b0c6d7ab-ad16-47a5-9000-9b3a9c481f0b


> 우주 공간에서 Drone AI와 교전하는 우주 슈팅 게임. 



## DirectX 12 Deferred Rendering Pipeline

상용 엔진의 렌더링 구조를 이해하기 위해 **Deferred Rendering 파이프라인**을 설계했습니다. 다중 광원 환경에서 Forward 대비 픽셀 셰이더 호출 비용을 
크게 줄일 수 있다는 점에서 채택했습니다.

### 핵심 설계

- **6장의 G-Buffer 분리 저장**  
  Albedo / Normal / WorldPosition / Emissive / Irradiance / Metallic-Roughness
- **HDR 파이프라인**: `R16G16B16A16_FLOAT` 포맷으로 광원 누적 시 색상 범위 보존
- **Shadow Mapping**: 2048×2048 별도 패스 분리
- **Post-Process Chain**: Bloom (Bright Pass → Downsample → Blur → Composite)

### 렌더링 흐름

```
Geometry Pass → G-Buffer 6장 동시 출력
    ↓
Shadow Pass → 2048×2048 Depth Map 생성
    ↓
Light Pass → G-Buffer 샘플링으로 PBR 라이팅 계산
    ↓
Post-Process → HDR Bloom + Tone Mapping
    ↓
Back Buffer
```

### 트러블슈팅: Root Signature 계층 분리

초기 구현에서는 모든 상수 버퍼를 매 Draw Call마다 갱신했지만, 
Descriptor Heap 갱신 비용이 병목이 되었습니다. 

이를 **PerFrame / PerObject / PerExecute** 3계층으로 분리하여, 
프레임 단위 데이터는 1회만 바인딩하도록 개선했습니다.

```cpp
// Root Signature 계층화
int GetPerFrameParameterIndex() const;    // 카메라, 라이트 등
int GetPerObjectParameterIndex() const;   // 오브젝트별 트랜스폼
int GetPerExecuteParameterIndex() const;  // 머티리얼별 텍스처
```

## Behavior Tree + Blackboard AI System

언리얼 엔진의 AI 아키텍처를 참고하여 **계층적 행동 트리 시스템**을 
구현했습니다. 단순 FSM으로는 표현하기 어려운 복잡한 의사결정 
(추적/공격/순찰/시야 이탈 후 마지막 위치 탐색)을 모듈화된 노드 조합으로 처리합니다.

### 핵심 설계

- **Composite / Decorator / Task 노드 분리**  
  Selector, Sequence 등 조합 노드와 실제 행동 노드(Task)를 명확히 분리
- **Blackboard 기반 데이터 공유**  
  AI 노드 간 상태를 키-값으로 공유 (PlayerPawn, LastSeenLocation 등)
- **AIPerception 통합**: 시야 감지 → Blackboard 자동 업데이트 → BT 재평가
- **Abort 시스템**: 우선순위 변경 시 실행 중인 하위 노드 안전 중단

### 트러블슈팅: Abort 조건 설계

가장 까다로웠던 부분은 **실행 중인 노드를 안전하게 중단**하는 메커니즘입니다.
예를 들어 적이 순찰 중에 플레이어를 발견했을 때, 진행 중인 MoveTo 노드를 
즉시 멈추고 공격 시퀀스로 전환해야 합니다.

이를 위해 **두 가지 Abort 조건**을 분리 구현했습니다:

```cpp
enum class EAbortCondition {
    AC_OnResultChange,  // Decorator 결과가 바뀔 때 중단
    AC_OnValueChange,   // Blackboard 값 자체가 바뀔 때 중단
};

enum class EDecoratorAbortType {
    DAT_Self,           // 자기 자신만 중단
    DAT_LowerPriority,  // 하위 우선순위 노드 중단
    DAT_Both            // 양쪽 모두
};
```

`OnResultChange`는 단순 true/false 변화를 감지하고, `OnValueChange`는 
값의 ID 자체가 바뀌어야 트리거됩니다 (같은 결과여도 대상이 바뀌면 재평가 필요).

### 실제 적용: Drone AI

순찰 → 플레이어 감지 → 추적 → 공격 → 시야 잃음 → 마지막 위치 이동 → 순찰 복귀



## Engine Architecture

언리얼 엔진의 검증된 패턴을 학습 목적으로 참고하여, **Object 기반 계층 구조**와 
**Actor-Component 시스템**으로 엔진을 설계했습니다.

### 핵심 구조

- **Object**: 모든 엔진 객체의 베이스 클래스, 자체 RTTI(`STATIC_TYPE_IMPLE`) 제공
- **Actor**: 월드에 배치되는 게임 오브젝트
- **Component**: 액터에 부착되는 기능 단위 (Transform, Collision, Mesh 등)
- **Outer 시스템**: 객체 소유권 명시 → 메모리 누수 방지
- **Shared/Weak 래퍼**: `std::shared_ptr` 기반 + 순환 참조 차단

### 라이프사이클

```cpp
Initialize() → OnCreate() → Begin() → Update(dt) → ~Object()
```

---

## 추가 구현 기능

- **멀티스레드 물리 시스템**: Sphere / OBB / AABB 충돌, 충격량 기반 응답
- **HUD 시스템**: 체력바, 퀘스트 UI, 미니맵, 호버링 디테일
- **인벤토리/포션 시스템**: 게임 시스템 설계
- **포스트프로세스 볼륨**: 피격/힐/실드 상태 시각 피드백

---








# 목차

**1. Deferred Rendering**

**2. Instancing**

**3. Irradiance Mapping (SkyLight)**

**4. Tone Mapping**

**5. Bloom Effect**

**6. Shadow Mapping**

# Deferred Rendering
## 사진
<img src="ReadMeResource/Albedo.png" width="30%" height="30%"></img> 
<img src="ReadMeResource/Emissive.png" width="30%" height="30%"></img>
<img src="ReadMeResource/Irradiance.png" width="30%" height="30%"></img>
<img src="ReadMeResource/RoughnessMetallic.png" width="30%" height="30%"></img>
<img src="ReadMeResource/WorldNormal.png" width="30%" height="30%"></img>
<img src="ReadMeResource/WorldPosition.png" width="30%" height="30%"></img>
###### <center><순서대로 AlbedoBuffer, EmissiveBuffer, RoughnessMetallicBuffer, WorldNormalBuffer, WorldPositionBuffer></center>

<img src="ReadMeResource/Result.png" width="92%" height="92%"></img> 
###### <center><최종 렌더링 이미지></center>

## 세부 설명
빛에 영향을 받는 오브젝트들을 우선적으로 Depth Test를 통과시키고 최종적으로 화면을 꽉채우는 사각형에 화면상에 나타나는 픽셀들에 대해서만 라이팅 계산을하여 퍼포먼스 향상.


# Instancing
## 사진

<img src="ReadMeResource/Instance_Mesh.png" width="90%" height="90%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center><인스턴싱을 하는 메쉬></center>

<img src="ReadMeResource/DrawCall.png" width="90%" height="90%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center><인스턴싱을 하는 메쉬의 DrawCall></center>
<img src="ReadMeResource/Instancing_InputLayout.png" width="60%" height="60%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center><인스턴싱을 하는 메쉬의 InputLayout Data></center>

## 세부 설명
기존의 ConstantBuffer로 GPU에 보내는 Data를 InputLayout으로 옮겨서 하나의 DrawCall로 처리. DrawCall을 줄임으로써 퍼포먼스 향상.

# Irradiance Mapping (SkyLight)
## 사진

<img src="ReadMeResource/NoSkyLight.png" width="80%" height="80%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center>< No Skylight ></center>
<img src="ReadMeResource/SkyLight.png" width="80%" height="80%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center>< Skylight ></center>
<img src="ReadMeResource/SkyLightCode.png" width="100%" height="100%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center>< Skylight Convolute 코드 ></center>

## 세부 설명
Scene의 Skybox의 색상이 오브젝트에 영향을 끼치게 하여 좀더 Scene과 어울리는 색상을 표현. 구면좌표계에 있는 Skybox의 좌표를 이산적인 양을 증가 시키면서 Convolute하고 후에 메쉬의 표면 노말을 입력을 텍스쳐 좌표로 이용하여 샘플링하여 빛을 계산.


# Tone Mapping
## 개요

<img src="ReadMeResource/NoToneMapping.png" width="80%" height="80%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center>< Tone Mapping 이전 ></center>
<img src="ReadMeResource/ToneMapping.png" width="80%" height="80%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center>< Tone Mapping 적용 ></center>

## 세부 설명
HDR 밝기 영역으로 표현된 Scene을 모니터가 표현할 수 있는 영역으로 압축하여 표현. 기존의 Scene을 Floating-Point-Buffer에 렌더링한 후 최종 렌더링된 Scene을 Reinhard Tone Mapping 기법을 사용하여 압축.

# Bloom Effect
## 사진

<img src="ReadMeResource/WithoutBloom.png" width="80%" height="80%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center>< Without Bloom ></center>
<img src="ReadMeResource/WithBloom.png" width="80%" height="80%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center>< With Bloom ></center>
<img src="ReadMeResource/Brightness.png" width="80%" height="80%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center>< Bright Color Blurred ></center>

## 세부 설명
실제 카메라 렌즈에 빛번짐 효과를 구현. Floating-Point-Buffer에 렌더링된 Scene에서 일정 밝기 이상의 부분을 추출하여 텍스쳐에 저장. 이 텍스쳐를 Compute Shader를 사용하여 여러번 가우시안 블러링한 후 기존에 Scene과 Additive Blending을 함.


# Shadow Mapping
## 사진

<img src="ReadMeResource/shadow.png" width="80%" height="80%" style="margin-left: auto; margin-right: auto; display: block;"></img>
###### <center>< Shadow Map ></center>

## 세부 설명
Directional Light로부터 생기는 Shadow를 구현. Directional Light의 방향을 바라보는 플레이어를 따라다니는 카메라를 만들고, Scene을 렌더링(Depth만). 렌더링된 Shadow Map을 이후에 라이팅을 계산할 때, 월드공간의 좌표를 Shadow Map의 공간으로 변환 후 샘플링 하여 사용. PCF(Percentage Closer Filtering)기법을 사용하여 그림자의 외곽부분을 자연스럽게 표현.




















