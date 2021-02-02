//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <gpvulc/console/MessageLogger.h>

#include <iostream>
#include <sstream>

namespace gpvulc
{
	std::shared_ptr<MessageLogger> MessageLogger::GlobalLogger;

	std::shared_ptr<MessageLogger> MessageLogger::GetGlobalLogger()
	{
		if (!GlobalLogger)
		{
			GlobalLogger = std::shared_ptr<MessageLogger>(new MessageLogger());
		}
		return GlobalLogger;
	}

	void MessageLogger::LogMessage(
		int severity, const std::string& message, const std::string& category,
		bool toConsole, bool toScreen, const std::string& msgRef)
	{
		toConsole = toConsole && IsConsoleEnabled;
		toScreen = toScreen && IsScreenEnabled;

		if (DisplayMessage)
		{
			DisplayMessage(severity, message, category, toConsole, toScreen, msgRef);
		}
		else
		{
			if (severity >= LOG_WARNING)
			{
				std::string prefix;
				switch (severity)
				{
				case LOG_WARNING:
					prefix = "Warning!";
					break;
				case LOG_ERROR:
					prefix = "ERROR!";
					break;
				case LOG_FATAL:
					prefix = "FATAL!";
					break;
				default:
					break;
				}

				// TODO: replace standard streams with a shared string buffer?
				//ErrorList += prefix + " [" + category + "] " + message + "\n";

				std::cerr << prefix << " [" << category << "] " << message << std::endl;
			}
			else
			{
				std::cout << "[" << category << "] " << message << std::endl;
			}
		}
	}

}