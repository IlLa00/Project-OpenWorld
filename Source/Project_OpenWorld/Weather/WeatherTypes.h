#pragma once

#include "CoreMinimal.h"
#include "WeatherTypes.generated.h"

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
	Clear		UMETA(DisplayName = "맑음"),
	Cloudy		UMETA(DisplayName = "흐림"),
	Rainy		UMETA(DisplayName = "비"),
	Snowy		UMETA(DisplayName = "눈"),
	Foggy		UMETA(DisplayName = "안개"),
	Stormy		UMETA(DisplayName = "폭풍")
};

USTRUCT(BlueprintType)
struct FWeatherTransitionInfo
{
	GENERATED_BODY()

	/** 전환에 걸리는 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	float TransitionDuration = 5.0f;

	/** 현재 전환 진행도 (0.0 ~ 1.0) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather")
	float TransitionProgress = 0.0f;

	/** 전환 중인지 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather")
	bool bIsTransitioning = false;

	/** 이전 날씨 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather")
	EWeatherType PreviousWeather = EWeatherType::Clear;

	/** 목표 날씨 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather")
	EWeatherType TargetWeather = EWeatherType::Clear;
};
