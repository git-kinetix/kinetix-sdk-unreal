// Copyright Kinetix. All Rights Reserved.


#include "KinanimPointerCollector.h"

bool bEnabled = false;
TArray<IDisposable*> Pointers;

void KinanimPointerCollector::SetEnabled(bool enabled) 
{
	bEnabled = enabled;
}

void KinanimPointerCollector::CollectPointer(IDisposable* ptr)
{
	if (!bEnabled)
		return;

	if (ptr == nullptr)
		return;

	//Collect only if it doesn't exist yet
	if (KinanimPointerCollector::IsDisposed(ptr))
		Pointers.Add(ptr);
}

bool KinanimPointerCollector::ReleasePointer(IDisposable* ptr)
{
	if (!bEnabled)
		return false;

	if (ptr == nullptr)
		return false;

	if(Pointers.IsEmpty())
		return false;

	return Pointers.Remove(ptr) == 1;
	// TArray<IDisposable*>::TIterator position = TArray<IDisposable*>::fin(Pointers.begin(), Pointers.end(), ptr);
	// if (position != Pointers.end()) // == myVector.end() means the element was not found
	// 	{
	// 	Pointers.erase(position);
	// 	return true;
	// 	}
	// return false;
}

bool KinanimPointerCollector::IsDisposed(IDisposable* ptr)
{
	return Pointers.Contains(ptr);
}

void KinanimPointerCollector::DisposeAll()
{
	for (int i = Pointers.Num() - 1; i >= 0; --i)
	{
		delete Pointers[i];
	}
	Pointers.Empty();
	
	// for each (IDisposable* disposable in Pointers)
	// {
	// 	delete disposable;
	// }
}