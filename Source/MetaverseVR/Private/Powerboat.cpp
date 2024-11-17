// Fill out your copyright notice in the Description page of Project Settings.


#include "Powerboat.h"
#include "BuoyancyComponent.h"

// Sets default values
APowerboat::APowerboat()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	powerboat = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Powerboat"));
	buoyancyComp = CreateDefaultSubobject<UBuoyancyComponent>(TEXT("Buoyancy"));
}

// Called when the game starts or when spawned
void APowerboat::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APowerboat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APowerboat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

