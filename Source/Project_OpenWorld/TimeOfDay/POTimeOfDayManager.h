#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeOfDayTypes.h"
#include "POTimeOfDayManager.generated.h"

class UDirectionalLightComponent;
class UExponentialHeightFogComponent;
class USkyAtmosphereComponent;
class UMaterialParameterCollection;

UCLASS()
class PROJECT_OPENWORLD_API APOTimeOfDayManager : public AActor
{
	GENERATED_BODY()

public:
	APOTimeOfDayManager();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	// 현재 시간 (0.0 ~ 24.0, 12.0 = 정오) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Current", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float CurrentTime = 12.0f;

	// 시간 흐름 속도 (1.0 = 실시간, 60.0 = 1분에 1시간) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Speed")
	float TimeSpeed = 1.0f;

	// 시간이 자동으로 흐를지 여부 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Speed")
	bool bAutoProgress = false;

	// 24시간이 지나면 0시로 리셋할지 여부 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Speed")
	bool bLoopTime = true;

	// 태양 (Directional Light) 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time of Day|Components")
	TObjectPtr<UDirectionalLightComponent> Sun;

	// 대기 효과 (Sky Atmosphere) 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time of Day|Components")
	TObjectPtr<USkyAtmosphereComponent> SkyAtmosphere;

	// 높이 안개 (Exponential Height Fog) 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time of Day|Components")
	TObjectPtr<UExponentialHeightFogComponent> HeightFog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|References")
	TObjectPtr<UMaterialParameterCollection> TimeOfDayMPC;

	// 태양 최대 강도 (정오) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Sun")
	float MaxSunIntensity = 10.0f;

	// 태양 최소 강도 (밤) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Sun")
	float MinSunIntensity = 0.1f;

	// 특정 시간으로 즉시 설정 
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	void SetTimeOfDay(float NewTime);

	// 특정 시간으로 부드럽게 전환 
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	void TransitionToTime(float TargetTime, float Duration = 5.0f);

	// 프리셋 시간으로 설정 
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	void SetTimePreset(ETimeOfDayPreset Preset);

	// 현재 시간 가져오기 
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	float GetCurrentTime() const { return CurrentTime; }

	// 현재가 낮인지 밤인지 확인 
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	bool IsDaytime() const { return CurrentTime >= 6.0f && CurrentTime < 18.0f; }

	// 시간을 시:분 형식으로 가져오기 
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	void GetFormattedTime(int32& OutHours, int32& OutMinutes) const;

protected:
	void UpdateTime(float DeltaTime);
	void UpdateSkySystem();

	void UpdateMaterialParameters();

	FRotator CalculateSunRotation(float TimeOfDay) const;
	float CalculateSunIntensity(float TimeOfDay) const;
	FLinearColor CalculateSunColor(float TimeOfDay) const;

private:
	// 전환 중인지 여부 
	bool bIsTransitioning = false;

	// 전환 시작 시간 
	float TransitionStartValue = 0.0f;

	// 전환 목표 시간 
	float TransitionTargetValue = 0.0f;

	// 전환 진행도 (0.0 ~ 1.0) 
	float TransitionProgress = 0.0f;

	// 전환 지속 시간 
	float TransitionDuration = 5.0f;

	// 전환 시작 타임스탬프 
	float TransitionStartTime = 0.0f;
};
