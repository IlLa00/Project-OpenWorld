// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeatherTypes.h"
#include "WeatherStateDataAsset.generated.h"

class UNiagaraSystem;
class UMaterialInterface;

/**
 * 날씨 파라미터 구조체
 * Material Parameter Collection에 적용될 값들
 */
USTRUCT(BlueprintType)
struct FWeatherMaterialParameters
{
	GENERATED_BODY()

	/** 지표면 젖음 정도 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Wetness = 0.0f;

	/** 눈 적설량 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SnowCoverage = 0.0f;

	/** 안개 밀도 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FogDensity = 0.0f;

	/** 비 강도 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RainIntensity = 0.0f;

	/** 바람 세기 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WindStrength = 0.0f;

	/** 바람 방향 (각도) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters")
	float WindDirection = 0.0f;
};

/**
 * Niagara VFX 설정
 */
USTRUCT(BlueprintType)
struct FWeatherVFXSettings
{
	GENERATED_BODY()

	/** Niagara 파티클 시스템 (비, 눈 등) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UNiagaraSystem> ParticleSystem;

	/** VFX 스폰 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	float SpawnRate = 100.0f;

	/** VFX 범위 (반경) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	float EffectRadius = 5000.0f;

	/** VFX 높이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	float EffectHeight = 1000.0f;
};

/**
 * Post Process 설정
 */
USTRUCT(BlueprintType)
struct FWeatherPostProcessSettings
{
	GENERATED_BODY()

	/** Color Grading - 색온도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Process")
	float ColorTemperature = 6500.0f;

	/** Color Grading - 채도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Process", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float Saturation = 1.0f;

	/** Color Grading - 대비 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Process", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float Contrast = 1.0f;

	/** 전체 밝기 보정 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Process")
	float ExposureCompensation = 0.0f;

	/** Bloom 강도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Process")
	float BloomIntensity = 1.0f;
};

/**
 * 대기 효과 설정 (Sky Atmosphere, Height Fog 등)
 */
USTRUCT(BlueprintType)
struct FWeatherAtmosphereSettings
{
	GENERATED_BODY()

	/** Exponential Height Fog 밀도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	float FogDensity = 0.02f;

	/** Fog 색상 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	FLinearColor FogInscatteringColor = FLinearColor(0.447f, 0.638f, 1.0f);

	/** Directional Inscattering Exponent */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	float DirectionalInscatteringExponent = 4.0f;

	/** Sky Atmosphere - Rayleigh Scattering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	FLinearColor RayleighScatteringScale = FLinearColor(0.0331f, 0.0796f, 0.17f);

	/** Sky Atmosphere - Mie Scattering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	float MieScatteringScale = 3.996f;
};

/**
 * 오디오 설정
 */
USTRUCT(BlueprintType)
struct FWeatherAudioSettings
{
	GENERATED_BODY()

	/** Ambient 사운드 (빗소리, 바람소리 등) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> AmbientSound;

	/** 사운드 볼륨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Volume = 0.5f;

	/** 사운드가 페이드 인/아웃 되는 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float FadeTime = 2.0f;
};

/**
 * 날씨 상태별 데이터 에셋
 * 각 날씨(맑음, 비, 눈 등)의 모든 설정을 정의
 */
UCLASS(BlueprintType)
class PROJECT_OPENWORLD_API UWeatherStateDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 날씨 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather")
	EWeatherType WeatherType = EWeatherType::Clear;

	/** 날씨 이름 (표시용) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather")
	FText WeatherName;

	/** 날씨 설명 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather", meta = (MultiLine = true))
	FText Description;

	/** Material Parameter Collection 파라미터 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Parameters")
	FWeatherMaterialParameters MaterialParameters;

	/** VFX 설정 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|VFX")
	FWeatherVFXSettings VFXSettings;

	/** Post Process 설정 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Post Process")
	FWeatherPostProcessSettings PostProcessSettings;

	/** 대기 효과 설정 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Atmosphere")
	FWeatherAtmosphereSettings AtmosphereSettings;

	/** 오디오 설정 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Audio")
	FWeatherAudioSettings AudioSettings;

	/** 이 날씨로 자동 전환될 확률 가중치 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Auto Change", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RandomWeightProbability = 0.2f;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		// 에디터에서 값 변경 시 처리할 로직
	}
#endif
};
