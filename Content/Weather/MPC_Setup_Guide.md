# Material Parameter Collection 생성 가이드

## 📋 MPC_GlobalWeather 생성 방법

### 1️⃣ MPC 에셋 생성

1. **Content Browser**에서 `Content/Weather/` 폴더로 이동 (없으면 생성)
2. 우클릭 → **Materials & Textures** → **Material Parameter Collection** 선택
3. 이름을 `MPC_GlobalWeather`로 설정

---

### 2️⃣ 스칼라 파라미터 추가

MPC 에셋을 더블클릭해서 열고, **Scalar Parameters** 섹션에 다음 파라미터들을 추가:

| Parameter Name | Default Value | Description |
|----------------|---------------|-------------|
| `Wetness` | `0.0` | 지표면 젖음 정도 (0.0 ~ 1.0) |
| `SnowCoverage` | `0.0` | 눈 적설량 (0.0 ~ 1.0) |
| `FogDensity` | `0.0` | 안개 밀도 (0.0 ~ 1.0) |
| `RainIntensity` | `0.0` | 비 강도 (0.0 ~ 1.0) |
| `WindStrength` | `0.3` | 바람 세기 (0.0 ~ 1.0) |
| `WindDirection` | `0.0` | 바람 방향 (각도 0.0 ~ 360.0) |
| `TimeOfDay` | `12.0` | 시간 (0.0 ~ 24.0) |

---

### 3️⃣ 벡터 파라미터 추가

**Vector Parameters** 섹션에 다음 파라미터들을 추가:

| Parameter Name | Default Value | Description |
|----------------|---------------|-------------|
| `WindVector` | `(1.0, 0.0, 0.0, 0.0)` | 바람 방향 벡터 (정규화된 방향) |
| `FogColor` | `(0.447, 0.638, 1.0, 1.0)` | 안개 색상 (Linear Color) |

---

### 4️⃣ WeatherSystemManager에 할당

1. 레벨에 **POWeatherSystemManager** 액터 배치
2. **Details** 패널에서 **Weather MPC** 항목에 방금 생성한 `MPC_GlobalWeather` 할당

---

## 🎨 머티리얼에서 사용하는 방법

### Landscape 머티리얼 예시

```
[MPC Parameter] Wetness
    ↓
[Lerp] A: Dry Texture, B: Wet Texture, Alpha: Wetness
    ↓
[Base Color]
```

### 눈 효과 예시

```
[MPC Parameter] SnowCoverage
    ↓
[Lerp] A: Ground Texture, B: Snow Texture, Alpha: SnowCoverage
    ↓
[Base Color]
```

### 바람 애니메이션 예시

```
[MPC Parameter] WindVector
    ↓
[Time] + [WindStrength]
    ↓
[World Position Offset] (나무/풀 흔들림)
```

---

## ✅ 검증 방법

1. MPC 에셋 열기
2. 각 파라미터 값을 수동으로 변경
3. 게임 실행 중 실시간으로 변경사항 확인
4. WeatherSystemManager가 자동으로 값을 업데이트하는지 확인

---

## 📌 참고사항

- MPC는 **전역 파라미터**이므로 모든 머티리얼에서 접근 가능
- 성능 영향이 거의 없음 (GPU 상수 버퍼 사용)
- 블루프린트에서도 `Get/Set Scalar/Vector Parameter Value` 노드로 접근 가능
