// Fill out your copyright notice in the Description page of Project Settings.


#include "CppAICharacterBase.h"

#include "AITemplateCharacter.h"
#include "Consts.h"
#include "HealthBarWidget.h"
#include "NPC.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
ACppAICharacterBase::ACppAICharacterBase() :
WidgetComponent{CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthValue"))},
Health{MaxHealth},
RightFistCollisionBox{CreateDefaultSubobject<UBoxComponent>(TEXT("RightFistCollisionBox"))}
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (WidgetComponent)
	{
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComponent->SetRelativeLocation(Defs::HealthBarZ);
		static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass{TEXT("/Game/UI/WBP_HealthBar")};
		if (WidgetClass.Succeeded())
		{
			WidgetComponent->SetWidgetClass(WidgetClass.Class);
		}
	}
	if (RightFistCollisionBox)
	{
		RightFistCollisionBox->SetBoxExtent(Defs::RightFistBoxSize, false);
		FAttachmentTransformRules const Rules {
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false
		};
		RightFistCollisionBox->AttachToComponent(GetMesh(), Rules, Defs::RightFistSocketName);
		RightFistCollisionBox->SetRelativeLocation(Defs::CollisionBoxLocation);
	}
}

// Called when the game starts or when spawned
void ACppAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
	RightFistCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACppAICharacterBase::OnAttackOverlapBegin);
	RightFistCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ACppAICharacterBase::OnAttackOverlapEnd);
	
}

// Called every frame
void ACppAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (auto const Widget = Cast<UHealthBarWidget>(WidgetComponent->GetUserWidgetObject()))
	{
		Widget->SetBarValuePercent(Health / MaxHealth);
	}

}

// Called to bind functionality to input
void ACppAICharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ACppAICharacterBase::GetHealth() const
{
	return Health;
}

float ACppAICharacterBase::GetMaxHealth() const
{
	return MaxHealth;
}

void ACppAICharacterBase::SetHealth(float const NewHealth)
{
	Health = NewHealth;
	if ( Cast<AAITemplateCharacter>(this))
	{
		if (Health <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("You Lose!!!"));
			GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
		}
		
	}
	else if ( Cast<ANPC>(this))
	{
		if (Health <=0)
		{
			UE_LOG(LogTemp, Warning, TEXT("You Win!!!"));
			GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
		}
	}
}

void ACppAICharacterBase::AttackStart() const
{
	RightFistCollisionBox->SetCollisionProfileName("Fist");
	RightFistCollisionBox->SetNotifyRigidBodyCollision(true);
}

void ACppAICharacterBase::AttackEnd() const
{
	RightFistCollisionBox->SetCollisionProfileName("Fist");
	RightFistCollisionBox->SetNotifyRigidBodyCollision(false);
}

void ACppAICharacterBase::OnAttackOverlapBegin(UPrimitiveComponent* const OverlappedComponent, AActor* const OtherActor,
	UPrimitiveComponent* const OtherComponent, int const OtherBodyIndex, bool const FromSweep,
	FHitResult const& SweepResult)
{
	if (OtherActor == this)
	{
		return;
	}
	if (auto const Enemy = Cast<ANPC>(OtherActor))
	{
		auto const NewHealth = Enemy->GetHealth() - Enemy->GetMaxHealth()* 0.1;
		Enemy->SetHealth(NewHealth);
	}
	else if (auto const Player = Cast<AAITemplateCharacter>(OtherActor))
	{
		auto const NewHealth = Player->GetHealth() - Player->GetMaxHealth()* 0.07;
		Player->SetHealth(NewHealth);
	}
}

void ACppAICharacterBase::OnAttackOverlapEnd(UPrimitiveComponent* const OverlappedComponent, AActor* const OtherActor,
	UPrimitiveComponent* const OtherComponent, int const OtherBodyIndex)
{
	
}

