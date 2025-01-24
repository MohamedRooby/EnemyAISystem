// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MeleeAttack.h"

#include "AIController.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = TEXT("Melee Attack");
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// if we are out of range don't attack the player
	auto const OutOfRange = !OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
	if (OutOfRange)
	{
		// finish the task
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	//we are in range so we get the controller and the NPC
	auto const * const Cont = OwnerComp.GetAIOwner();
	auto* const NPC = Cast<ANPC>(Cont->GetPawn());

	//IF THE NPC supports ICombatInterface , cast and call the execute melee attack function
	if (auto* const ICombat = Cast<ICombatInterface>(NPC))
	{
		//necessary check to see if the montage has finished ao we don't try and play it again
		if (MontageHasFinished(NPC))
		{
			ICombat->Execute_MeleeAttack(NPC);
		}
	}

	//finish the task
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Type();
	

		
	
}

bool UBTTask_MeleeAttack::MontageHasFinished(ANPC* const NPC)
{
	return NPC->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(NPC->GetCurrentMontage());
}
