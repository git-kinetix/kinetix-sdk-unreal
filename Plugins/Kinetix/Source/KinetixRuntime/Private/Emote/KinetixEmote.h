// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"

/**
 * 
 */
class FKinetixEmote
{
public:
	FKinetixEmote();
	FKinetixEmote(const FAnimationID& InAnimationID);
	~FKinetixEmote();

	/**
 * @brief Check if emote contains metadata
 * @return True if contains metadata
 */
	bool HasMetadata() const;

	/**
	 * @brief Set the metadata for this emote
	 * @param InAnimationMetadata Metadata of the emote
	 */
	void SetMetadata(const FAnimationMetadata& InAnimationMetadata);

	/**
	 * @brief Set the metadata for this emote
	 * @param InAnimationMetadata Metadata of the emote
	 * @param InLocalGLBPath Absolute local path on disk
	 */
	void SetLocalMetadata(const FAnimationMetadata& InAnimationMetadata, FString InLocalGLBPath);

	/**
	 * @brief Mainly used for network purposes
	 * Set only the minimum datas for a valid emote
	 * @param InAnimationID ID of the animation
	 * @param InAnimationURL URL of the animation
	 */
	void SetShortMetadata(const FAnimationID& InAnimationID, FString InAnimationURL);

	/**
	 * @brief Check if the GLB file is used (e.g. Import Retargeting)
	 * @return True if currently used
	 */
	bool IsFileInUse() const;

	/**
	 * @brief Check if the emote has fully been retargeted for the AnimInstance
	 * @param InAnimInstance AnimInstance to check
	 * @return True if the emote is ready to be played
	 */
	bool HasAnimationRetargeted(UAnimInstance* InAnimInstance);

	bool IsLocal() const;
	
	FString GetPathToGlb() const;
private:
	
	/**
	 * @brief Check if the GLB path points to a valid GLB file
	 * @return True if the GLB file exists
	 */
	bool HasValidPath() const;

	/**
	 * @brief Check if the emote is currently retargeting to that specific AnimInstance
	 * @param InAnimInstance AnimInstance to check
	 * @return True if the emote is currently retargeting to that specific AnimInstance
	 */
	bool IsRetargeting(UAnimInstance* InAnimInstance);
	
private:

	FAnimationID AnimationID;

	bool bIsLocal;

	FAnimationMetadata AnimationMetadata;

	FString PathToGLB;

};
