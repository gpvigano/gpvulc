//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


#pragma once

namespace gpvulc
{

	/// @addtogroup DS
	/// @{

	/// @addtogroup Util
	/// @{

	/// Define/set/call a callback to be called when busy (loading files, processing geometries, ...)
	/// A message related to the current activity can be displayed by the callback.
	/// @name Busy callback
	///@{

	//! Callback to be called for time expensive tasks (see SetBusyCallBack()).
	typedef void (gvBusyCallBack)(const char*, void*);

	/*!
	 @brief Register a callback to be called for time expensive tasks (see gvCallBusyCallBack()).
	 @details To avoid affecting performance the given callback must be very lightweight,
	 otherwise you can call it only after a given time interval.
	*/
	void SetBusyCallBack(gvBusyCallBack* cb, void* data);

	//! Get the registered busy callback (see SetBusyCallBack())
	gvBusyCallBack* GetBusyCallBack(void** data);

	//! Call the registered busy callback (see SetBusyCallBack())
	void gvCallBusyCallBack(const char* msg = nullptr);

	///@}
	///@}
	///@}
}
