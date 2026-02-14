#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "POGameModeBase.generated.h"

class UPOWeatherHUDWidget;

UCLASS()
class PROJECT_OPENWORLD_API APOGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	APOGameModeBase();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UPOWeatherHUDWidget> WeatherHUDWidgetClass;

protected:
	UPROPERTY()
	TObjectPtr<UPOWeatherHUDWidget> WeatherHUDWidgetInstance;

	void CreateHUDWidget();
};
