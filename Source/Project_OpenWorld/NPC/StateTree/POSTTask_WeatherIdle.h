#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "POSTTask_WeatherIdle.generated.h"

class APONPCCharacter;
class UAnimMontage;

UCLASS(Blueprintable, meta = (DisplayName = "NPC Weather Idle Task"))
class PROJECT_OPENWORLD_API UPOSTTask_WeatherIdle : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "NPC|Animation")
	TObjectPtr<UAnimMontage> MontageToPlay = nullptr;

	UPROPERTY(EditAnywhere, Category = "NPC|Animation", meta = (ClampMin = "0.1", ClampMax = "3.0"))
	float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "NPC|Animation")
	FName LoopSectionName = NAME_None;

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
		const float DeltaTime) override;

	virtual void ExitState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;

private:
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentPlayingMontage = nullptr;

	APONPCCharacter* GetNPCCharacter(FStateTreeExecutionContext& Context) const;
};
