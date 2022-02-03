//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// ConsoleMessage.cpp Messaging functions: errors, warnings, info...

#include <gpvulc/ds/util/ConsoleMessage.h>

#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <vector>

namespace gpvulc
{

	//**************************************************************************
	//                          NOTIFY
	//**************************************************************************

#ifdef _DEBUG
	static int gNotifyLevel = LOG_DEBUG;
#else
	static int gNotifyLevel = LOG_NOTICE;
#endif

	// Notify an event writing a message to the console.
	// The first parameter is the level of attention and can be:
	// LOG_FATAL, LOG_WARN, LOG_NOTICE, LOG_INFO, LOG_DEBUG
	void NotifyMsg(int lev, const char *fmt, ...)
	{
		va_list args;

		if (fmt == nullptr)
			return;

		if (lev > gNotifyLevel)
			return;

		switch (lev)
		{
		case LOG_FATAL:
			printf("FATAL!!! ");
			break;
		case LOG_WARN:
			printf("Warning! ");
			break;
		case LOG_NOTICE:
			//printf("* ");
			break;
		case LOG_INFO:
			//printf("> ");
			break;
		case LOG_DEBUG:
			//printf("    DEBUG --> ");
			break;
		case LOG_VERBOSE:
			//printf("    DEBUG --> ");
			break;
		}

		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);

		// ensure output is written before the application terminates
		if (lev == LOG_FATAL) { fflush(stdout); fflush(stderr); }

		// Add final newline
	//    putchar('\n');
	}


	void LogMsg(int lev, const char *file_name, const char *fmt, ...)
	{
		va_list args;

		if (fmt == nullptr || file_name == nullptr)
			return;

		if (lev > gNotifyLevel)
			return;

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif
		FILE *fpdebug;
		if ((fpdebug = fopen(file_name, "a")) != nullptr)
		{
			va_start(args, fmt);
			vfprintf(fpdebug, fmt, args);
			va_end(args);
			fclose(fpdebug);
		}
#ifdef _MSC_VER
#pragma warning( pop )
#endif
	}


	void SetNotifyLevel(int lev)
	{
		if (lev >= 0 || lev <= LOG_VERBOSE)
			gNotifyLevel = lev;
	}


	int GetNotifyLevel()
	{
		return gNotifyLevel;
	}


	bool CheckNotifyLevel(int lev)
	{
		return lev <= gNotifyLevel;
	}

}


