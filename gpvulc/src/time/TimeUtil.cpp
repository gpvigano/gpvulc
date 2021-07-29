//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Time utility


#include <gpvulc/time/TimeUtil.h>

#include <chrono>
#include <ctime>
#include <thread>
#include <sstream>
#include <iomanip>


namespace gpvulc
{

	long long GetRunTimeMicroseconds()
	{
		static std::chrono::time_point<std::chrono::system_clock> beginning;

		static bool done = false;
		if (!done)
		{
			beginning = std::chrono::system_clock::now();
			done = true;
		}
		auto diff = std::chrono::system_clock::now() - beginning;
		long long micros = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
		return micros;
	}


	long long GetRunTimeMilliseconds()
	{
		long long micros = GetRunTimeMicroseconds();
		long long msec = micros / 1000;
		return msec;
	}


	double GetRunTimeSeconds()
	{
		long long micros = GetRunTimeMicroseconds();
		double sec = micros / 1000000.0;
		return sec;
	}


	DateTime GetDateTimeNow()
	{
		DateTime dt;
		auto nowTp = std::chrono::system_clock::now();
		std::time_t t = std::chrono::system_clock::to_time_t(nowTp);
#ifdef _MSC_VER
		// disable the unsafe warning
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif
		struct tm * timeinfo = std::localtime(&t);
#ifdef _MSC_VER
#pragma warning( pop )
#endif
		dt.Year = timeinfo->tm_year + 1900;
		dt.Month = timeinfo->tm_mon + 1;
		dt.Day = timeinfo->tm_mday + 1;
		dt.WeekDay = timeinfo->tm_wday + 1;
		dt.Hour = timeinfo->tm_hour;
		dt.Minute = timeinfo->tm_min;
		dt.Second = timeinfo->tm_sec;

		std::time_t tt = std::mktime(timeinfo);
		auto tp = std::chrono::system_clock::from_time_t(tt);
		auto diff = nowTp - tp;
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
		dt.Millisecond = (int)millis;

		return dt;
	}


	long GetSystemTimeStamp()
	{
		DateTime dt = GetDateTimeNow();

		long id = 0L;

		id += dt.Year;
		id *= 10000L;
		id += dt.Month;
		id *= 100L;
		id += dt.Day;
		id *= 100L;
		id += dt.Hour;
		id *= 100L;
		id += dt.Minute;
		id *= 100L;
		id += dt.Second;
		id *= 1000L;
		id += dt.Millisecond;

		return id;
	}


	std::string GetSystemTimeStampID()
	{
		DateTime dt = GetDateTimeNow();

		std::ostringstream oStr;
		oStr << std::setfill('0')
			<< std::setw(4) << dt.Year
			<< std::setw(2) << dt.Month
			<< dt.Day
			<< "_"
			<< dt.Hour
			<< dt.Minute
			<< "_"
			<< dt.Second
			<< std::setw(3) << dt.Millisecond;

		return oStr.str();
	}


	TimeDuration SecondsToTimeDuration(double timeSeconds)
	{
		TimeDuration timeDuration;
		double& t = timeSeconds;

		int d = (int)t / 86400;
		int h = (int)t / 3600 - 24 * d;
		int m = (int)t / 60 - 1440 * d - 60 * h;
		int s = (int)t - 86400 * d - 3600 * h - 60 * m;
		int ms = int(t*1000.0) - int(t) * 1000;

		timeDuration.Days = d;
		timeDuration.Hours = h;
		timeDuration.Minutes = m;
		timeDuration.Seconds = s;
		timeDuration.Milliseconds = ms;

		return timeDuration;
	}



	double TimeDurationToSeconds(const TimeDuration& timeDuration)
	{
		double timeSeconds = timeDuration.Milliseconds / 1000.0
			+ timeDuration.Seconds +
			+timeDuration.Minutes * 60.0
			+ timeDuration.Hours * 3600.0
			+ timeDuration.Days * 86400.0;

		return timeSeconds;
	}

} // namespace gpvulc
