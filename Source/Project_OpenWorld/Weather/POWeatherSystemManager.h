#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeatherTypes.h"
#include "POWeatherSystemManager.generated.h"

class UWeatherStateDataAsset;
class UMaterialParameterCollection;
class UNiagaraComponent;
class APORVTManager;

UCLASS()
class PROJECT_OPENWORLD_API APOWeatherSystemManager : public AActor
{
	GENERATED_BODY()

public:
	APOWeatherSystemManager();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	// 현재 날씨 상태 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|State")
	EWeatherType CurrentWeather = EWeatherType::Clear;

	// 날씨 전환 정보
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather|State")
	FWeatherTransitionInfo TransitionInfo;

	// 자동으로 날씨를 변경할지 여부 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Auto Change")
	bool bEnableAutoWeatherChange = false;

	// 날씨 변경 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Auto Change", meta = (EditCondition = "bEnableAutoWeatherChange"))
	float WeatherChangeInterval = 300.0f;

	// 다음 날씨 변경까지 남은 시간 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weather|Auto Change")
	float TimeUntilNextWeatherChange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|MPC")
	TObjectPtr<UMaterialParameterCollection> WeatherMPC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Data")
	TMap<EWeatherType, TObjectPtr<UWeatherStateDataAsset>> WeatherDataMap;

	/** RVT 매니저 참조 (레벨에 배치된 PORVTManager 액터, 없으면 BeginPlay에서 자동 탐색) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|RVT")
	TObjectPtr<APORVTManager> RVTManager;

	// 날씨 즉시 변경 
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetWeatherImmediate(EWeatherType NewWeather);

	// 날씨를 부드럽게 전환 
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void TransitionToWeather(EWeatherType NewWeather, float Duration = 5.0f);

	// 랜덤 날씨로 전환 
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void TransitionToRandomWeather(float Duration = 5.0f);

	// 현재 날씨 상태 가져오기 
	UFUNCTION(BlueprintPure, Category = "Weather")
	EWeatherType GetCurrentWeather() const { return CurrentWeather; }

	// 날씨 전환 중인지 확인 
	UFUNCTION(BlueprintPure, Category = "Weather")
	bool IsTransitioning() const { return TransitionInfo.bIsTransitioning; }

protected:
	// 날씨 전환 업데이트 
	void UpdateWeatherTransition(float DeltaTime);

	// MPC 파라미터 업데이트 
	void UpdateMaterialParameters();

	// 자동 날씨 변경 타이머 업데이트 
	void UpdateAutoWeatherChange(float DeltaTime);

	// 날씨 효과 적용
	void ApplyWeatherEffects(EWeatherType Weather, float Intensity);

	// 전환 완료 처리 
	void OnTransitionComplete();

private:
	// 전환 시작 시간 
	float TransitionStartTime = 0.0f;
};
