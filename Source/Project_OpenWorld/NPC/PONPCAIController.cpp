#include "PONPCAIController.h"
#include "Components/StateTreeComponent.h"

APONPCAIController::APONPCAIController()
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
}

void APONPCAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (StateTreeComponent)
	{
		StateTreeComponent->StartLogic();
		UE_LOG(LogTemp, Log, TEXT("[NPCAIController] StateTree 로직 시작 - Pawn: %s"), *GetNameSafe(InPawn));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCAIController] StateTreeComponent가 없습니다."));
	}
}

void APONPCAIController::OnUnPossess()
{
	if (StateTreeComponent)
	{
		StateTreeComponent->StopLogic(TEXT("Controller unpossessed"));
		UE_LOG(LogTemp, Log, TEXT("[NPCAIController] StateTree 로직 중지"));
	}

	Super::OnUnPossess();
}
