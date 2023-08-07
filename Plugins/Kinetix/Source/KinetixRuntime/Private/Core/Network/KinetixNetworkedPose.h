// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/NetSerialization.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "KinetixNetworkedPose.generated.h"

USTRUCT()
struct FBonePoseInfo
{
	GENERATED_BODY()

	// Position of the bone in component space
	UPROPERTY()
	FVector LocalPosition;

	UPROPERTY()
	FQuat LocalQuaternion;

	UPROPERTY()
	FVector LocalScale;

	FBonePoseInfo()
	{
		LocalPosition = FVector::ZeroVector;
		LocalQuaternion = FQuat::Identity;
		LocalScale = FVector::OneVector;
	}
	
	FBonePoseInfo(const FTransform& Transform)
	{
		LocalPosition = Transform.GetLocation();
		LocalQuaternion = Transform.GetRotation();
		LocalScale = Transform.GetScale3D();
	}

	void SerializeCompressedShort(FArchive& Ar, uint8 Flags);
};

USTRUCT()
struct FKinetixNetworkedPose
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGuid Guid;

	UPROPERTY()
	double TimeStamp;

	UPROPERTY()
	bool bPosEnabled;

	UPROPERTY()
	bool bScaleEnabled;
	
	UPROPERTY()
	bool bHasBlendshapes;

	UPROPERTY()
	bool bHasArmature;

	UPROPERTY()
	TArray<FBonePoseInfo> Bones; 

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FKinetixNetworkedPose> : public TStructOpsTypeTraitsBase2<FKinetixNetworkedPose>
{
	enum
	{
		WithNetSerializer = true
	};
};