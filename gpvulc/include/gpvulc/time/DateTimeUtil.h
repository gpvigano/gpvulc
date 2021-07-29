//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Time Utility
/// @file TimeUtil.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include "gpvulc/time/DateTime.h"

#include <string>

namespace gpvulc
{

	/// @addtogroup Time
	/// @{

	DateTime StringToDateTime(const std::string& timeString);

	const std::string& DateTimeToString(const DateTime& dateTime);

	const char* DateTimeToCString(const DateTime& dateTime);

	//! Calculate the time in milliseconds between two date/time
	long long DateTimeDistanceMs(const DateTime& dateTime1, const DateTime& dateTime2);

	//! Calculate the time in seconds between two date/time
	long long DateTimeDistanceSec(const DateTime& dateTime1, const DateTime& dateTime2);

	//! Calculate the time in seconds (with decimals) between two date/time
	double DateTimeDistanceSecD(const DateTime& dateTime1, const DateTime& dateTime2);

	//! Calculate the time between two date/time and convert it to a string with hours, minutes, seconds
	std::string DateTimeDistanceString(const DateTime& dateTime1, const DateTime& dateTime2);
	//void DateTimeToLocal(DateTime& dateTime);

	//void DateTimeToUTC(DateTime& dateTime);

	//! Add to a date time the given hours, minutes, seconds, milliseconds.
	DateTime DateTimeAdd(const DateTime& dateTime1, int h, int m, int s, int ms = 0);

	//! Add to a date time the given milliseconds.
	DateTime DateTimeAddMs(const DateTime& dateTime, long ms);

	///@}
}//namespace gpvulc


/// @addtogroup Time
/// @{

bool operator<(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2);

bool operator==(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2);
inline bool operator!=(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2)
{
	return !(dateTime1 == dateTime2);
}

bool operator>(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2);

bool operator<=(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2);

bool operator>=(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2);

///@}

