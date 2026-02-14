// Copyright Epic Games, Inc. All Rights Reserved.

#include "POWeatherHUDWidget.h"
#include "../TimeOfDay/POTimeOfDayManager.h"
#include "../Weather/POWeatherSystemManager.h"
#include "../Weather/WeatherTypes.h"
#include "Kismet/GameplayStatics.h"

void UPOWeatherHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FindManagers();
}

void UPOWeatherHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!TimeOfDayManager || !WeatherSystemManager)
	{
		FindManagers();
	}
}

void UPOWeatherHUDWidget::FindManagers()
{
	if (!TimeOfDayManager)
	{
		TimeOfDayManager = Cast<APOTimeOfDayManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), APOTimeOfDayManager::StaticClass())
		);
	}

	if (!WeatherSystemManager)
	{
		WeatherSystemManager = Cast<APOWeatherSystemManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), APOWeatherSystemManager::StaticClass())
		);
	}
}

float UPOWeatherHUDWidget::GetCurrentTime() const
{
	if (TimeOfDayManager)
	{
		return TimeOfDayManager->GetCurrentTime();
	}
	return 12.0f; // 기본값: 정오
}

FText UPOWeatherHUDWidget::GetFormattedTimeText() const
{
	if (!TimeOfDayManager)
	{
		return FText::FromString(TEXT("--:--"));
	}

	int32 Hours, Minutes;
	TimeOfDayManager->GetFormattedTime(Hours, Minutes);

	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Hours, Minutes));
}

FText UPOWeatherHUDWidget::GetTimeOfDayText() const
{
	float Time = GetCurrentTime();

	if (Time >= 5.0f && Time < 8.0f)
	{
		return FText::FromString(TEXT("새벽"));
	}
	else if (Time >= 8.0f && Time < 12.0f)
	{
		return FText::FromString(TEXT("아침"));
	}
	else if (Time >= 12.0f && Time < 17.0f)
	{
		return FText::FromString(TEXT("오후"));
	}
	else if (Time >= 17.0f && Time < 20.0f)
	{
		return FText::FromString(TEXT("저녁"));
	}
	else
	{
		return FText::FromString(TEXT("밤"));
	}
}

bool UPOWeatherHUDWidget::IsDaytime() const
{
	if (TimeOfDayManager)
	{
		return TimeOfDayManager->IsDaytime();
	}
	return true;
}

FText UPOWeatherHUDWidget::GetCurrentWeatherText() const
{
	if (!WeatherSystemManager)
	{
		return FText::FromString(TEXT("날씨 정보 없음"));
	}

	EWeatherType CurrentWeather = WeatherSystemManager->GetCurrentWeather();
	return WeatherTypeToText(CurrentWeather);
}

bool UPOWeatherHUDWidget::IsWeatherTransitioning() const
{
	if (WeatherSystemManager)
	{
		return WeatherSystemManager->IsTransitioning();
	}
	return false;
}

float UPOWeatherHUDWidget::GetWeatherTransitionProgress() const
{
	if (WeatherSystemManager && WeatherSystemManager->IsTransitioning())
	{
		return WeatherSystemManager->TransitionInfo.TransitionProgress;
	}
	return 0.0f;
}

FLinearColor UPOWeatherHUDWidget::GetWeatherColor() const
{
	if (!WeatherSystemManager)
	{
		return FLinearColor::White;
	}

	EWeatherType CurrentWeather = WeatherSystemManager->GetCurrentWeather();

	switch (CurrentWeather)
	{
	case EWeatherType::Clear:
		return FLinearColor(1.0f, 1.0f, 0.3f); // 노란색
	case EWeatherType::Cloudy:
		return FLinearColor(0.7f, 0.7f, 0.7f); // 회색
	case EWeatherType::Rainy:
		return FLinearColor(0.3f, 0.5f, 1.0f); // 파란색
	case EWeatherType::Snowy:
		return FLinearColor(1.0f, 1.0f, 1.0f); // 흰색
	case EWeatherType::Foggy:
		return FLinearColor(0.8f, 0.8f, 0.8f); // 연한 회색
	case EWeatherType::Stormy:
		return FLinearColor(0.3f, 0.3f, 0.5f); // 어두운 파란색
	default:
		return FLinearColor::White;
	}
}

FText UPOWeatherHUDWidget::WeatherTypeToText(EWeatherType WeatherType) const
{
	switch (WeatherType)
	{
	case EWeatherType::Clear:
		return FText::FromString(TEXT("맑음"));
	case EWeatherType::Cloudy:
		return FText::FromString(TEXT("흐림"));
	case EWeatherType::Rainy:
		return FText::FromString(TEXT("비"));
	case EWeatherType::Snowy:
		return FText::FromString(TEXT("눈"));
	case EWeatherType::Foggy:
		return FText::FromString(TEXT("안개"));
	case EWeatherType::Stormy:
		return FText::FromString(TEXT("폭풍"));
	default:
		return FText::FromString(TEXT("알 수 없음"));
	}
}
