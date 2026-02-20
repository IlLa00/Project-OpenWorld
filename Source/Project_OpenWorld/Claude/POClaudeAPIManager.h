#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POClaudeTypes.h"
#include "POClaudeAPIManager.generated.h"

class APOWeatherSystemManager;
class APOTimeOfDayManager;
class APORVTManager;

UCLASS()
class PROJECT_OPENWORLD_API APOClaudeAPIManager : public AActor
{
	GENERATED_BODY()

public:
	APOClaudeAPIManager();

protected:
	virtual void BeginPlay() override;

public:
	// 사용할 Claude 모델 ID 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Claude|Config")
	FString ModelID = TEXT("claude-haiku-4-5");

	// 응답 최대 토큰 수 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Claude|Config",meta = (ClampMin = "50", ClampMax = "500"))
	int32 MaxTokens = 200;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Claude|State")
	bool bRequestInProgress = false;

	UFUNCTION(BlueprintCallable, Category = "Claude")
	void SendMessageToClaude(const FClaudeRequestContext& Context,const FOnClaudeResponse& ResponseCallback);

	UFUNCTION(BlueprintCallable, Category = "Claude")
	void SendMessageWithAutoContext(const FString& PlayerMessage,const FString& NPCName,const FString& NPCPersonality,const FOnClaudeResponse& ResponseCallback);

private:
	FString APIKey;

	void LoadAPIKeyFromConfig();

	// 시스템 프롬프트 조립 (날씨/시간 필수 반영) 
	FString BuildSystemPrompt(const FClaudeRequestContext& Context) const;

	// Claude API 요청 JSON 조립 
	FString BuildRequestJson(const FClaudeRequestContext& Context) const;

	// HTTP 응답에서 content[0].text 추출 
	FString ParseClaudeResponse(const FString& ResponseBody) const;

	FString TimeOfDayToKorean(float TimeOfDay) const;

	APOWeatherSystemManager* FindWeatherManager() const;
	APOTimeOfDayManager* FindTimeOfDayManager() const;
	APORVTManager* FindRVTManager() const;
};
