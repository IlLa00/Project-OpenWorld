#include "POWorldPartitionSubsystem.h"
#include "WorldPartition/WorldPartition.h"
#include "WorldPartition/WorldPartitionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

void UPOWorldPartitionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void UPOWorldPartitionSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UPOWorldPartitionSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 플레이어 속도 기반 스트리밍 반경 업데이트
	UpdateStreamingRadius();

	if (bShowDebugInfo)
	{
		DrawDebugInfo();
	}
}

TStatId UPOWorldPartitionSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UPOWorldPartitionSubsystem, STATGROUP_Tickables);
}

bool UPOWorldPartitionSubsystem::IsLocationLoaded(FVector Location) const
{
	UWorldPartitionSubsystem* WPSubsystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>();
	if (!WPSubsystem)
	{
		return false;
	}

	UWorldPartition* WorldPartition = GetWorld()->GetWorldPartition();
	if (!WorldPartition)
	{
		return true;
	}

	return true; 
}

void UPOWorldPartitionSubsystem::RequestLoadingAtLocation(FVector Location, float Radius)
{
	UWorldPartition* WorldPartition = GetWorld()->GetWorldPartition();
	if (!WorldPartition)
		return;

	UE_LOG(LogTemp, Log, TEXT("[WorldPartition] Requesting load at: %s, Radius: %.0f"),
		*Location.ToString(), Radius);
}

void UPOWorldPartitionSubsystem::UpdateStreamingRadius()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->GetPawn())
		return;

	FVector CurrentLocation = PC->GetPawn()->GetActorLocation();

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	if (DeltaTime > 0.0f)
	{
		CurrentPlayerSpeed = FVector::Dist(CurrentLocation, PreviousPlayerLocation) / DeltaTime;
	}

	PreviousPlayerLocation = CurrentLocation;
}

void UPOWorldPartitionSubsystem::DrawDebugInfo()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->GetPawn())
	{
		return;
	}

	FVector PlayerLocation = PC->GetPawn()->GetActorLocation();

	// 스트리밍 반경 시각화
	float ActiveRadius = (CurrentPlayerSpeed > HighSpeedThreshold)
		? HighSpeedStreamingRadius
		: DefaultStreamingRadius;

	DrawDebugSphere(
		GetWorld(),
		PlayerLocation,
		ActiveRadius,
		32,
		CurrentPlayerSpeed > HighSpeedThreshold ? FColor::Yellow : FColor::Green,
		false,
		-1.0f,
		0,
		50.0f 
	);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			10, 0.0f, FColor::Cyan,
			FString::Printf(TEXT("[World Partition]\nSpeed: %.0f cm/s\nStreaming Radius: %.0f m\nLoaded Cells: %d"),
				CurrentPlayerSpeed,
				ActiveRadius / 100.0f,
				LoadedCellCount)
		);
	}
}
