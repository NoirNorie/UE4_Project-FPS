// Fill out your copyright notice in the Description page of Project Settings.


#include "TPlayer.h"

// Sets default values
ATPlayer::ATPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FireRate = 0; // 초기 발사 속도는 무기가 없으므로 0.0f;

	// 달리기 배속 초기화
	SprintSpeedMultiplier = 2.0f;

	// 무기 장착 상태 표시
	CheckWeapon = false;

	// 무기 이름 초기화
	WeaponName = "-";
	// 총알 초기화
	player_ammo = 0; // 총알 수
	player_mag = 0; // 탄창 수

	// 캐릭터 상태 초기화
	player_HP = 100.0f;
	player_Hungry = 0.0f;
	player_Thirsty = 0.0f;

	// 에너지, 수분 소모량 초기화
	RequireMoisture = 1.0f;
	RequireFat = 1.0f;


	// 스프링암
	TPSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSpringArm"));
	TPSpringArm->SetupAttachment(RootComponent);
	TPSpringArm->TargetArmLength = CamArmLength = 300.0f;
	CamOriginPos.X = 100.0f;
	CamOriginPos.Y = 50.0f;
	CamOriginPos.Z = 50.0f;
	TPSpringArm->bUsePawnControlRotation = true;

	// 카메라
	TFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TFollowCamera"));
	TFollowCamera->SetupAttachment(TPSpringArm, USpringArmComponent::SocketName);
	TFollowCamera->bUsePawnControlRotation = false;

	// 무기 소켓 - 손
	FName weaponSocketName = TEXT("Socket_R_Hand");
	Weapon_Socket = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon_Socket->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, weaponSocketName);

	// 콜리전 프로필
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("TPlayer"));

	static ConstructorHelpers::FObjectFinder<USoundCue>RifleEmpty(TEXT("SoundCue'/Game/Blueprint/WeaponSound/RifleEmpty_Cue.RifleEmpty_Cue'"));
	if (RifleEmpty.Succeeded())
	{
		EmptyCue = RifleEmpty.Object;
	}
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PlayerAudio"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(RootComponent);

	// 사용할 무기를 맵에 기록해 놓는다
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>WeaponSK1(TEXT("SkeletalMesh'/Game/Blueprint/Weapon/SK_AR4_X.SK_AR4_X'"));
	if (WeaponSK1.Succeeded())
	{
		FName WeaponSKName = "AR-15";
		WeaponMap.Add(WeaponSKName, WeaponSK1);
	}
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>WeaponSK2(TEXT("SkeletalMesh'/Game/Blueprint/Weapon/SK_KA47_X.SK_KA47_X'"));
	if (WeaponSK2.Succeeded())
	{
		FName WeaponSKName = "AK-47";
		WeaponMap.Add(WeaponSKName, WeaponSK2);
	}
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>WeaponSK3(TEXT("SkeletalMesh'/Game/Blueprint/Weapon/SK_KA_Val_X.SK_KA_Val_X'"));
	if (WeaponSK3.Succeeded())
	{
		FName WeaponSKName = "As-Val";
		WeaponMap.Add(WeaponSKName, WeaponSK3);
	}

	// 접촉한 액터의 정보를 초기화한다.
	ContactActor = nullptr;
}

// Called when the game starts or when spawned
void ATPlayer::BeginPlay()
{
	Super::BeginPlay();
	// 디버그 메시지
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Play")));

	UWorld* const world = GetWorld();
	if (world) // 월드 유효성 검사
	{
		if (APPGameModeBase* myGmd = Cast<APPGameModeBase>(GetWorld()->GetAuthGameMode())) // 게임모드 포인터 유효성 검사
		{
			GMD = myGmd;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("GMD ON")));

			// 게임모드에서 위젯을 가져온다.
			PlayerWidget = GMD->GetPlayerStateWidget();
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Widget Called")));
			if (PlayerWidget)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Player Widget Called")));
				PlayerWidget->SetWeaponName(WeaponName);
				PlayerWidget->SetCurrentAmmo(player_ammo);
				PlayerWidget->SetRemainAmmo(player_mag);
				PlayerWidget->SetCurrentHP(player_HP);
				PlayerWidget->SetCurrentHungry(player_Hungry);
				PlayerWidget->SetCurrentThirst(player_Thirsty);
			}
			Inventory = GMD->GetInventoryWidget();
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Inventory Called")));
			if (Inventory)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Player Inventory Called")));
			}
		}
	}

}

// Called every frame
void ATPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RequireMoisture += DeltaTime * 0.1;
	RequireFat += DeltaTime * 0.1;

	if (PlayerWidget)
	{
		PlayerWidget->SetCurrentThirst(RequireMoisture);
		PlayerWidget->SetCurrentHungry(RequireFat);
	}
}

// Called to bind functionality to input
void ATPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// 이동 바인드
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPlayer::MoveRight);
	// 카메라 회전 바인드
	PlayerInputComponent->BindAxis("Turn", this, &ATPlayer::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ATPlayer::AddControllerPitchInput);
	// 점프 바인드
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPlayer::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATPlayer::StopJump);
	// 달리기 바인드
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATPlayer::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATPlayer::StopSprinting);
	// 정조준 바인드
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATPlayer::StartAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATPlayer::StopAim);
	// 사격 바인드
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPlayer::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATPlayer::StopFire);
	// 재장전 바인드
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATPlayer::StartReload);
	// 인벤토리 바인드
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &ATPlayer::InventoryToggle);
	// 상호작용 바인드
	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &ATPlayer::StartInteraction);
	PlayerInputComponent->BindAction("Interaction", IE_Released, this, &ATPlayer::StopInteraction);
}

// 전후이동 함수
void ATPlayer::MoveForward(float v)
{
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, v);
}
// 좌우이동 함수
void ATPlayer::MoveRight(float v)
{
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, v);
}
// 점프 함수
void ATPlayer::StartJump()
{
	bPressedJump = true;
}
void ATPlayer::StopJump()
{
	bPressedJump = false;
}
// 달리기 함수
void ATPlayer::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed *= SprintSpeedMultiplier;
}
void ATPlayer::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed /= SprintSpeedMultiplier;
}
// 조준 함수
void ATPlayer::StartAim()
{
	TFollowCamera->SetFieldOfView(TFollowCamera->FieldOfView /= 2);
	CheckAim = true;
	AnimInst->Aiming = true;
}
void ATPlayer::StopAim()
{
	TFollowCamera->SetFieldOfView(TFollowCamera->FieldOfView *= 2);
	CheckAim = false;
	AnimInst->Aiming = false;
}
// 애니메이션 출력용 함수
void ATPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AnimInst = Cast<UTPlayerAnimInst>(GetMesh()->GetAnimInstance());
}
// 사격 함수 구현
void ATPlayer::StartFire()
{
	isFiring = true;
	Fire();
}
void ATPlayer::StopFire()
{
	isFiring = false;
}
void ATPlayer::Fire()
{
	if (isFiring == true && CheckWeapon == true)
	{
		if (player_ammo == 0) // 총알이 없는 경우 공이 소리만 나도록 한다
		{
			AudioComponent->SetSound(EmptyCue);
			AudioComponent->Play();
			return;
		}
		if (player_ammo > 0)
		{

			// 1차 충돌 검사용 선분

			FHitResult OutHit; // 충돌 검사를 할 변수
			FVector Line1Start = TFollowCamera->GetComponentLocation(); // 선분의 시작점
			FVector Line1FV = TFollowCamera->GetForwardVector(); // 선분의 방향 벡터
			FVector Line1End = Line1Start + (Line1FV * 10000.0f); // 선분의 끝점

			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this); // 충돌에서 이 플레이어 액터를 제외한다.
			DrawDebugLine(GetWorld(), Line1Start, Line1End, FColor::Green, true);
			bool isHit = GetWorld()->LineTraceSingleByChannel(OutHit, Line1Start, Line1End, ECC_Visibility, CollisionParams);

			if (isHit)
			{
				//if (OutHit.GetActor()->ActorHasTag("Monster"))
				if(OutHit.GetActor())
				{
					DrawDebugSolidBox(GetWorld(), OutHit.ImpactPoint, FVector(10.0f), FColor::Blue, true); // 피격된 지점에 박스를 그린다
					UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *OutHit.GetActor()->GetName());
					UE_LOG(LogTemp, Log, TEXT("Hit Bone : %s"), *OutHit.BoneName.ToString());
					AActor* HitActor = OutHit.GetActor();

					GameStatic->ApplyPointDamage(HitActor, player_Damage, HitActor->GetActorLocation(), OutHit, nullptr, this, nullptr); // 데미지를 가한다.
				}
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Fire")));

			// 사운드 재생은 애니메이션 노티파이에 부착했다
			// 소리가 중첩될 수 있도록 처리함
			AnimInst->PlayFire(); // 사격 모션을 동작시킨다.
			PlayerWidget->SetCurrentAmmo(--player_ammo);
			GetWorld()->GetTimerManager().SetTimer(timer, this, &ATPlayer::Fire, FireRate, false);
		}
	}
}
// 재장전 함수 구현
void ATPlayer::StartReload()
{
	if (player_mag != 0 && CheckWeapon == true)
	{
		AnimInst->PlayReload(); // 재장전 모션을 동작시킨다
		IsReloading = true;
	}
}
void ATPlayer::ReloadEnd()
{
	IsReloading = false;
	PlayerWidget->SetCurrentAmmo(--player_mag);
}

// 무기 장착 사실을 전달할 함수
bool ATPlayer::GetWeaponCheck()
{
	return CheckWeapon;
}
// 조준 여부를 전달할 함수
bool ATPlayer::GetAimCheck()
{
	return CheckAim;
}
// 인벤토리 토글 함수
void ATPlayer::InventoryToggle()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Inventory Toggle")));
	if (Inventory != nullptr)
	{
		if (Inventory->GetVisibility() == ESlateVisibility::Collapsed)
		{
			Inventory->SetVisibility(ESlateVisibility::Visible);
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeGameAndUI::FInputModeGameAndUI());
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->bEnableClickEvents = true;
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->bEnableMouseOverEvents = true;
		}
		else
		{
			Inventory->SetVisibility(ESlateVisibility::Collapsed);
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeGameOnly::FInputModeGameOnly());
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = false;
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->bEnableClickEvents = false;
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->bEnableMouseOverEvents = false;
		}
	}
}
void ATPlayer::UseItem(int32 itemIdx, float Inc_HP, float Dec_Hungry, float Dec_Thirst)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Item Index %d"), itemIdx));
	switch(itemIdx)
	{
	case 4: // Water
	{
		RequireFat -= Dec_Hungry;
		RequireMoisture -= Dec_Thirst;
		break;
	}
	case 5: // Coke
	{
		RequireFat -= Dec_Hungry;
		RequireMoisture -= Dec_Thirst;
		break;
	}
	case 6: // FoodCan
	{
		RequireFat -= Dec_Hungry;
		RequireMoisture -= Dec_Thirst;
		break;
	}
	case 7: // SoupCan
	{
		RequireFat -= Dec_Hungry;
		RequireMoisture -= Dec_Thirst;
		break;
	}
	}
	if (RequireFat < 0.0f) RequireFat = 0.0f;
	if (RequireMoisture < 0.0f) RequireMoisture = 0.0f;
	PlayerWidget->SetCurrentHungry(RequireFat);
	PlayerWidget->SetCurrentThirst(RequireMoisture);
}
// 상호작용 함수 구현
void ATPlayer::StartInteraction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("On")));
	if (OnLootingStarted.IsBound() == true)
	{
		OnLootingStarted.Execute(); // 델리게이트를 동작시킨다.
	}

}
void ATPlayer::StopInteraction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Off")));
	if (OnLootingCancled.IsBound() == true)
	{
		OnLootingCancled.Execute();
	}
}

float ATPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Log, TEXT("Actor : %s Took Damage : %f"), *GetName(), FinalDamage);

	player_HP -= FinalDamage;
	if (PlayerWidget)
	{
		PlayerWidget->SetCurrentHP(player_HP);
	}

	return FinalDamage;
}

// 무기 장착 인터페이스 함수
void ATPlayer::EquipWeaponItem_Implementation(FName weapon_Name, int32 weaponAmmo, float weaponDamage, float weaponFireRate, int32 weaponIDX)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Interface Called")));
	
	WeaponName = weapon_Name;
	player_ammo = weaponAmmo;
	if (PlayerWidget)
	{
		PlayerWidget->SetWeaponName(WeaponName);
		PlayerWidget->SetCurrentAmmo(player_ammo);
	}
	player_Damage = weaponDamage;
	FireRate = weaponFireRate;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Damage %f"), player_Damage));
	AnimInst->ShotIDX = weaponIDX;

	USkeletalMesh* CurrentWeapon = WeaponMap.Find(WeaponName)->Object;
	if (CurrentWeapon != nullptr)
	{
		Weapon_Socket->SetSkeletalMesh(CurrentWeapon);
		CheckWeapon = true;
		AnimInst->WeaponState = true;
	}
}

void ATPlayer::GetAmmoItem_Implementation(FName Ammo_Name, int32 Ammo_Type)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Get Ammo Interface Called")));
	if (Inventory)
	{
		Inventory->AmmoInserter(Ammo_Name, Ammo_Type);

	}
}

void ATPlayer::GetFoodItem_Implementation(FName Food_Name, float fHungry, float fThirst, int32 FoodType)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Get Food Interface Called")));
	if (Inventory)
	{
		Inventory->FoodInserter(Food_Name, fHungry, fThirst, FoodType);
	}
}

void ATPlayer::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ContactActor = OtherActor;
}

void ATPlayer::OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ContactActor = nullptr;
}