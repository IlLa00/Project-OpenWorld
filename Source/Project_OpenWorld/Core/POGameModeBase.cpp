#include "POGameModeBase.h"
#include "../UI/POWeatherHUDWidget.h"
#include "Blueprint/UserWidget.h"

APOGameModeBase::APOGameModeBase()
{

}

void APOGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CreateHUDWidget();
}

void APOGameModeBase::CreateHUDWidget()
{
	if (!WeatherHUDWidgetClass)
		return;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
		return;

	WeatherHUDWidgetInstance = CreateWidget<UPOWeatherHUDWidget>(PlayerController, WeatherHUDWidgetClass);
	if (WeatherHUDWidgetInstance)
	{
		WeatherHUDWidgetInstance->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] Failed to create Weather HUD Widget!"));
	}
}
