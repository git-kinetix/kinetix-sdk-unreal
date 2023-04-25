// // Copyright Kinetix. All Rights Reserved.


#include "Core/Metadata/KinetixMetadata.h"

void UKinetixMetadata::GetAnimationMetadataByAnimationIDs(FAnimationID InID, const FOnMetadataAvailable& Callback)
{
	
}

void UKinetixMetadata::IsAnimationOwnedByUser(FAnimationID InID, const FOnMetadataOwnershipLoaded& Callback)
{
}

void UKinetixMetadata::GetUserAnimationMetadatas(const FOnMetadatasAvailable& Callback)
{
}

void UKinetixMetadata::GetUserAnimationMetadatasByPage(int InCount, int InPageNumber,
	const FOnMetadatasAvailable& Callback)
{
}

void UKinetixMetadata::GetUserAnimationMetadatasTotalPagesCount(int InCountPerPage,
	const FOnTotalNumberOfPagesAvailable& Callback)
{
}
