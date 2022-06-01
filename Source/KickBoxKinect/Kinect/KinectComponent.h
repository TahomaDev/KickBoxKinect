// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#include "Kinect.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KinectComponent.generated.h"

template <typename T>
static void SetArrayElement(T item, TArray<T>& item_array, int32 index)
{
	if (item_array.Num() - 1 < index)
		item_array.SetNum(index);

	item_array.Insert(item, index);
}

UENUM(BlueprintType)
enum class ETrackingState : uint8
{
	TrackingState_NotTracked	= 0,
	TrackingState_Inferred		= 1,
	TrackingState_Tracked		= 2
};

UENUM(BlueprintType)
enum class EJointType : uint8
{
	SpineBase		= 0,
	SpineMid		= 1,
	Neck			= 2,
	Head			= 3,
	ShoulderLeft	= 4,
	ElbowLeft		= 5,
	WristLeft		= 6,
	HandLeft		= 7,
	ShoulderRight	= 8,
	ElbowRight		= 9,
	WristRight		= 10,
	HandRight		= 11,
	HipLeft			= 12,
	KneeLeft		= 13,
	AnkleLeft		= 14,
	FootLeft		= 15,
	HipRight		= 16,
	KneeRight		= 17,
	AnkleRight		= 18,
	FootRight		= 19,
	SpineShoulder	= 20,
	HandTipLeft		= 21,
	ThumbLeft		= 22,
	HandTipRight	= 23,
	ThumbRight		= 24,
	Count			= ( ThumbRight + 1 )
};

USTRUCT(BlueprintType)
struct FJoint
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EJointType JointType;
	UPROPERTY(BlueprintReadWrite)
	FVector Position;
	UPROPERTY(BlueprintReadWrite)
	ETrackingState TrackingState;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKinectBodySend, FJoint, Joint);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KICKBOXKINECT_API UKinectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKinectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	TMap<EJointType, FTransform> GetKinectBodyTransform(int BodyIndex);

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FKinectBodySend KinectBodySend;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int KinectBodiesNum = 6;
	
	
private:

	void InitializeKinectSensor();
	void KinectBodyUpdate();
	void CreateEmptyKinectbodiesMassive();
	void KinectProcess(INT64 nTime, int nBodyCount, IBody** ppBodies);
	
	IKinectSensor*          m_pKinectSensor = nullptr;
	ICoordinateMapper*      m_pCoordinateMapper = nullptr;
	IBodyFrameReader*       m_pBodyFrameReader = nullptr;
	TArray<TMap<EJointType, FTransform>> KinectBodiesPositions;	
		
};
