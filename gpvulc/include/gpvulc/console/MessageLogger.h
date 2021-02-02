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

#include <memory>
#include <functional>
#include <string>

namespace gpvulc
{
	/*!
	Message log severity level.
	*/
	enum LogLevel
	{
		LOG_DEBUG = 0, LOG_VERBOSE, LOG, LOG_WARNING, LOG_ERROR, LOG_FATAL
	};

	/*!
	Logger for displaying messages to a generic user interface.
	DisplayMessage function must be set according to the specific implementation.
	If no DisplayMessage function is provided the standard output will be used.
	*/
	struct MessageLogger
	{
		/*!
		Callback function used to display log messages.
		*/
		std::function<void(
			int severity,
			const std::string& message,
			const std::string& category,
			bool onConsole,
			bool onScreen,
			const std::string& msgTag)>	DisplayMessage;

		/*!
		Obtain a shared pointer to a global message logger.
		*/
		static std::shared_ptr<MessageLogger> GetGlobalLogger();

		/*!
		Check if the DisplayMessage callback is defined.
		*/
		bool DisplayMessageDefined() const { return DisplayMessage != nullptr; }

		/*!
		Log a message using the provided callback function (or the standard output if missing).
		@param severity Severity level (see LogLevel).
		@param message Text of the message to be displayed.
		@param category Category name, used to identify the source of the message.
		@param toConsole Display the message on console (when available).
		@param toScreen Display the message on screen (when available).
		@param msgRef Reference identifier used to update existing messages instead of creating a new one.
		*/
		void LogMessage(
			int severity,
			const std::string& message,
			const std::string& category,
			bool toConsole,
			bool toScreen,
			const std::string& msgRef = "");

		/*!
		Log a message using the provided callback function on default output channels (or the standard output if missing).
		@param severity Severity level (see LogLevel).
		@param message Text of the message to be displayed.
		@param category Category name, used to identify the source of the message.
		@see SetDefaultOutput(), MessageLogger::LogMessage(int,const std::string&,const std::string&,bool,bool,const std::string&)
		*/
		void LogMessage(
			int severity,
			const std::string& message,
			const std::string& category
		);


		/*!
		Set the default output to console (default=true) and/or to screen (default=false).
		@param toConsole Display the message on console (when available).
		@param toScreen Display the message on screen (when available).
		*/
		void SetDefaultOutput(bool toConsole = true, bool toScreen = false);


		/*!
		Enable the output to console (default=true) and/or to screen (default=true).
		@param toConsole Display the message on console (when available).
		@param toScreen Display the message on screen (when available).
		*/
		void EnableOutput(bool toConsole = true, bool toScreen = false);

	private:
		static std::shared_ptr<MessageLogger> GlobalLogger;

		bool DefaultToConsole = true;
		bool DefaultToScreen = false;
		bool IsConsoleEnabled = true;
		bool IsScreenEnabled = true;
	};


	inline void MessageLogger::LogMessage(
		int severity,
		const std::string& message,
		const std::string& category
	)
	{
		LogMessage(severity, message, category, DefaultToConsole, DefaultToScreen, "");
	}


	inline void MessageLogger::SetDefaultOutput(bool toConsole, bool toScreen)
	{
		DefaultToConsole = toConsole;
		DefaultToScreen = toScreen;
	}


	inline void MessageLogger::EnableOutput(bool toConsole, bool toScreen)
	{
		IsConsoleEnabled = toConsole;
		IsScreenEnabled = toScreen;
	}


	inline std::shared_ptr<MessageLogger> GetGlobalLogger()
	{
		return MessageLogger::GetGlobalLogger();
	}


	/*!
	Log a message using the global MessageLogger.
	@param severity Severity level (see LogLevel).
	@param message Text of the message to be displayed.
	@param category Category name, used to identify the source of the message.
	@param toConsole Display the message on console (when available).
	@param toScreen Display the message on screen (when available).
	@param msgRef Reference identifier used to update existing messages instead of creating a new one.
	*/
	inline void LogMessage(
		int severity,
		const std::string& message,
		const std::string& category,
		bool toConsole,
		bool toScreen,
		const std::string& msgRef = "")
	{
		MessageLogger::GetGlobalLogger()->LogMessage(
			severity, message, category, toConsole, toScreen, msgRef);
	}


	/*!
	Log a message using the global MessageLogger.
	@param severity Severity level (see LogLevel).
	@param message Text of the message to be displayed.
	@param category Category name, used to identify the source of the message.
	@see SetDefaultOutput(), LogMessage(int,const std::string&,const std::string&,bool,bool,const std::string&)
	*/
	inline void LogMessage(
		int severity,
		const std::string& message,
		const std::string& category)
	{
		MessageLogger::GetGlobalLogger()->LogMessage(severity, message, category);
	}

}
