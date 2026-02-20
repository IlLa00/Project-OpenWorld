#include "POSTTask_WeatherIdle.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "../PONPCCharacter.h"

APONPCCharacter* UPOSTTask_WeatherIdle::GetNPCCharacter(FStateTreeExecutionContext& Context) const
{
	UObject* OwnerObj = Context.GetOwner();
	if (!OwnerObj) return nullptr;
	AAIController* AIC = Cast<AAIController>(OwnerObj);
	if (!AIC) return nullptr;
	return Cast<APONPCCharacter>(AIC->GetPawn());
}

EStateTreeRunStatus UPOSTTask_WeatherIdle::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	CurrentPlayingMontage = nullptr;

	APONPCCharacter* NPC = GetNPCCharacter(Context);
	if (!NPC || !MontageToPlay)
	{
		return EStateTreeRunStatus::Running;
	}

	UAnimInstance* AnimInstance = NPC->GetMesh() ? NPC->GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, 0.0f);

		if (LoopSectionName != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(LoopSectionName, MontageToPlay);
		}

		CurrentPlayingMontage = MontageToPlay;
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UPOSTTask_WeatherIdle::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime)
{
	return EStateTreeRunStatus::Running;
}

void UPOSTTask_WeatherIdle::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	if (!CurrentPlayingMontage) return;

	APONPCCharacter* NPC = GetNPCCharacter(Context);
	if (!NPC) return;

	UAnimInstance* AnimInstance = NPC->GetMesh() ? NPC->GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, CurrentPlayingMontage);
	}

	CurrentPlayingMontage = nullptr;
}
