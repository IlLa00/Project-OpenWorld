#include "POWeatherSystemManager.h"
#include "WeatherStateDataAsset.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/GameplayStatics.h"

APOWeatherSystemManager::APOWeatherSystemManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APOWeatherSystemManager::BeginPlay()
{
	Super::BeginPlay();

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

void APOWeatherSystemManager::UpdateMaterialParameters()
{
	if (!WeatherMPC)
	{
		return;
	}

	UMaterialParameterCollectionInstance* MPCInstance = GetWorld()->GetParameterCollectionInstance(WeatherMPC);
	if (!MPCInstance)
	{
		return;
	}

	// 기본 파라미터 설정 (현재는 더미 값, 추후 WeatherDataAsset에서 가져올 예정)
	float Wetness = 0.0f;
	float SnowCoverage = 0.0f;
	float FogDensity = 0.0f;
	float RainIntensity = 0.0f;

	// 현재 날씨에 따라 파라미터 설정
	switch (CurrentWeather)
	{
	case EWeatherType::Clear:
		Wetness = 0.0f;
		SnowCoverage = 0.0f;
		FogDensity = 0.0f;
		RainIntensity = 0.0f;
		break;

	case EWeatherType::Cloudy:
		FogDensity = 0.2f;
		break;

	case EWeatherType::Rainy:
		Wetness = 1.0f;
		RainIntensity = 0.8f;
		FogDensity = 0.3f;
		break;

	case EWeatherType::Snowy:
		SnowCoverage = 1.0f;
		FogDensity = 0.4f;
		break;

	case EWeatherType::Foggy:
		FogDensity = 1.0f;
		break;

	case EWeatherType::Stormy:
		Wetness = 1.0f;
		RainIntensity = 1.0f;
		FogDensity = 0.5f;
		break;
	}

	// 전환 중이면 보간
	if (TransitionInfo.bIsTransitioning)
	{
		float Alpha = TransitionInfo.TransitionProgress;
		// TODO: 이전 날씨와 목표 날씨 값을 보간
	}

	// MPC 파라미터 설정
	MPCInstance->SetScalarParameterValue(FName("Wetness"), Wetness);
	MPCInstance->SetScalarParameterValue(FName("SnowCoverage"), SnowCoverage);
	MPCInstance->SetScalarParameterValue(FName("FogDensity"), FogDensity);
	MPCInstance->SetScalarParameterValue(FName("RainIntensity"), RainIntensity);
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
