#pragma once

#include "CoreMinimal.h"
#include "PONPCTypes.generated.h"

UENUM(BlueprintType)
enum class ENPCTalkState : uint8
{
	Idle          UMETA(DisplayName = "대기"),
	WaitingForAPI UMETA(DisplayName = "API 응답 대기"),
	Talking       UMETA(DisplayName = "대화 중"),
	Cooldown      UMETA(DisplayName = "쿨다운")
};

USTRUCT(BlueprintType)
struct FNPCDialogueEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "NPC|Dialogue")
	FString PlayerMessage;

	UPROPERTY(BlueprintReadOnly, Category = "NPC|Dialogue")
	FString NPCResponse;

	UPROPERTY(BlueprintReadOnly, Category = "NPC|Dialogue")
	FString WeatherContext;
};
