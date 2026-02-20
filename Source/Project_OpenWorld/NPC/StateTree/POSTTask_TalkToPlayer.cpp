#include "POSTTask_TalkToPlayer.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "../PONPCCharacter.h"
#include "../PONPCTypes.h"

APONPCCharacter* UPOSTTask_TalkToPlayer::GetNPCCharacter(FStateTreeExecutionContext& Context) const
{
	UObject* OwnerObj = Context.GetOwner();
	if (!OwnerObj) return nullptr;
	AAIController* AIC = Cast<AAIController>(OwnerObj);
	if (!AIC) return nullptr;
	return Cast<APONPCCharacter>(AIC->GetPawn());
}

EStateTreeRunStatus UPOSTTask_TalkToPlayer::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	ElapsedTime = 0.0f;

	APONPCCharacter* NPC = GetNPCCharacter(Context);
	if (!NPC)
	{
		return EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UPOSTTask_TalkToPlayer::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	ElapsedTime += DeltaTime;

	APONPCCharacter* NPC = GetNPCCharacter(Context);
	if (!NPC)
	{
		return EStateTreeRunStatus::Failed;
	}

	switch (NPC->TalkState)
	{
	case ENPCTalkState::WaitingForAPI:
		return EStateTreeRunStatus::Running;

	case ENPCTalkState::Talking:
		return EStateTreeRunStatus::Running;

	case ENPCTalkState::Cooldown:
		return EStateTreeRunStatus::Running;

	case ENPCTalkState::Idle:
	default:
		return EStateTreeRunStatus::Succeeded;
	}
}

void UPOSTTask_TalkToPlayer::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	APONPCCharacter* NPC = GetNPCCharacter(Context);
	if (!NPC) return;

	if (NPC->TalkState != ENPCTalkState::Idle)
	{
		NPC->EndConversation();
	}
}
