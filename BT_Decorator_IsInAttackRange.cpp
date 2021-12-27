// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Decorator_IsInAttackRange.h"

UBT_Decorator_IsInAttackRange::UBT_Decorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBT_Decorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return false;
	}

	ATPlayer* Target = Cast<ATPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AZombieAIController::TargetPosKey));
	if (Target == nullptr)
	{
		return false;
	}

	bool bResult = (Target->GetDistanceTo(ControllingPawn) <= 200.0f);
	return bResult;
}