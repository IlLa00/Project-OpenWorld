#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "PORVTManager.generated.h"

class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;

UCLASS(BlueprintType, Blueprintable)
class PROJECT_OPENWORLD_API APORVTManager : public AActor
{
	GENERATED_BODY()

public:
	APORVTManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVT|MPC")
	TObjectPtr<UMaterialParameterCollection> GlobalWeatherMPC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVT|Weather Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Wetness = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVT|Weather Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SnowCoverage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVT|Weather Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RainIntensity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVT|Weather Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FogDensity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVT|Weather Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WindStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVT|Weather Parameters", meta = (ClampMin = "0.0", ClampMax = "360.0"))
	float WindDirection = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVT|Weather Parameters", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float TimeOfDay = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVT|Transition")
	float InterpolationSpeed = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RVT|Transition")
	float TargetWetness = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RVT|Transition")
	float TargetSnowCoverage = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RVT|Transition")
	float TargetRainIntensity = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RVT|Transition")
	float TargetWindStrength = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "RVT|Control")
	void SetWeatherParameters(float InWetness, float InSnowCoverage, float InRainIntensity,
		float InFogDensity, float InWindStrength, float InWindDirection);

	UFUNCTION(BlueprintCallable, Category = "RVT|Control")
	void SetWeatherParametersTarget(float InWetness, float InSnowCoverage, float InRainIntensity,
		float InFogDensity, float InWindStrength, float InWindDirection);

	UFUNCTION(BlueprintCallable, Category = "RVT|Control")
	void UpdateTimeOfDay(float InTimeOfDay);

	UFUNCTION(BlueprintCallable, Category = "RVT|Control")
	void FlushMPCParameters();

	UFUNCTION(BlueprintCallable, Category = "RVT|Debug")
	void ToggleDebugInfo();

protected:
	void UpdateMPCParameters(float DeltaTime);

	void InterpolateParameters(float DeltaTime);
	void DrawDebugInfo();

private:
	UPROPERTY()
	TObjectPtr<UMaterialParameterCollectionInstance> MPCInstance;

	bool bShowDebugInfo = false;

	static const FName ParamName_Wetness;
	static const FName ParamName_SnowCoverage;
	static const FName ParamName_RainIntensity;
	static const FName ParamName_FogDensity;
	static const FName ParamName_WindStrength;
	static const FName ParamName_WindDirection;
	static const FName ParamName_TimeOfDay;
};
