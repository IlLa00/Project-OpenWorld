#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "POWeatherHUDWidget.generated.h"

class APOTimeOfDayManager;
class APOWeatherSystemManager;
enum class EWeatherType : uint8;

UCLASS()
class PROJECT_OPENWORLD_API UPOWeatherHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Weather HUD")
	TObjectPtr<APOTimeOfDayManager> TimeOfDayManager;

	UPROPERTY(BlueprintReadOnly, Category = "Weather HUD")
	TObjectPtr<APOWeatherSystemManager> WeatherSystemManager;

public:
	UFUNCTION(BlueprintPure, Category = "Weather HUD|Time")
	float GetCurrentTime() const;

	UFUNCTION(BlueprintPure, Category = "Weather HUD|Time")
	FText GetFormattedTimeText() const;

	UFUNCTION(BlueprintPure, Category = "Weather HUD|Time")
	FText GetTimeOfDayText() const;

	UFUNCTION(BlueprintPure, Category = "Weather HUD|Time")
	bool IsDaytime() const;

	UFUNCTION(BlueprintPure, Category = "Weather HUD|Weather")
	FText GetCurrentWeatherText() const;

	UFUNCTION(BlueprintPure, Category = "Weather HUD|Weather")
	bool IsWeatherTransitioning() const;

	UFUNCTION(BlueprintPure, Category = "Weather HUD|Weather")
	float GetWeatherTransitionProgress() const;

	UFUNCTION(BlueprintPure, Category = "Weather HUD|Weather")
	FLinearColor GetWeatherColor() const;

private:
	void FindManagers();
	FText WeatherTypeToText(EWeatherType WeatherType) const;
};
