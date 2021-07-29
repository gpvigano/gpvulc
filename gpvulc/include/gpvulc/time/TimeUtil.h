//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


#pragma once

/// @anchor Time
/// @defgroup Time Time
/// @brief Date/time Utility library.
/// @author Giovanni Paolo Vigano'

/// @brief Date/Time Utility
/// @file TimeUtil.h
/// @author Giovanni Paolo Vigano'

#include "gpvulc/time/DateTime.h"

#include <string>

namespace gpvulc
{

	/// @addtogroup Time
	/// @{

	//! Time duration expressed in days, hours, minutes, seconds, milliseconds
	struct TimeDuration
	{
		int Days = 0;  //!< duration in days [0, ...]
		int Hours = 0;  //!< hours since midnight [0, 23]
		int Minutes = 0;   //!< minutes after the hour [0, 59]
		int Seconds = 0;   //!< seconds after the minute [0, 59]
		int Milliseconds = 0;   //!< milliseconds after the second [0, 999]
	};


	//! Get the current time in microseconds (since the first call).
	long long GetRunTimeMicroseconds();

	//! Get the current time in milliseconds (since the first call).
	long long GetRunTimeMilliseconds();

	//! Get the current time in seconds (since the first call).
	double GetRunTimeSeconds();

	//! Get current time and date
	DateTime GetDateTimeNow();

	//! Get a long identifier based on system time and date
	long GetSystemTimeStamp();

	//! Get a string identifier based on system time and date
	std::string GetSystemTimeStampID();

	/*!
	 Convert the given time (seconds) into days, hours, minutes, seconds, milliseconds
	 @return filled time duration structure
	*/
	TimeDuration SecondsToTimeDuration(double timeSeconds);

	//! Convert the given time duration structure in seconds
	double TimeDurationToSeconds(const TimeDuration& timeDuration);

	///@}

}//namespace gpvulc
