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
	// 시간을 각도로 변환 (0시 = -90도, 12시 = 90도, 24시 = 270도)
	// Pitch: 태양의 고도각 (-90 ~ 90)
	// Yaw: 태양의 방위각 (동 → 남 → 서)

	// 0시(자정) = -90도, 6시(새벽) = 0도, 12시(정오) = 90도, 18시(저녁) = 0도, 24시 = -90도
	float NormalizedTime = (TimeOfDay - 6.0f) / 12.0f; // 6시를 0으로 정규화
	float Pitch = FMath::Sin(NormalizedTime * PI) * 90.0f; // -90 ~ 90도 범위

	// 방위각 (동쪽에서 떠서 서쪽으로 짐)
	float Yaw = (TimeOfDay / 24.0f) * 360.0f - 90.0f; // 동쪽(90도)에서 시작

	return FRotator(Pitch, Yaw, 0.0f);
}

float APOTimeOfDayManager::CalculateSunIntensity(float TimeOfDay) const
{
	// 낮(6~18시)에는 밝고, 밤(18~6시)에는 어두움
	float NormalizedTime = (TimeOfDay - 6.0f) / 12.0f; // 6시를 0, 18시를 1로 정규화

	if (TimeOfDay >= 6.0f && TimeOfDay <= 18.0f)
	{
		// 낮: Sin 곡선으로 부드러운 밝기 변화
		float Intensity = FMath::Sin(NormalizedTime * PI);
		return FMath::Lerp(MinSunIntensity, MaxSunIntensity, Intensity);
	}
	else
	{
		// 밤: 최소 강도
		return MinSunIntensity;
	}
}

FLinearColor APOTimeOfDayManager::CalculateSunColor(float TimeOfDay) const
{
	// 기본 색상 계산
	if (TimeOfDay >= 5.0f && TimeOfDay <= 7.0f)
	{
		// 새벽: 주황빛
		return FLinearColor(1.0f, 0.6f, 0.4f);
	}
	else if (TimeOfDay >= 7.0f && TimeOfDay <= 17.0f)
	{
		// 낮: 밝은 노란빛
		return FLinearColor(1.0f, 0.95f, 0.85f);
	}
	else if (TimeOfDay >= 17.0f && TimeOfDay <= 19.0f)
	{
		// 저녁: 붉은빛
		return FLinearColor(1.0f, 0.5f, 0.3f);
	}
	else
	{
		// 밤: 푸른빛 (달빛)
		return FLinearColor(0.4f, 0.5f, 0.8f);
	}
}
