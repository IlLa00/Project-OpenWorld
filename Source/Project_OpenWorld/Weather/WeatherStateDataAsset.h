#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeatherTypes.h"
#include "WeatherStateDataAsset.generated.h"

class UNiagaraSystem;
class UMaterialInterface;

USTRUCT(BlueprintType)
struct FWeatherMaterialParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Wetness = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SnowCoverage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FogDensity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RainIntensity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WindStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Parameters")
	float WindDirection = 0.0f;
};

USTRUCT(BlueprintType)
struct FWeatherVFXSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UNiagaraSystem> ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	float SpawnRate = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	float EffectRadius = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	float EffectHeight = 1000.0f;
};

USTRUCT(BlueprintType)
struct FWeatherPostProcessSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Process")
	float ColorTemperature = 6500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Process", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float Saturation = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Process", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float Contrast = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Process")
	float ExposureCompensation = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Process")
	float BloomIntensity = 1.0f;
};

USTRUCT(BlueprintType)
struct FWeatherAtmosphereSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	float FogDensity = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	FLinearColor FogInscatteringColor = FLinearColor(0.447f, 0.638f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	float DirectionalInscatteringExponent = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	FLinearColor RayleighScatteringScale = FLinearColor(0.0331f, 0.0796f, 0.17f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atmosphere")
	float MieScatteringScale = 3.996f;
};

USTRUCT(BlueprintType)
struct FWeatherAudioSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> AmbientSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Volume = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float FadeTime = 2.0f;
};

UCLASS(BlueprintType)
class PROJECT_OPENWORLD_API UWeatherStateDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather")
	EWeatherType WeatherType = EWeatherType::Clear;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather")
	FText WeatherName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weather", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Parameters")
	FWeatherMaterialParameters MaterialParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|VFX")
	FWeatherVFXSettings VFXSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Post Process")
	FWeatherPostProcessSettings PostProcessSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Atmosphere")
	FWeatherAtmosphereSettings AtmosphereSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Audio")
	FWeatherAudioSettings AudioSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Auto Change", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RandomWeightProbability = 0.2f;

};
