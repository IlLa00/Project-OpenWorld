#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "POSTTask_TalkToPlayer.generated.h"

class APONPCCharacter;

UCLASS(Blueprintable, meta = (DisplayName = "NPC Talk To Player Task"))
class PROJECT_OPENWORLD_API UPOSTTask_TalkToPlayer : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "NPC|Talk", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float MinDisplayTime = 3.0f;

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
		const float DeltaTime) override;

	virtual void ExitState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;

private:
	UPROPERTY()
	float ElapsedTime = 0.0f;

	APONPCCharacter* GetNPCCharacter(FStateTreeExecutionContext& Context) const;
};
