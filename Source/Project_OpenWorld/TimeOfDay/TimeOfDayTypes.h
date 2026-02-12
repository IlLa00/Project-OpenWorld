#pragma once

#include "CoreMinimal.h"
#include "TimeOfDayTypes.generated.h"

USTRUCT(BlueprintType)
struct FTimeOfDaySettings
{
	GENERATED_BODY()

	// 태양 고도각 (0 = 수평선, 90 = 정오) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	float SunElevation = 45.0f;

	// 태양 방위각 (0 = 북, 90 = 동, 180 = 남, 270 = 서) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	float SunAzimuth = 0.0f;

	// 태양광 강도 (Lux) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	float SunIntensity = 10.0f;

	// 태양광 색상 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	FLinearColor SunColor = FLinearColor(1.0f, 0.95f, 0.85f);

	// 하늘 밝기 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	float SkyLightIntensity = 1.0f;

	// 안개 밀도 (시간대별로 다름) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	float FogDensity = 0.02f;

	// 안개 색상 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	FLinearColor FogColor = FLinearColor(0.447f, 0.638f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	float StarVisibility = 0.0f;
};

UENUM(BlueprintType)
enum class ETimeOfDayPreset : uint8
{
	Dawn		UMETA(DisplayName = "새벽 (06:00)"),
	Morning		UMETA(DisplayName = "아침 (09:00)"),
	Noon		UMETA(DisplayName = "정오 (12:00)"),
	Afternoon	UMETA(DisplayName = "오후 (15:00)"),
	Dusk		UMETA(DisplayName = "저녁 (18:00)"),
	Night		UMETA(DisplayName = "밤 (21:00)"),
	Midnight	UMETA(DisplayName = "자정 (00:00)")
};
