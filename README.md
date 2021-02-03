# Crystal
DirectX12기반의 졸업작품 프레임워크를 개발합니다.
## 목차
1. [기능](#기능)
2. [주차별 진행 사항](#주차별-진행-사항)

## 기능

## 주차별 진행 사항
### 0주차 (2021-01-03 ~ 2021-01-09)
  * Physically Based Rendering(PBR)이론 조사 및 학습.
  * DirectX12 초기화 및 API 래핑.
  * 전체화면<->창모드 변경 및 해상도 변경 구현.
### 1주차 (2021-01-03 ~ 2021-01-09)
  * 기본적인 Physically Based Rendering(PBR) 적용. (Directional Light에 대한 Material에 따른 Specular, Diffuse 반응).
  * Full Screen Quad Skybox 구현.
  * Skinned Animation에 대한 이론 조사 및 학습.
### 2주차 (2021-01-10 ~ 2021-01-16)
  * Skinned Animation의 기본적인 부분 구현. (기능 구현만 했고 후에 Animation을 조정하기 위한 시스템을 구현하여야함)
  * ImGUI라이브러리를 추가.
### 3주차 (2021-01-17 ~ 2021-01-23)
  * 런타임에 Equirectangular image를 Cubemap(Texture array)으로 변환(추출)하는 Compute Shader를 구현.
### 4주차 (2021-01-24 ~ 2021-01-30)
  * 기본적인 Material System 및 Actor - Component시스템 구현.
  * 런타임에 Cubemap으로부터 Diffuse Irradiance map을 샘플링하는 Compute Shader를 구현.
### 5주차 (2021-01-31 ~ 2021-02-06)
  * 멀티스레드를 사용하여 Rendering work를 submit할 수 있도록 리팩토링.
  * Specular Irradiance map에 대한 이론 조사 및 학습.
