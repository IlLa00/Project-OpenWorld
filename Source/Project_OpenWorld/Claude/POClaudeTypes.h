#pragma once

#include "CoreMinimal.h"
#include "POClaudeTypes.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnClaudeResponse,bool, bSuccess,const FString&, ResponseText);

USTRUCT(BlueprintType)
struct FClaudeRequestContext
{
	GENERATED_BODY()

	/** 플레이어가 입력한 메시지 */
	UPROPERTY(BlueprintReadWrite, Category = "Claude")
	FString PlayerMessage;

	/** NPC 이름 (시스템 프롬프트에 주입) */
	UPROPERTY(BlueprintReadWrite, Category = "Claude")
	FString NPCName = TEXT("마을 주민");

	/** NPC 성격 설명 (시스템 프롬프트에 주입) */
	UPROPERTY(BlueprintReadWrite, Category = "Claude")
	FString NPCPersonality = TEXT("친절하고 소박한 시골 마을 주민");

	/** 현재 날씨 한국어 문자열 ("맑음", "비", "눈" 등) */
	UPROPERTY(BlueprintReadWrite, Category = "Claude")
	FString WeatherType = TEXT("맑음");

	/** 현재 시간 (0.0 ~ 24.0) */
	UPROPERTY(BlueprintReadWrite, Category = "Claude")
	float TimeOfDay = 12.0f;

	/** 비 강도 (0.0 ~ 1.0, RVTManager에서 수집) */
	UPROPERTY(BlueprintReadWrite, Category = "Claude")
	float RainIntensity = 0.0f;

	/** 눈 덮임 정도 (0.0 ~ 1.0) */
	UPROPERTY(BlueprintReadWrite, Category = "Claude")
	float SnowCoverage = 0.0f;

	/** 바람 강도 (0.0 ~ 1.0) */
	UPROPERTY(BlueprintReadWrite, Category = "Claude")
	float WindStrength = 0.0f;
};
