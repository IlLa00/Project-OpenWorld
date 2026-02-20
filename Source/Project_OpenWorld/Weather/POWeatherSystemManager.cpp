#include "POWeatherSystemManager.h"
#include "WeatherStateDataAsset.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../RVT/PORVTManager.h"

APOWeatherSystemManager::APOWeatherSystemManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APOWeatherSystemManager::BeginPlay()
{
	Super::BeginPlay();

	// RVTManager 자동 탐색 (에디터에서 직접 할당하지 않은 경우)
	if (!RVTManager)
	{
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), APORVTManager::StaticClass());
		RVTManager = Cast<APORVTManager>(FoundActor);
		if (RVTManager)
		{
			UE_LOG(LogTemp, Log, TEXT("[WeatherSystem] RVTManager 자동 탐색 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[WeatherSystem] RVTManager를 찾을 수 없음. 레벨에 BP_PORVTManager를 배치하세요."));
		}
	}

	// 초기 날씨 적용
	SetWeatherImmediate(CurrentWeather);

	// 자동 날씨 변경 타이머 초기화
	if (bEnableAutoWeatherChange)
	{
		TimeUntilNextWeatherChange = WeatherChangeInterval;
	}
}

void APOWeatherSystemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 날씨 전환 업데이트
	if (TransitionInfo.bIsTransitioning)
	{
		UpdateWeatherTransition(DeltaTime);
	}

	// 자동 날씨 변경 업데이트
	if (bEnableAutoWeatherChange)
	{
		UpdateAutoWeatherChange(DeltaTime);
	}
}

void APOWeatherSystemManager::SetWeatherImmediate(EWeatherType NewWeather)
{
	CurrentWeather = NewWeather;
	TransitionInfo.bIsTransitioning = false;
	TransitionInfo.TransitionProgress = 1.0f;

	// 날씨 효과 즉시 적용 (강도 100%)
	ApplyWeatherEffects(NewWeather, 1.0f);
	UpdateMaterialParameters();

	UE_LOG(LogTemp, Log, TEXT("[WeatherSystem] Weather changed immediately to: %d"), (int32)NewWeather);
}

void APOWeatherSystemManager::TransitionToWeather(EWeatherType NewWeather, float Duration)
{
	if (NewWeather == CurrentWeather)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeatherSystem] Already in target weather state"));
		return;
	}

	// 전환 정보 설정
	TransitionInfo.bIsTransitioning = true;
	TransitionInfo.PreviousWeather = CurrentWeather;
	TransitionInfo.TargetWeather = NewWeather;
	TransitionInfo.TransitionDuration = FMath::Max(Duration, 0.1f); // 최소 0.1초
	TransitionInfo.TransitionProgress = 0.0f;

	TransitionStartTime = GetWorld()->GetTimeSeconds();

	UE_LOG(LogTemp, Log, TEXT("[WeatherSystem] Starting transition from %d to %d (Duration: %.2f)"),
		(int32)CurrentWeather, (int32)NewWeather, Duration);
}

void APOWeatherSystemManager::TransitionToRandomWeather(float Duration)
{
	// 현재 날씨를 제외한 랜덤 날씨 선택
	TArray<EWeatherType> AllWeatherTypes = {
		EWeatherType::Clear,
		EWeatherType::Cloudy,
		EWeatherType::Rainy,
		EWeatherType::Snowy,
		EWeatherType::Foggy,
		EWeatherType::Stormy
	};

	AllWeatherTypes.Remove(CurrentWeather);

	if (AllWeatherTypes.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, AllWeatherTypes.Num() - 1);
		EWeatherType RandomWeather = AllWeatherTypes[RandomIndex];
		TransitionToWeather(RandomWeather, Duration);
	}
}

void APOWeatherSystemManager::UpdateWeatherTransition(float DeltaTime)
{
	// 전환 진행도 계산
	float ElapsedTime = GetWorld()->GetTimeSeconds() - TransitionStartTime;
	TransitionInfo.TransitionProgress = FMath::Clamp(ElapsedTime / TransitionInfo.TransitionDuration, 0.0f, 1.0f);

	// 부드러운 전환을 위함
	float SmoothedProgress = FMath::SmoothStep(0.0f, 1.0f, TransitionInfo.TransitionProgress);

	// 이전 날씨 효과 페이드아웃, 새 날씨 효과 페이드인
	ApplyWeatherEffects(TransitionInfo.PreviousWeather, 1.0f - SmoothedProgress);
	ApplyWeatherEffects(TransitionInfo.TargetWeather, SmoothedProgress);

	// MPC 파라미터 업데이트
	UpdateMaterialParameters();

	// 전환 완료 체크
	if (TransitionInfo.TransitionProgress >= 1.0f)
	{
		OnTransitionComplete();
	}
}

// 날씨 타입 → 머티리얼 파라미터 변환 헬퍼
static void GetWeatherMaterialParams(EWeatherType Weather,
	float& OutWetness, float& OutSnowCoverage, float& OutRainIntensity,
	float& OutFogDensity, float& OutWindStrength)
{
	OutWetness = OutSnowCoverage = OutRainIntensity = OutFogDensity = OutWindStrength = 0.0f;

	switch (Weather)
	{
	case EWeatherType::Clear:
		// 모두 0 (기본값)
		break;
	case EWeatherType::Cloudy:
		OutFogDensity   = 0.2f;
		OutWindStrength = 0.3f;
		break;
	case EWeatherType::Rainy:
		OutWetness      = 1.0f;
		OutRainIntensity= 0.8f;
		OutFogDensity   = 0.3f;
		OutWindStrength = 0.5f;
		break;
	case EWeatherType::Snowy:
		OutSnowCoverage = 1.0f;
		OutFogDensity   = 0.4f;
		OutWindStrength = 0.4f;
		break;
	case EWeatherType::Foggy:
		OutFogDensity   = 1.0f;
		OutWindStrength = 0.1f;
		break;
	case EWeatherType::Stormy:
		OutWetness      = 1.0f;
		OutRainIntensity= 1.0f;
		OutFogDensity   = 0.5f;
		OutWindStrength = 1.0f;
		break;
	}
}

void APOWeatherSystemManager::UpdateMaterialParameters()
{
	if (!RVTManager)
	{
		return;
	}

	float Wetness, SnowCoverage, RainIntensity, FogDensity, WindStrength;

	if (TransitionInfo.bIsTransitioning)
	{
		// 전환 중: 이전 날씨와 목표 날씨 값을 보간
		float PrevWetness, PrevSnow, PrevRain, PrevFog, PrevWind;
		float NextWetness, NextSnow, NextRain, NextFog, NextWind;

		GetWeatherMaterialParams(TransitionInfo.PreviousWeather, PrevWetness, PrevSnow, PrevRain, PrevFog, PrevWind);
		GetWeatherMaterialParams(TransitionInfo.TargetWeather,   NextWetness, NextSnow, NextRain, NextFog, NextWind);

		float Alpha = FMath::SmoothStep(0.0f, 1.0f, TransitionInfo.TransitionProgress);
		Wetness      = FMath::Lerp(PrevWetness, NextWetness, Alpha);
		SnowCoverage = FMath::Lerp(PrevSnow,    NextSnow,    Alpha);
		RainIntensity= FMath::Lerp(PrevRain,    NextRain,    Alpha);
		FogDensity   = FMath::Lerp(PrevFog,     NextFog,     Alpha);
		WindStrength = FMath::Lerp(PrevWind,    NextWind,    Alpha);
	}
	else
	{
		// 전환 완료: 현재 날씨 파라미터 직접 사용
		GetWeatherMaterialParams(CurrentWeather, Wetness, SnowCoverage, RainIntensity, FogDensity, WindStrength);
	}

	// RVTManager에 파라미터 전달 (부드러운 보간은 RVTManager에서 처리)
	RVTManager->SetWeatherParametersTarget(Wetness, SnowCoverage, RainIntensity, FogDensity, WindStrength, 0.0f);
}

void APOWeatherSystemManager::UpdateAutoWeatherChange(float DeltaTime)
{
	TimeUntilNextWeatherChange -= DeltaTime;

	if (TimeUntilNextWeatherChange <= 0.0f)
	{
		// 랜덤 날씨로 전환
		TransitionToRandomWeather(5.0f);

		// 타이머 리셋
		TimeUntilNextWeatherChange = WeatherChangeInterval;
	}
}

void APOWeatherSystemManager::ApplyWeatherEffects(EWeatherType Weather, float Intensity)
{
	// TODO: Niagara 파티클 시스템 활성화/비활성화
	// TODO: PostProcess 효과 적용
	// TODO: 사운드 효과 재생

	// 현재는 로그만 출력
	if (Intensity > 0.0f && Intensity < 1.0f)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[WeatherSystem] Applying weather %d at intensity %.2f"),
			(int32)Weather, Intensity);
	}
}

void APOWeatherSystemManager::OnTransitionComplete()
{
	CurrentWeather = TransitionInfo.TargetWeather;
	TransitionInfo.bIsTransitioning = false;
	TransitionInfo.TransitionProgress = 1.0f;

	UE_LOG(LogTemp, Log, TEXT("[WeatherSystem] Transition complete. Current weather: %d"), (int32)CurrentWeather);
}
