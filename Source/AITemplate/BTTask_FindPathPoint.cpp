// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPathPoint.h"

#include "NPC.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindPathPoint::UBTTask_FindPathPoint(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName =  TEXT("Find Path Point");
	
}

EBTNodeResult::Type UBTTask_FindPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//attempt to get the NPC's Controller
	if (auto* const Cont = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		//attempt to get the blackboard component from the behavioral tree
		if (auto* const BC = OwnerComp.GetBlackboardComponent())
		{
			// get the current patrol path index from the blackboard
			auto const Index = BC->GetValueAsInt(GetSelectedBlackboardKey());

			//get the NPC
			if (auto* NPC = Cast<ANPC>(Cont->GetPawn()))
			{
				//get the current patrol path vector from the NPC - This is local patrol path to actor
				auto const Point = NPC->GetPatrolPath()->GetPatrolPoints(Index);

				// convert the local vector to a global point
				auto const GlobalPoint = NPC->GetPatrolPath()->GetActorTransform().TransformPosition(Point);
				BC->SetValueAsVector(PatrolPathVectorKey.SelectedKeyName, GlobalPoint );

				//Finish with success
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
			
		}
	}
	
	return EBTNodeResult::Failed;
}
