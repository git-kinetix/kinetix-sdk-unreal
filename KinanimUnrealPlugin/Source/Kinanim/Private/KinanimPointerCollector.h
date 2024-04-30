// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IDisposable
{
public:
	virtual inline ~IDisposable()
	{
	};
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 
 */
namespace KinanimPointerCollector
{
		/// <summary>
	/// Enable pointer collecting
	/// </summary>
	/// <param name="enabled"></param>
	/// <returns></returns>
	void SetEnabled(bool enabled);

	/// <summary>
	/// Add pointer to the collector
	/// </summary>
	/// <param name="ptr">Pointer to add</param>
	void CollectPointer(IDisposable* ptr);
	/// <summary>
	/// Remove pointer from the collector
	/// </summary>
	/// <param name="ptr">Pointer to add</param>
	/// <returns>Returns true if the pointer has been released (= it wasn't a disposed pointer)</returns>
	bool ReleasePointer(IDisposable* ptr);

	/// <summary>
	/// Check if pointer is disposed or not
	/// </summary>
	/// <param name="ptr">Pointer to check</param>
	/// <returns>If true, the pointer has already been disposed</returns>
	bool IsDisposed(IDisposable* ptr);

	/// <summary>
	/// Dispose all collected pointers
	/// </summary>
	void DisposeAll();

	/// <summary>
	/// Enable pointer collecting
	/// </summary>
	/// <param name="enabled"></param>
	/// <returns></returns>
	/* __LINKER_DLL_EXPORT__ */
	inline void KinanimPointerCollector_SetEnabled(bool enabled) { return SetEnabled(enabled); };

	/// <summary>
	/// Add pointer to the collector
	/// </summary>
	/// <param name="ptr">Pointer to add</param>
	/* __LINKER_DLL_EXPORT__ */
	inline void KinanimPointerCollector_CollectPointer(void* ptr) { CollectPointer(static_cast<IDisposable*>(ptr)); };

	/// <summary>
	/// Remove pointer from the collector
	/// </summary>
	/// <param name="ptr">Pointer to add</param>
	/// <returns>Returns true if the pointer has been released (= it wasn't a disposed pointer)</returns>
	/* __LINKER_DLL_EXPORT__ */
	inline bool KinanimPointerCollector_ReleasePointer(void* ptr) { return ReleasePointer(static_cast<IDisposable*>(ptr)); };

	/// <summary>
	/// Check if pointer is disposed or not
	/// </summary>
	/// <param name="ptr">Pointer to check</param>
	/// <returns>If true, the pointer has already been disposed</returns>
	/* __LINKER_DLL_EXPORT__ */
	inline bool KinanimPointerCollector_IsDisposed(void* ptr) { return IsDisposed(static_cast<IDisposable*>(ptr)); };

	/// <summary>
	/// Dispose all collected pointers
	/// </summary>
	/* __LINKER_DLL_EXPORT__ */
	inline void KinanimPointerCollector_DisposeAll() { DisposeAll(); };
};

#ifdef __cplusplus
}
#endif
