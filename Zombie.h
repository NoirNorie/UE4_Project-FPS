// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// 생성자 보조함수 헤더
#include "UObject/ConstructorHelpers.h"

// 캡슐 컴포넌트 헤더
#include "Components/CapsuleComponent.h"
// 정적 클래스 헤더
#include "UObject/Class.h"

// 좀비용으로 만든 컨트롤러 헤더
#include "ZombieAIController.h"

// 좀비 애니메이션 헤더
#include "ZombieAnim.h"

#include "Zombie.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate); // 공격 종료 멀티캐스트 델리게이트 선언
DECLARE_MULTICAST_DELEGATE(FOnDetectEndDelegate); // 추적 종료 멀티캐스트 델리게이트 선언

UCLASS()
class PP_API AZombie : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZombie();
	

	//void ReceivePointDamage(float Damage, const UDamageType* DamageType, FVector HitLocation, FVector HitNormal, UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, AController* InstigatedBy, AActor* DamageCauser, const FHitResult& HitInfo);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	UFUNCTION() void Attack();
	UFUNCTION() void Detect();	
	UFUNCTION() void Death();
	
	
	FOnAttackEndDelegate OnAttackEnd; // 공격이 종료되었다는 델리게이트
	FOnDetectEndDelegate OnDetectEnd; // 추적이 종료되었다는 델리게이트

	

	// -- 변수--
	UPROPERTY() bool IsAttacking;
	UPROPERTY() bool IsDetect;

	UPROPERTY() float Zombie_HP;
	UPROPERTY() bool IsDeath;


	// 애니메이션 인스턴스 변수
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	class UZombieAnim* ZombieAnimInst;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 애니메이션 출력용 함수
	virtual void PostInitializeComponents() override;

	//void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION() void OnAttackEnded();
	UFUNCTION() void OnDetectEnded();
};
