// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Powerboat.generated.h"

UCLASS()
class METAVERSEVR_API APowerboat : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APowerboat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* powerboat;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
