//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2019-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once


#include "RapidJsonInclude.h" // Macro definitions for rapidjson

#include <rapidjson/document.h>     // rapidjson's DOM-style API

#include <string>
#include <vector>

namespace gpvulc
{
	namespace json
	{
		//! Type of error.
		enum class ErrorType
		{
			//! The requested root element missing.
			MISSING_ROOT,
			//! The requested member missing.
			MISSING_MEMBER,
			//! The requested member has a different type.
			WRONG_TYPE,
			//! The requested member is not an object.
			NOT_OBJECT,
			//! The requested member is not an array.
			NOT_ARRAY,
			//! The requested member has a dangling reference.
			DANGLING_REFERENCE,
			//! The requested member has an invalid value.
			INVALID_VALUE,
			//! An unhandled exception was thrown.
			UNHANDLED_EXCEPTION,
		};

		//! Error information record
		struct ErrorInfo
		{
			//! Identifier of the context where the error occurred.
			std::string Context;
			//! Type of error.
			ErrorType Type;
			//! Identifier of the element where the error occurred.
			std::string Element;
		};

		//! Convert an ErrorType to a string.
		inline std::string ErrorTypeToString(ErrorType errType)
		{
			switch (errType)
			{
			case ErrorType::MISSING_ROOT:
				return "Missing root element ";
			case ErrorType::MISSING_MEMBER:
				return "Missing member ";
			case ErrorType::WRONG_TYPE:
				return "Wrong type for ";
			case ErrorType::NOT_OBJECT:
				return "Not an object";
			case ErrorType::NOT_ARRAY:
				return "Not an array";
			case ErrorType::DANGLING_REFERENCE:
				return "Dangling reference in ";
			case ErrorType::INVALID_VALUE:
				return "Invalid value for ";
			case ErrorType::UNHANDLED_EXCEPTION:
				return "Unhandled exception: ";
				//default:
				//	return "Unknown error ";
			}
			return "";
		}

		/*!
		JSON parser based on rapidjson library (https://github.com/miloyip/rapidjson/).

		Instead of throwing an exception on each error,
		errors are collected, the parsing continues
		and an exception is thrown at the end, calling CheckJsonErrors(),
		if errors occurred.
		@c Get methods call AddJsonError() and return a value instead of throwing.
		StartContext()/EndContext() methods must be used to get a meaningful
		error summary if parsing errors occur.
		@note Any call to StartContext() must be matched by a a call to EndContext().
		*/
		class RapidJsonParser
		{
		public:

			//! Default contructor
			RapidJsonParser();

			//! Check if the parsed document has the named root element.
			bool HasRootElement(const char* name);

			//! Get the named root element of the parsed document.
			const rapidjson::Value& GetRootElement(const char* name);
			
			//! Chech if at least one error occurred.
			bool ErrorsOccurred();

			//! Start a named parsing context, used to get a meaningful error summary.
			void StartContext(const char* name);

			//! Start a named parsing context, used to get a meaningful error summary.
			void StartContext(const std::string& name);
			
			//! Start a numbered parsing context (e.g. for arrays), used to get a meaningful error summary.
			void StartContext(size_t index)
			{
				StartContext(std::to_string(index));
			}


			//! End the last parsing context, used to get a meaningful error summary.
			void EndContext();

			/*!
			Build and return a (static) string with an error summary.
			@note The returned string is reallocated at each call,
			so you should copy its value soon after calling this method.
			*/
			const char* GetJsonErrorSummary(bool notNull = false);

			/*!
			Check if the given Value has the named member, if not optional an error is registered.
			@param val input value
			@param name member name
			@param optional if false (default) and the member is missing an error is registered.
			*/
			bool CheckHasMember(const rapidjson::Value& val, const char* name, bool optional = false);

			//! Check if the given Value has the list of named members.
			bool CheckHasMembers(const rapidjson::Value& val, const std::vector<const char*>& membersNames);

			//! Check if the given Value is an object.
			bool CheckIsObject(const rapidjson::Value& val);

			//! Check if the given Value is an array.
			bool CheckIsArray(const rapidjson::Value& val);
			
			/*!
			Check if the given Value has the named member and it is an array.
			@param val input value
			@param name member name
			@param optional if false (default) and the member is missing or it is not an array an error is registered.
			*/
			bool CheckHasArray(const rapidjson::Value& val, const char* name, bool optional = false)
			{
				return CheckHasMember(val, name, optional) &&CheckIsArray(val[name]);
			}

			/// @note An optional context can be provided for these methods,
			/// used for error messages if available.
			/// @name Getters
			/// @{

			//! Get the given Value as a string ().
			const char* GetString(const rapidjson::Value& val, const char* context = "");

			//! Get the given Value as an integer value (an optional context can be provided for error messages).
			int GetInt(const rapidjson::Value& val, const char* context = "");

			//! Get the given Value as a boolean value (an optional context can be provided for error messages).
			bool GetBool(const rapidjson::Value& val, const char* context = "");

			//! Get the given Value as a floating point value (an optional context can be provided for error messages).
			float GetFloat(const rapidjson::Value& val, const char* context = "");

			/// @}

			/// Member Getters
			/// @note if @c optional parameter is false (default) and the member is missing
			/// or it is of a different type an error is registered.
			/// @name MemberGetters
			/// @{

			//! Get the named member of the given Value as a string.
			const char* GetAsString(const rapidjson::Value& val, const char* name, bool optional = false);

			//! Get the named member of the given Value as an integer value.
			int GetAsInt(const rapidjson::Value& val, const char* name, bool optional = false, int defaultValue = 0);

			//! Get the named member of the given Value as a boolean value.
			bool GetAsBool(const rapidjson::Value& val, const char* name, bool optional = false, bool defaultValue = false);

			//! Get the named member of the given Value as a floating point value.
			float GetAsFloat(const rapidjson::Value& val, const char* name, bool optional = false, float defaultValue = 0.0f);

			/// @}

			/*!
			Reset the internal document and clear errors.
			*/
			void Reset();

			//! Parse a JSON string into DOM data.
			void Parse(const std::string& jsonText);

			/*!
			Check for errors and, if any, throw an exception with an error summary.
			*/
			void CheckJsonErrors();

		protected:

			//! Internal document buffer
			rapidjson::Document DocumentBuffer;

			//! Add an error related to the last parsing context, called by @c Get methods.
			void AddJsonError(ErrorType errorType, const std::string& elementName = "");

		private:

			std::vector<std::string> PathToMember;
			std::vector<ErrorInfo> ErrorList;

			std::string GetPathToMember();
		};
	}
}