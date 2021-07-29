//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// Date/time utility


#include <gpvulc/time/DateTimeUtil.h>


#include "boost/date_time.hpp"

#include <iomanip>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <iomanip>

namespace
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	using namespace gpvulc;

	inline void DateTimeSetWeekDay(DateTime& dateTime)
	{
		if (dateTime.Month > 0)
		{
			date d(dateTime.Year, dateTime.Month, dateTime.Day);
			dateTime.WeekDay = d.day_of_week().as_number();
		}
	}

	ptime DateTimeToTime(const DateTime& dateTime)
	{

		ptime time(
			date(
				(unsigned short)dateTime.Year,
				(unsigned short)dateTime.Month,
				(unsigned short)dateTime.Day),
			hours(dateTime.Hour) + minutes(dateTime.Minute) + seconds(dateTime.Second)
			+ milliseconds(dateTime.Millisecond)
			- hours(dateTime.TimeOffsetHour) - minutes(dateTime.TimeOffsetMinute));

		return time;
	}


	long DateTimeToMs(const DateTime& dateTime)
	{
		// DateTime considered as a time counter
		const long msInSec = 1000L;
		const long msInMin = 60L*msInSec;
		const long msInHour = 60L*msInMin;
		const long msInDay = 24L * msInHour;

		long sSum = dateTime.Second * msInSec;
		long mSum = dateTime.Minute * msInMin;
		long hSum = dateTime.Hour * msInHour;
		long dSum = dateTime.Day * msInDay;
		long ms = dSum + hSum + mSum + sSum + dateTime.Millisecond;
		return ms;
	}


	time_duration DateTimeDist(const DateTime& dateTime1, const DateTime& dateTime2)
	{
		ptime time1 = DateTimeToTime(dateTime1);
		ptime time2 = DateTimeToTime(dateTime2);
		time_duration timeDuration = time2 - time1;

		return timeDuration;
	}


	DateTime DateTimeSetTime(const DateTime& dateTime, const ptime& time)
	{
		date d = time.date();
		time_duration t = time.time_of_day();
		DateTime result = dateTime;
		result.Year = d.year();
		result.Month = d.month();
		result.Day = d.day();
		result.Hour = (int)t.hours();
		result.Minute = (int)t.minutes();
		result.Second = (int)t.seconds();
		long long totMs = t.total_milliseconds();
		long long hmsSec = result.Hour * 3600 + result.Minute * 60 + result.Second;
		result.Millisecond = (int)(totMs - 1000L * hmsSec);
		DateTimeSetWeekDay(result);

		return result;
	}

}


namespace gpvulc
{
	DateTime StringToDateTime(const std::string& timeString)
	{
		DateTime dateTime;
		std::istringstream sStream(timeString);
		char c;
		// YYYY-MM-DDTHH:MM:SS.ms
		sStream
			>> dateTime.Year >> c >> dateTime.Month >> c >> dateTime.Day
			>> c // T
			>> dateTime.Hour >> c >> dateTime.Minute >> c >> dateTime.Second;
		if (!sStream.eof())
		{
			sStream >> c;
			if (c == '.')
			{
				sStream >> dateTime.Millisecond;
				sStream >> c;
			}
			if (c == 'U')
			{
				sStream >> c >> c; // UTC
				sStream >> dateTime.TimeOffsetHour
					>> c >> dateTime.TimeOffsetMinute;
			}
		}
		DateTimeSetWeekDay(dateTime);
		return dateTime;
	}


	const std::string& DateTimeToString(const DateTime& dateTime)
	{
		std::ostringstream oStream;
		oStream << std::setfill('0')
			<< std::setw(4) << dateTime.Year
			<< '-'
			<< std::setw(2) << dateTime.Month
			<< '-'
			<< std::setw(2) << dateTime.Day
			<< 'T'
			<< std::setw(2) << dateTime.Hour
			<< ':'
			<< std::setw(2) << dateTime.Minute
			<< ':'
			<< std::setw(2) << dateTime.Second;
		if (dateTime.Millisecond > 0)
		{
			oStream << '.' << std::setfill('0')
				<< std::setw(3) << dateTime.Millisecond;
		}
		if (dateTime.TimeOffsetHour != 0 || dateTime.TimeOffsetMinute != 0)
		{
			oStream << "UTC";
			oStream << std::showpos << std::setfill('0')
				<< std::setw(2) << dateTime.TimeOffsetHour
				<< ':'
				<< std::setw(2) << dateTime.TimeOffsetMinute;
		}
		static std::string str;
		str = oStream.str();
		return str;
	}

	const char* DateTimeToCString(const DateTime& dateTime)
	{
		static std::string str;

		str = DateTimeToString(dateTime);
		return str.c_str();
	}

	long long DateTimeDistanceSec(const DateTime& dateTime1, const DateTime& dateTime2)
	{
		if (dateTime1.Month == 0 || dateTime2.Month == 0)
		{
			long long sec = (DateTimeToMs(dateTime2) - DateTimeToMs(dateTime1)) / 1000L;
			return sec;
		}


		return DateTimeDist(dateTime1, dateTime2).seconds();
	}


	double DateTimeDistanceSecD(const DateTime& dateTime1, const DateTime& dateTime2)
	{
		double s = (double)DateTimeDistanceMs(dateTime1, dateTime2) / 1000.0;
		return s;
	}


	long long DateTimeDistanceMs(const DateTime& dateTime1, const DateTime& dateTime2)
	{
		if (dateTime1.Month == 0 || dateTime2.Month == 0)
		{
			long long ms = DateTimeToMs(dateTime2) - DateTimeToMs(dateTime1);
			return ms;
		}
		long long tdSec = DateTimeDistanceSec(dateTime1, dateTime2);
		long long ms = tdSec * 1000 + dateTime2.Millisecond - dateTime1.Millisecond;
		return ms;
	}


	std::string DateTimeDistanceString(const DateTime& dateTime1, const DateTime& dateTime2)
	{
		return to_simple_string(DateTimeDist(dateTime1, dateTime2));
	}


	DateTime DateTimeAdd(const DateTime& dateTime, int h, int m, int s, int ms)
	{
		if (dateTime.Month == 0)
		{
			// if not a valid date consider it as a time counter
			int msSum = dateTime.Millisecond + ms;
			DateTime newDateTime;
			int sSum = dateTime.Second + s + msSum / 1000;
			int mSum = dateTime.Minute + m + sSum / 60;
			int hSum = dateTime.Hour + h + mSum / 60;
			int dSum = dateTime.Day + hSum / 24;
			newDateTime.Day = dateTime.Day + dSum;
			newDateTime.Hour = hSum % 24;
			newDateTime.Minute = mSum % 60;
			newDateTime.Second = msSum % 60;
			newDateTime.Millisecond = msSum % 1000;
			return newDateTime;
		}
		ptime time(
			date(
				(unsigned short)dateTime.Year,
				(unsigned short)dateTime.Month,
				(unsigned short)dateTime.Day),
			hours(dateTime.Hour) + minutes(dateTime.Minute) + seconds(dateTime.Second)
			+ hours(h) + minutes(m) + seconds(s) + milliseconds(ms)
			);
		return DateTimeSetTime(dateTime, time);
	}


	DateTime DateTimeAddMs(const DateTime& dateTime, long ms)
	{
		if (dateTime.Month == 0)
		{
			// if not a valid date consider it as a time counter
			long msSum = dateTime.Millisecond + ms;
			DateTime newDateTime;
			long sSum = dateTime.Second + msSum / 1000L;
			long mSum = dateTime.Minute + sSum / 60L;
			long hSum = dateTime.Hour + mSum / 60L;
			int dSum = dateTime.Day + (int)hSum / 24;
			newDateTime.Day = dateTime.Day + dSum;
			newDateTime.Hour = (int)(hSum % 24L);
			newDateTime.Minute = (int)(mSum % 60L);
			newDateTime.Second = (int)(msSum % 60L);
			newDateTime.Millisecond = (int)(msSum % 1000L);
			return newDateTime;
		}
		ptime time(
			date(
				(unsigned short)dateTime.Year,
				(unsigned short)dateTime.Month,
				(unsigned short)dateTime.Day),
			hours(dateTime.Hour) + minutes(dateTime.Minute) + seconds(dateTime.Second)
			+ milliseconds(ms)
			);
		return DateTimeSetTime(dateTime, time);
	}
}


using namespace gpvulc;

bool operator<(const DateTime& dateTime1, const DateTime& dateTime2)
{
	if (dateTime1.Month == 0 || dateTime2.Month == 0)
	{
		return DateTimeToMs(dateTime1) < DateTimeToMs(dateTime2);
	}
	ptime t1 = DateTimeToTime(dateTime1);
	ptime t2 = DateTimeToTime(dateTime2);
	return t1 < t2;
}


//bool operator<(DateTime& dateTime1, DateTime& dateTime2)
//{
//	return const_cast<const DateTime&>(dateTime1) < const_cast<const DateTime&>(dateTime2);
//}


bool operator==(const DateTime& dateTime1, const DateTime& dateTime2)
{
	if (dateTime1.Month == 0 || dateTime2.Month == 0)
	{
		return DateTimeToMs(dateTime1) == DateTimeToMs(dateTime2);
	}
	ptime t1 = DateTimeToTime(dateTime1);
	ptime t2 = DateTimeToTime(dateTime2);
	return t1 == t2;
}


//bool operator==(DateTime& dateTime1, DateTime& dateTime2)
//{
//	return const_cast<const DateTime&>(dateTime1) == const_cast<const DateTime&>(dateTime2);
//}


bool operator>(const DateTime& dateTime1, const DateTime& dateTime2)
{
	if (dateTime1.Month == 0 || dateTime2.Month == 0)
	{
		return DateTimeToMs(dateTime1) > DateTimeToMs(dateTime2);
	}
	ptime t1 = DateTimeToTime(dateTime1);
	ptime t2 = DateTimeToTime(dateTime2);
	bool isLess = t1 < t2;
	bool isEqual = t1 == t2;
	return !(isLess || isEqual);
}
//bool operator>(DateTime& dateTime1, DateTime& dateTime2)
//{
//	return const_cast<const DateTime&>(dateTime1) > const_cast<const DateTime&>(dateTime2);
//}


bool operator<=(const DateTime& dateTime1, const DateTime& dateTime2)
{
	if (dateTime1.Month == 0 || dateTime2.Month == 0)
	{
		return DateTimeToMs(dateTime1) <= DateTimeToMs(dateTime2);
	}
	ptime t1 = DateTimeToTime(dateTime1);
	ptime t2 = DateTimeToTime(dateTime2);
	bool isLess = t1 < t2;
	bool isEqual = t1 == t2;
	return isLess || isEqual;
}
//bool operator<=(DateTime& dateTime1, DateTime& dateTime2)
//{
//	return const_cast<const DateTime&>(dateTime1) <= const_cast<const DateTime&>(dateTime2);
//}


bool operator>=(const DateTime& dateTime1, const DateTime& dateTime2)
{
	if (dateTime1.Month == 0 || dateTime2.Month == 0)
	{
		return DateTimeToMs(dateTime1) >= DateTimeToMs(dateTime2);
	}
	ptime t1 = DateTimeToTime(dateTime1);
	ptime t2 = DateTimeToTime(dateTime2);
	return !(t1 < t2);
}
//bool operator>=(DateTime& dateTime1, DateTime& dateTime2)
//{
//	return const_cast<const DateTime&>(dateTime1) >= const_cast<const DateTime&>(dateTime2);
//}


/*
inline std::time_t GetTimeZoneOffset()
{
std::time_t now = { 0 };
std::time_t local = std::mktime(std::localtime(&now));
std::time_t gmt = std::mktime(std::gmtime(&now));
return gmt - local;
}


std::tm DateTimeToTm(const DateTime& dateTime)
{
std::tm t = { 0 };
t.tm_year = dateTime.Year;
t.tm_mon = dateTime.Month;
t.tm_mday = dateTime.Day;
t.tm_hour = dateTime.Hour;
t.tm_min = dateTime.Minute;
t.tm_sec = dateTime.Second;
return t;
}


std::time_t DateTimeToTime(const DateTime& dateTime)
{
std::tm timeInfo = DateTimeToTm(dateTime);

return mktime ( &timeInfo );
}


DateTime TmToDateTime(const std::tm& t)
{
DateTime dateTime;
dateTime.Year = t.tm_year;
dateTime.Month = t.tm_mon;
dateTime.Day = t.tm_mday;
dateTime.Hour = t.tm_hour;
dateTime.Minute = t.tm_min;
dateTime.Second = t.tm_sec;
return dateTime;
}


void SetTimeZoneOffset(DateTime& dateTime, std::time_t timeZoneOffset)
{
int timeOffsetHour;
int timeOffsetMinute;
long timeOffsetSec = static_cast<long> (timeZoneOffset);
dateTime.TimeOffsetHour = timeOffsetSec / 3600;
dateTime.TimeOffsetMinute = timeOffsetSec / 60;
}

void gpvulc::DateTimeToLocal(DateTime& dateTime)
{
std::time_t timeZoneOffset = GetTimeZoneOffset();
std::time_t dtOffset = dateTime.TimeOffsetHour * 3600 + dateTime.TimeOffsetMinute * 60;
std::time_t t = DateTimeToTime(dateTime) + timeZoneOffset + dtOffset;
std::tm timeInfo = *(std::localtime(&t));
dateTime = TmToDateTime(timeInfo);
SetTimeZoneOffset(dateTime,timeZoneOffset);
}


void gpvulc::DateTimeToUTC(DateTime& dateTime)
{
std::time_t timeZoneOffset = GetTimeZoneOffset();
std::time_t dtOffset = dateTime.TimeOffsetHour * 3600 + dateTime.TimeOffsetMinute * 60;
std::time_t t = DateTimeToTime(dateTime) + timeZoneOffset + dtOffset;
std::tm timeInfo = *(std::gmtime(&t));
dateTime = TmToDateTime(timeInfo);
SetTimeZoneOffset(dateTime,timeZoneOffset);
}

std::time_t DateTimeToTime(DateTime& dateTime)
{
std::time_t timeZoneOffset = GetTimeZoneOffset();
std::time_t dtOffset = dateTime.TimeOffsetHour * 3600 + dateTime.TimeOffsetMinute * 60;
std::time_t t = DateTimeToTime(dateTime) + timeZoneOffset + dtOffset;
return mktime ( std::gmtime(&t) );
}

std::time_t DateTimeToUtcTime(DateTime& dateTime)
{
std::time_t timeZoneOffset = GetTimeZoneOffset();
std::time_t dtOffset = dateTime.TimeOffsetHour * 3600 + dateTime.TimeOffsetMinute * 60;
std::time_t t = DateTimeToTime(dateTime) + timeZoneOffset + dtOffset;
return mktime ( std::gmtime(&t) );
}

long gpvulc::DateTimeDistanceMs(const DateTime& dateTime1, const DateTime& dateTime2)
{
return 0;
}


//DateTime operator-(const DateTime& dateTime1, const DateTime& dateTime2)
//{
//	DateTime dateTime;
//	struct tm t = { 0 };
//
//	return dateTime;
//}
*/