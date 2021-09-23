//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2019-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

// rapidJSON
#define RAPIDJSON_HAS_STDSTRING 1 // Enable RapidJSON support for std::string


#define RAPIDJSON_ASSERT(x) \
	if (!(x)) { throw gpvulc::json::FormatException(#x); }

#define RAPIDJSON_PARSE_ERROR_NORETURN(parseErrorCode,offset) \
    throw gpvulc::json::ParseException(parseErrorCode, #parseErrorCode, offset)

#include <rapidjson/error/error.h> // rapidjson::ParseResult, rapidjson::kParseErrorNone

#include <stdexcept>
#include <string>

namespace gpvulc
{
	namespace json
	{
		/*!
		Exception thrown when parsing data.
		*/
		struct ParseException : std::runtime_error, rapidjson::ParseResult
		{
			ParseException(rapidjson::ParseErrorCode code, const char* msg, size_t offset)
				: std::runtime_error(msg), ParseResult(code, offset) {}
		};


		/*!
		Exception thrown if the JSON document has a wrong format.
		*/
		struct FormatException : std::runtime_error
		{
			FormatException()
				: std::runtime_error("invalid JSON") {}
			FormatException(const char* description)
				: std::runtime_error(description) {}
		};

		/*!
		Exception thrown when the content is wrong.
		*/
		struct ContentException : std::runtime_error
		{
			ContentException()
				: std::runtime_error("invalid JSON content") {}
			ContentException(const char* description)
				: std::runtime_error(description) {}
		};


		inline std::string GetParseExceptionErrorMessage(const ParseException& parseException)
		{
			std::string errMsg = "";
			if (parseException.Code() == rapidjson::kParseErrorNone)
			{
				errMsg = "";
			}
			else
			{
				std::string errDescr(parseException.what());
				errMsg = "parsing error (" + errDescr.substr(11) + ") at byte " + std::to_string(parseException.Offset()); // remove "kParseError"
			}
			return errMsg;
		}
	}
}
