# Niagara VFX 빠른 시작 가이드

## 🌧️ 비 효과 만들기 (5분)

### 1. 비 Niagara System 생성

```
Content Browser → Content/Weather/VFX/ 폴더로 이동
우클릭 → FX → Niagara System
→ "New system from selected emitters" 또는 "New system from a template" 선택
→ Template: Rain (있다면) 또는 Simple Sprite Burst
→ 이름: NS_Rain
```

### 2. 기본 설정 (템플릿 없이 만드는 경우)

**Emitter 추가:**
```
1. Niagara System 열기
2. '+' 버튼 → Add Emitter → New Empty Emitter
3. Emitter 이름: RainDrops
```

**Spawn Rate 설정:**
```
Emitter Update → Spawn Rate
- Spawn Rate: 500 ~ 1000 (비 밀도)
```

**초기 파라미터:**
```
Particle Spawn 섹션:
- Initialize Particle
  - Lifetime: 2.0 ~ 3.0
  - Color: 흰색 또는 연한 파란색
  - Sprite Size: (0.05, 0.3) - 얇고 긴 빗방울 모양

- Add Velocity
  - Velocity: (0, 0, -800) - 아래로 떨어지는 속도
  - Velocity Random: 약간의 랜덤 추가
```

**모양 설정:**
```
Particle Update → Sprite Rotation Rate
- Rotation Rate: 0 (비는 회전하지 않음)
```

**렌더러:**
```
Render 섹션:
- Sprite Renderer
  - Sprite Alignment: Velocity Aligned (속도 방향으로 정렬)
```

---

## ❄️ 눈 효과 만들기 (5분)

### 1. 눈 Niagara System 생성

```
위와 동일하게 생성, 이름: NS_Snow
Template: Snow (있다면)
```

### 2. 기본 설정

**눈송이 특성:**
```
Particle Spawn:
- Lifetime: 5.0 ~ 8.0 (천천히 내림)
- Sprite Size: (0.2, 0.2) ~ (0.5, 0.5) (둥근 눈송이)
- Velocity: (0, 0, -200) (비보다 느림)

Particle Update:
- Drag: 0.5 (공기 저항)
- Add Force: (Wind 추가)
  - Force: (50, 0, 0) - 바람에 날림
```

**회전 효과:**
```
Particle Update:
- Rotation Rate: 30 ~ 60 (눈송이 회전)
```

---

## 🌫️ 안개 효과 만들기 (옵션)

안개는 Niagara보다 **Exponential Height Fog** 사용을 추천합니다 (POSkySystem에 이미 포함됨)

하지만 지면 안개가 필요하다면:

```
Template: Smoke 사용
- Sprite Size: 크게 (10 ~ 20)
- Color: 회색 또는 흰색
- Lifetime: 길게 (10초+)
- Velocity: 매우 느리게
- 지면 근처에만 스폰되도록 Box Location 설정
```

---

## 🎯 WeatherSystemManager에 연결하기

### VFX 컴포넌트 추가 (C++ 확장 필요)

현재 `POWeatherSystemManager`를 확장해서 Niagara Component를 동적으로 제어할 수 있습니다:

**방법 1: Blueprint로 확장 (추천)**
```
1. POWeatherSystemManager 기반 Blueprint 생성 (BP_WeatherSystemManager)
2. Niagara Component 추가:
   - RainComponent (NS_Rain)
   - SnowComponent (NS_Snow)
3. Event Graph에서:
   - On Weather Changed 이벤트 만들기
   - 날씨에 따라 Activate/Deactivate
```

**방법 2: C++ 확장**
```cpp
// POWeatherSystemManager.h에 추가
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather|VFX")
TObjectPtr<UNiagaraComponent> RainVFXComponent;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather|VFX")
TObjectPtr<UNiagaraComponent> SnowVFXComponent;

// 생성자에서
RainVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RainVFX"));
SnowVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnowVFX"));

// ApplyWeatherEffects에서
if (Weather == EWeatherType::Rainy && Intensity > 0.0f)
{
    RainVFXComponent->Activate();
    // Spawn Rate 조절 등
}
```

---

## 📦 에셋 마켓 활용 (선택사항)

무료로 좋은 VFX를 원한다면:

### **Unreal Marketplace - Free 섹션:**
- **Realistic Starter VFX Pack** (무료)
- **Particle Effects** (무료)
- **Niagara Fluids** (UE5용)

### **다운로드 방법:**
```
Epic Games Launcher → Unreal Engine → Marketplace
→ Free 필터 적용 → VFX/Particles 검색
→ 프로젝트에 Add to Project
```

---

## ⚡ 빠른 테스트

### 에디터에서 바로 테스트:
```
1. NS_Rain을 뷰포트에 드래그 앤 드롭
2. 플레이 버튼 클릭
3. 효과 확인
```

### 런타임에서 스폰:
```blueprint
Spawn System at Location
- Emitter Template: NS_Rain
- Location: Player 위치 + (0, 0, 1000)
- Auto Destroy: true
```

---

## 🎨 고급 팁

### 성능 최적화:
- **LOD 설정**: 거리에 따라 Spawn Rate 줄이기
- **Bounds**: Effect 범위를 플레이어 주변으로 제한
- **Pooling**: Object Pooling으로 재사용

### 리얼리즘 향상:
- **Collision**: Ground와 충돌하는 Splash 효과
- **GPU Particles**: 많은 양의 파티클에는 GPU Simulation 사용
- **Audio**: 날씨 효과에 맞는 사운드 추가

---

## ✅ 체크리스트

- [ ] NS_Rain 생성 및 테스트
- [ ] NS_Snow 생성 및 테스트
- [ ] WeatherStateDataAsset에 VFX 할당
- [ ] WeatherSystemManager에서 VFX 활성화/비활성화 구현
- [ ] 성능 테스트 (프레임 드롭 확인)

---

**다음:** Post Process 효과와 Material 반응 구현하기
