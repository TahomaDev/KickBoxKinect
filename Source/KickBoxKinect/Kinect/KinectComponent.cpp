// Fill out your copyright notice in the Description page of Project Settings.


#include "KinectComponent.h"

// Sets default values for this component's properties
UKinectComponent::UKinectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UKinectComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateEmptyKinectbodiesMassive();
	InitializeKinectSensor();
	
}


// Called every frame
void UKinectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	KinectBodyUpdate();
}

void UKinectComponent::CreateEmptyKinectbodiesMassive()
{
	TMap<EJointType, FTransform> MapNull;

	KinectBodiesPositions.Reset();
	for (int i = 0; i < KinectBodiesNum; i++)
	{
		KinectBodiesPositions.Add(MapNull);
	}
}

TMap<EJointType, FTransform> UKinectComponent::GetKinectBodyTransform(int BodyIndex)
{
	if (BodyIndex > 5)
		BodyIndex = 5;

	if (KinectBodiesPositions.Num() < BodyIndex)
	{
		UE_LOG(LogTemp, Error, TEXT("KinectComponent - Not found BodyIndex %d"), BodyIndex);
		TMap<EJointType, FTransform> MapNull;
		return MapNull;
	}

	return KinectBodiesPositions[BodyIndex];
}

void UKinectComponent::InitializeKinectSensor()
{
	HRESULT hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		UE_LOG(LogTemp, Error, TEXT("KinectComponent - Default kinect sensor in NULL"));
		return;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = nullptr;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		if (pBodyFrameSource != nullptr)
		{
			pBodyFrameSource->Release();
		}
	}
}

void UKinectComponent::KinectBodyUpdate()
{
	if (m_pBodyFrameReader == nullptr)
	{
		return;
	}

	IBodyFrame* pBodyFrame = nullptr;
	HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IBody* ppBodies[BODY_COUNT] = {0};
		hr = pBodyFrame->get_RelativeTime(&nTime);

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			KinectProcess(nTime, BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			if (ppBodies[i])
			{
				ppBodies[i]->Release();
			}
		}
	}

	if (pBodyFrame)
	{
		pBodyFrame->Release();
	}
}

void UKinectComponent::KinectProcess(INT64 nTime, int nBodyCount, IBody** ppBodies)
{
	if (nBodyCount <= 0)
		return;

	TMap<EJointType, FTransform> KinectJointsPositions;
	
	FTransform Transform;
	EJointType JointType;
	FQuat JointQuaternion;

	for (int i = 0; i < nBodyCount; i++)
	{
		KinectJointsPositions.Reset();
		
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			BOOLEAN bTracked = false;
			pBody->get_IsTracked(&bTracked);
			if (bTracked)
			{
				Joint joints[JointType_Count];
				JointOrientation Orients[JointType_Count];
				HandState leftHandState = HandState_Unknown;
				HandState rightHandState = HandState_Unknown;

				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				pBody->GetJoints(_countof(joints), joints);
				pBody->GetJointOrientations(_countof(Orients), Orients);
				pBody = nullptr;

				for (int l = 0; l < JointType_Count; l++)
				{
					ETrackingState TrackingState = static_cast<ETrackingState>(joints[l].TrackingState);
					
					FVector Location = FVector::Zero();
					Location.X = -joints[l].Position.Z;
					Location.Y = -joints[l].Position.X;
					Location.Z = joints[l].Position.Y;
					Transform.SetLocation(Location);

					JointType = static_cast<EJointType>(joints[l].JointType);
					KinectJointsPositions.Add(JointType, Transform);
					
					FTransform* Transform2 = KinectJointsPositions.Find(JointType);
					Transform = *Transform2;

					/*
					* Kinect Camera					Unreal
					* --------------------------------------
					* +ve X-axis		Right		+ve Y-axis
					* +ve Y-axis		Down		-ve Z-axis
					* +ve Z-axis		Forward		+ve X-axis
					*/

					FRotator Rotation;
					JointQuaternion = FQuat(
										Orients[l].Orientation.y,
										Orients[l].Orientation.z,
										Orients[l].Orientation.x,
										Orients[l].Orientation.w);

					JointType = static_cast<EJointType>(Orients[l].JointType);

					switch(JointType)
					{
					case EJointType::SpineBase:
						Location = (Transform.GetLocation() + 1.f) * 100;
						Transform.SetLocation(Location);
						break;
					case EJointType::SpineMid:
/*						JointQuaternion = FQuat(
										Orients[l].Orientation.y,
										Orients[l].Orientation.z,
										Orients[l].Orientation.x,
										Orients[l].Orientation.w);*/
						Rotation = FRotator(JointQuaternion) + FRotator(0.f, 0.f, 180.f);
						Rotation.Yaw *= -1;
						Rotation.Pitch *= -1;
						JointQuaternion = FQuat(Rotation);
						break;
					case EJointType::Neck:
						break;
					case EJointType::Head:
						break;
					case EJointType::HipLeft:
						break;
					case EJointType::KneeLeft:
						break;
					case EJointType::AnkleLeft:
						break;
					case EJointType::FootLeft:
//						Location = (Transform.GetLocation() + 1.f) * 100;
//						Transform.SetLocation(Location);
						break;
					case EJointType::HipRight:
						break;
					case EJointType::KneeRight:
						break;
					case EJointType::AnkleRight:
						break;
					case EJointType::FootRight:
//						Location = (Transform.GetLocation() + 1.f) * 100;
//						Transform.SetLocation(Location);
						break;
					case EJointType::ShoulderLeft:
						break;
					case EJointType::ShoulderRight:
						break;
					case EJointType::ElbowLeft:
						break;
					case EJointType::WristLeft:
						Rotation = FRotator(JointQuaternion) + FRotator(90.f, 0.f, 0.f);
						Rotation.Yaw *= -1;
						JointQuaternion = FQuat(Rotation);

						break;
					case EJointType::HandLeft:
						Location = Transform.GetLocation() * 100;
						Transform.SetLocation(Location);
						break;
					case EJointType::ElbowRight:
						break;
					case EJointType::WristRight:
						Rotation = FRotator(JointQuaternion) + FRotator(90.f, 0.f, 0.f);
						Rotation.Yaw *= -1;
						JointQuaternion = FQuat(Rotation);

						break;
					case EJointType::HandRight:
						Location = Transform.GetLocation() * 100;
						Transform.SetLocation(Location);


						break;
						
//					default:
					}
					

					Transform.SetRotation(JointQuaternion);
					KinectJointsPositions.Add(JointType, Transform);

					
				}
				
				if (KinectBodiesPositions.Num() >= i)
				{
					SetArrayElement<TMap<EJointType, FTransform>>(KinectJointsPositions, KinectBodiesPositions, i);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("KinectComponent - Kinect Bodies Positions array is overload [%d]"), i);
				}
			}
		}
	}
}

