// Copyright Epic Games, Inc. All Rights Reserved.

#include "POTimeOfDayManager.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveLinearColor.h"

APOTimeOfDayManager::APOTimeOfDayManager()
{
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 생성
	Sun = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sun"));
	SetRootComponent(Sun);

	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphere->SetupAttachment(Sun);

	HeightFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFog"));
	HeightFog->SetupAttachment(Sun);
}

void APOTimeOfDayManager::BeginPlay()
{
	Super::BeginPlay();

	// 초기 시간 적용
	UpdateSkySystem();
	UpdateMaterialParameters();
}

void APOTimeOfDayManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 시간 전환 중이면 전환 업데이트
	if (bIsTransitioning)
	{
		float ElapsedTime = GetWorld()->GetTimeSeconds() - TransitionStartTime;
		TransitionProgress = FMath::Clamp(ElapsedTime / TransitionDuration, 0.0f, 1.0f);

		// Smooth Step 보간
		float Alpha = FMath::SmoothStep(0.0f, 1.0f, TransitionProgress);
		CurrentTime = FMath::Lerp(TransitionStartValue, TransitionTargetValue, Alpha);

		// 전환 완료
		if (TransitionProgress >= 1.0f)
		{
			bIsTransitioning = false;
		}
	}
	else if (bAutoProgress)
	{
		UpdateTime(DeltaTime);
	}

	// Sky System 및 Material 업데이트
	UpdateSkySystem();
	UpdateMaterialParameters();
}

void APOTimeOfDayManager::SetTimeOfDay(float NewTime)
{
	CurrentTime = FMath::Fmod(NewTime, 24.0f);
	if (CurrentTime < 0.0f)
	{
		CurrentTime += 24.0f;
	}

	bIsTransitioning = false;
	UpdateSkySystem();
	UpdateMaterialParameters();
}

void APOTimeOfDayManager::TransitionToTime(float TargetTime, float Duration)
{
	bIsTransitioning = true;
	TransitionStartValue = CurrentTime;
	TransitionTargetValue = FMath::Fmod(TargetTime, 24.0f);
	TransitionDuration = FMath::Max(Duration, 0.1f);
	TransitionProgress = 0.0f;
	TransitionStartTime = GetWorld()->GetTimeSeconds();
}

void APOTimeOfDayManager::SetTimePreset(ETimeOfDayPreset Preset)
{
	float PresetTime = 12.0f;

	switch (Preset)
	{
	case ETimeOfDayPreset::Dawn:
		PresetTime = 6.0f;
		break;
	case ETimeOfDayPreset::Morning:
		PresetTime = 9.0f;
		break;
	case ETimeOfDayPreset::Noon:
		PresetTime = 12.0f;
		break;
	case ETimeOfDayPreset::Afternoon:
		PresetTime = 15.0f;
		break;
	case ETimeOfDayPreset::Dusk:
		PresetTime = 18.0f;
		break;
	case ETimeOfDayPreset::Night:
		PresetTime = 21.0f;
		break;
	case ETimeOfDayPreset::Midnight:
		PresetTime = 0.0f;
		break;
	}

	SetTimeOfDay(PresetTime);
}

void APOTimeOfDayManager::GetFormattedTime(int32& OutHours, int32& OutMinutes) const
{
	OutHours = static_cast<int32>(CurrentTime);
	OutMinutes = static_cast<int32>((CurrentTime - OutHours) * 60.0f);
}

void APOTimeOfDayManager::UpdateTime(float DeltaTime)
{
	// 시간 진행 (TimeSpeed에 따라 가속)
	CurrentTime += (DeltaTime / 3600.0f) * TimeSpeed;

	// 24시간 루프
	if (bLoopTime && CurrentTime >= 24.0f)
	{
		CurrentTime = FMath::Fmod(CurrentTime, 24.0f);
	}
}

void APOTimeOfDayManager::UpdateSkySystem()
{
	if (!Sun)
	{
		return;
	}

	// 태양 회전 설정
	FRotator SunRotation = CalculateSunRotation(CurrentTime);
	Sun->SetWorldRotation(SunRotation);

	// 태양 강도 설정
	float SunIntensity = CalculateSunIntensity(CurrentTime);
	Sun->SetIntensity(SunIntensity);

	// 태양 색상 설정
	FLinearColor SunColor = CalculateSunColor(CurrentTime);
	Sun->SetLightColor(SunColor);
}

void APOTimeOfDayManager::UpdateMaterialParameters()
{
	if (!TimeOfDayMPC)
	{
		return;
	}

	UMaterialParameterCollectionInstance* MPCInstance = GetWorld()->GetParameterCollectionInstance(TimeOfDayMPC);
	if (!MPCInstance)
	{
		return;
	}

	MPCInstance->SetScalarParameterValue(FName("TimeOfDay"), CurrentTime);

	// 낮/밤 블렌드 값 (0.0 = 밤, 1.0 = 낮)
	float DayNightBlend = CalculateSunIntensity(CurrentTime) / MaxSunIntensity;
	MPCInstance->SetScalarParameterValue(FName("DayNightBlend"), DayNightBlend);
}

FRotator APOTimeOfDayManager::CalculateSunRotation(float TimeOfDay) const
{
	float Pitch;
	if (TimeOfDay >= 6.0f && TimeOfDay <= 18.0f)
	{
		// 낮: 6시(0도) → 12시(-90도) → 18시(0도)
		float DayProgress = (TimeOfDay - 6.0f) / 12.0f; // 0.0 ~ 1.0
		Pitch = -FMath::Sin(DayProgress * PI) * 90.0f;  // 0 → -90 → 0
	}
	else
	{
		// 밤: 지평선 완전히 아래
		Pitch = 90.0f;
	}

	// 방위각: 동쪽(6시) → 남쪽(12시) → 서쪽(18시)
	float Yaw = (TimeOfDay / 24.0f) * 360.0f - 90.0f;

	return FRotator(Pitch, Yaw, 0.0f);
}

float APOTimeOfDayManager::CalculateSunIntensity(float TimeOfDay) const
{
	if (TimeOfDay >= 6.0f && TimeOfDay <= 18.0f)
	{
		// 낮: Sin 곡선으로 부드러운 밝기 변화 (일출/일몰 → 0, 정오 → 최대)
		float DayProgress = (TimeOfDay - 6.0f) / 12.0f;
		float Intensity = FMath::Sin(DayProgress * PI);
		return FMath::Lerp(0.0f, MaxSunIntensity, Intensity);
	}
	else
	{
		// 밤: 태양 강도 0 (달빛은 SkyLight로 표현)
		return 0.0f;
	}
}

FLinearColor APOTimeOfDayManager::CalculateSunColor(float TimeOfDay) const
{
	if (TimeOfDay >= 5.0f && TimeOfDay <= 7.0f)
	{
		return FLinearColor(1.0f, 0.6f, 0.4f);
	}
	else if (TimeOfDay >= 7.0f && TimeOfDay <= 17.0f)
	{
		return FLinearColor(1.0f, 0.95f, 0.85f);
	}
	else if (TimeOfDay >= 17.0f && TimeOfDay <= 19.0f)
	{
		return FLinearColor(1.0f, 0.5f, 0.3f);
	}
	else
	{
		return FLinearColor(0.4f, 0.5f, 0.8f);
	}
}
