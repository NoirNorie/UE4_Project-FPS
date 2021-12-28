// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "TPlayer.h"

#include "Zombie.h"
#include "ZombieAIController.h"

#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"


#include "BT_Task_TurnToTarget.generated.h"

/**
 * 
 */
UCLASS()
class PP_API UBT_Task_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
