#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PONPCTypes.h"
#include "PONPCCharacter.generated.h"

class APOClaudeAPIManager;
class APOWeatherSystemManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCDialogueUpdated,const FString&, NPCResponse,bool, bIsThinking);

UCLASS()
class PROJECT_OPENWORLD_API APONPCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APONPCCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Identity")
	FString NPCName = TEXT("마을 주민");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Identity",
		meta = (MultiLine = true))
	FString NPCPersonality = TEXT("친절하고 소박한 시골 마을 주민");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|State")
	ENPCTalkState TalkState = ENPCTalkState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|State")
	FString CurrentWeatherName = TEXT("맑음");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|Dialogue")
	FString LastNPCResponse;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|Dialogue")
	TArray<FNPCDialogueEntry> DialogueHistory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NPC|Dialogue")
	float TalkCooldown = 2.0f;

	UPROPERTY(BlueprintAssignable, Category = "NPC|Events")
	FOnNPCDialogueUpdated OnDialogueUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|References")
	TObjectPtr<APOClaudeAPIManager> ClaudeManager;

	UFUNCTION(BlueprintCallable, Category = "NPC|Talk")
	void StartConversation(const FString& PlayerMessage);

	UFUNCTION(BlueprintCallable, Category = "NPC|Talk")
	void EndConversation();

	UFUNCTION(BlueprintPure, Category = "NPC|State")
	bool IsInConversation() const;

private:
	UFUNCTION()
	void OnClaudeResponseReceived(bool bSuccess, const FString& ResponseText);

	void RefreshWeatherState();

	UFUNCTION()
	void OnCooldownFinished();

	TObjectPtr<APOWeatherSystemManager> WeatherManager;
	FTimerHandle WeatherRefreshTimerHandle;
	FTimerHandle CooldownTimerHandle;

	static constexpr float WeatherRefreshInterval = 2.0f;
	static constexpr int32 MaxDialogueHistory = 10;
};
