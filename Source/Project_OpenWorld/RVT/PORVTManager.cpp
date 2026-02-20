#include "PORVTManager.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

const FName APORVTManager::ParamName_Wetness        = TEXT("Wetness");
const FName APORVTManager::ParamName_SnowCoverage   = TEXT("SnowCoverage");
const FName APORVTManager::ParamName_RainIntensity  = TEXT("RainIntensity");
const FName APORVTManager::ParamName_FogDensity     = TEXT("FogDensity");
const FName APORVTManager::ParamName_WindStrength   = TEXT("WindStrength");
const FName APORVTManager::ParamName_WindDirection  = TEXT("WindDirection");
const FName APORVTManager::ParamName_TimeOfDay      = TEXT("TimeOfDay");

APORVTManager::APORVTManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.0f; // 매 프레임 업데이트
}

void APORVTManager::BeginPlay()
{
	Super::BeginPlay();

	// MPC 인스턴스 캐싱 (GetMPCInstance는 내부적으로 캐싱하므로 성능 안전)
	if (GlobalWeatherMPC)
	{
		MPCInstance = GetWorld()->GetParameterCollectionInstance(GlobalWeatherMPC);
		if (MPCInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("[PORVTManager] MPC 인스턴스 캐싱 완료: %s"), *GlobalWeatherMPC->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[PORVTManager] MPC 인스턴스를 가져올 수 없음. MPC 에셋을 확인하세요."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PORVTManager] GlobalWeatherMPC가 할당되지 않았습니다. 에디터에서 MPC를 할당하세요."));
	}

	// 초기 파라미터 적용
	FlushMPCParameters();
}

void APORVTManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 파라미터 부드럽게 보간
	InterpolateParameters(DeltaTime);

	// MPC에 현재 파라미터 업데이트
	UpdateMPCParameters(DeltaTime);

	if (bShowDebugInfo)
	{
		DrawDebugInfo();
	}
}

void APORVTManager::SetWeatherParameters(float InWetness, float InSnowCoverage, float InRainIntensity,
	float InFogDensity, float InWindStrength, float InWindDirection)
{
	Wetness       = FMath::Clamp(InWetness, 0.0f, 1.0f);
	SnowCoverage  = FMath::Clamp(InSnowCoverage, 0.0f, 1.0f);
	RainIntensity = FMath::Clamp(InRainIntensity, 0.0f, 1.0f);
	FogDensity    = FMath::Clamp(InFogDensity, 0.0f, 1.0f);
	WindStrength  = FMath::Clamp(InWindStrength, 0.0f, 1.0f);
	WindDirection = FMath::Clamp(InWindDirection, 0.0f, 360.0f);

	TargetWetness       = Wetness;
	TargetSnowCoverage  = SnowCoverage;
	TargetRainIntensity = RainIntensity;
	TargetWindStrength  = WindStrength;

	FlushMPCParameters();
}

void APORVTManager::SetWeatherParametersTarget(float InWetness, float InSnowCoverage, float InRainIntensity,
	float InFogDensity, float InWindStrength, float InWindDirection)
{
	TargetWetness       = FMath::Clamp(InWetness, 0.0f, 1.0f);
	TargetSnowCoverage  = FMath::Clamp(InSnowCoverage, 0.0f, 1.0f);
	TargetRainIntensity = FMath::Clamp(InRainIntensity, 0.0f, 1.0f);
	FogDensity          = FMath::Clamp(InFogDensity, 0.0f, 1.0f);
	TargetWindStrength  = FMath::Clamp(InWindStrength, 0.0f, 1.0f);
	WindDirection       = FMath::Clamp(InWindDirection, 0.0f, 360.0f);
}

void APORVTManager::UpdateTimeOfDay(float InTimeOfDay)
{
	TimeOfDay = FMath::Clamp(InTimeOfDay, 0.0f, 24.0f);

	if (MPCInstance)
	{
		MPCInstance->SetScalarParameterValue(ParamName_TimeOfDay, TimeOfDay);
	}
}

void APORVTManager::FlushMPCParameters()
{
	if (!MPCInstance)
	{
		if (GlobalWeatherMPC && GetWorld())
		{
			MPCInstance = GetWorld()->GetParameterCollectionInstance(GlobalWeatherMPC);
		}
	}

	if (!MPCInstance)
	{
		return;
	}

	MPCInstance->SetScalarParameterValue(ParamName_Wetness,       Wetness);
	MPCInstance->SetScalarParameterValue(ParamName_SnowCoverage,  SnowCoverage);
	MPCInstance->SetScalarParameterValue(ParamName_RainIntensity, RainIntensity);
	MPCInstance->SetScalarParameterValue(ParamName_FogDensity,    FogDensity);
	MPCInstance->SetScalarParameterValue(ParamName_WindStrength,  WindStrength);
	MPCInstance->SetScalarParameterValue(ParamName_WindDirection, WindDirection);
	MPCInstance->SetScalarParameterValue(ParamName_TimeOfDay,     TimeOfDay);
}

void APORVTManager::InterpolateParameters(float DeltaTime)
{
	float Alpha = FMath::Clamp(InterpolationSpeed * DeltaTime, 0.0f, 1.0f);

	Wetness       = FMath::Lerp(Wetness,       TargetWetness,       Alpha);
	SnowCoverage  = FMath::Lerp(SnowCoverage,  TargetSnowCoverage,  Alpha);
	RainIntensity = FMath::Lerp(RainIntensity, TargetRainIntensity, Alpha);
	WindStrength  = FMath::Lerp(WindStrength,  TargetWindStrength,  Alpha);
}

void APORVTManager::UpdateMPCParameters(float DeltaTime)
{
	if (!MPCInstance)
	{
		return;
	}

	MPCInstance->SetScalarParameterValue(ParamName_Wetness,       Wetness);
	MPCInstance->SetScalarParameterValue(ParamName_SnowCoverage,  SnowCoverage);
	MPCInstance->SetScalarParameterValue(ParamName_RainIntensity, RainIntensity);
	MPCInstance->SetScalarParameterValue(ParamName_FogDensity,    FogDensity);
	MPCInstance->SetScalarParameterValue(ParamName_WindStrength,  WindStrength);
	MPCInstance->SetScalarParameterValue(ParamName_WindDirection, WindDirection);
}

void APORVTManager::ToggleDebugInfo()
{
	bShowDebugInfo = !bShowDebugInfo;
}

void APORVTManager::DrawDebugInfo()
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(
		20, 0.0f, FColor::Green,
		FString::Printf(TEXT("[RVT Manager]\n")
			TEXT("Wetness:  %.2f → %.2f\n")
			TEXT("Snow:     %.2f → %.2f\n")
			TEXT("Rain:     %.2f → %.2f\n")
			TEXT("Fog:      %.2f\n")
			TEXT("Wind:     %.2f (%.0f°)\n")
			TEXT("Time:     %.1fh"),
			Wetness,       TargetWetness,
			SnowCoverage,  TargetSnowCoverage,
			RainIntensity, TargetRainIntensity,
			FogDensity,
			WindStrength,  WindDirection,
			TimeOfDay)
	);
}
