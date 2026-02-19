#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "POWorldPartitionSubsystem.generated.h"

UCLASS()
class PROJECT_OPENWORLD_API UPOWorldPartitionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

public:
	// 기본 스트리밍 로딩 반경 (cm) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Partition|Streaming")
	float DefaultStreamingRadius = 25600.0f; // 256m

	// 빠른 이동 시 확장되는 스트리밍 반경 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Partition|Streaming")
	float HighSpeedStreamingRadius = 51200.0f; // 512m

	// 빠른 이동으로 판단하는 속도 임계값 (cm/s) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Partition|Streaming")
	float HighSpeedThreshold = 1200.0f; // 약 43km/h

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Partition|Debug")
	bool bShowDebugInfo = false;

	// 현재 스트리밍 셀 수 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Partition|Debug")
	int32 LoadedCellCount = 0;

	// 현재 플레이어 위치의 스트리밍 상태 확인 
	UFUNCTION(BlueprintPure, Category = "World Partition")
	bool IsLocationLoaded(FVector Location) const;

	// 특정 위치 강제 로딩 요청 
	UFUNCTION(BlueprintCallable, Category = "World Partition")
	void RequestLoadingAtLocation(FVector Location, float Radius);

	UFUNCTION(BlueprintCallable, Category = "World Partition")
	void ToggleDebugInfo() { bShowDebugInfo = !bShowDebugInfo; }

protected:
	// 플레이어 속도 기반 스트리밍 반경 동적 조절 
	void UpdateStreamingRadius();

	void DrawDebugInfo();

private:
	// 이전 프레임 플레이어 위치 (속도 계산용) 
	FVector PreviousPlayerLocation = FVector::ZeroVector;

	// 현재 플레이어 이동 속도 
	float CurrentPlayerSpeed = 0.0f;
};
