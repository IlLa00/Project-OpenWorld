#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PONPCAIController.generated.h"

class UStateTreeComponent;

UCLASS()
class PROJECT_OPENWORLD_API APONPCAIController : public AAIController
{
	GENERATED_BODY()

public:
	APONPCAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|StateTree")
	TObjectPtr<UStateTreeComponent> StateTreeComponent;
};
