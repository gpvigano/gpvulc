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
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <string>

namespace gpvulc
{
	namespace json
	{

		/*!
		JSON document writer based on rapidjson library (https://github.com/miloyip/rapidjson/).

		A StringBuffer and a PrettyWriter (provided by rapidjson library)
		are used to build the document that can be copied to a string
		with the CopyDocument() method.

		@note This class is intended to be derived with a proper "Write" method
		using the provided methods to build the JSON document.
		@note Any call to StartDocument() must be matched by a call to EndDocument();
		any call to StartObject() must be matched by a call to EndObject();
		any call to StartArray() must be matched by a call to EndArray().
		*/
		class RapidJsonWriter
		{
		public:

			//! Default constructor.
			RapidJsonWriter();

			//! Delete document content.
			void ResetDocument();

			//! Copy the JSON document to a string, called by EndDocument().
			void CopyDocument(std::string& jsonText);

			//! Set the maximum number of decimal digits for the next floating point numbers.
			void SetDecimalPrecision(int numDigits);

			//! Start a new document, any previous content is removed.
			void StartDocument();

			//! Copy the JSON document to the given string and reset its content.
			void EndDocument(std::string& jsonText);

			//! Start a new object, if a memberName is not null a key is previously created with that name.
			void StartObject(const char* memberName = nullptr);

			//! End the object created with StartObject().
			void EndObject();

			//! Start a new array, if a memberName is not null a key is previously created with that name.
			void StartArray(const char* memberName = nullptr);

			//! End the array created with StartArray().
			void EndArray();

			/// Write a pair (key, value) with the given member name and value
			/// @name Writer methods for primitive types
			/// @{

			//! Write a boolean element
			void WriteBool(const char* memberName, bool boolValue);

			//! Write an integer element
			void WriteInt(const char* memberName, int intValue);

			//! Write a floating point number element (see SetDecimalPrecision())
			void WriteFloat(const char* memberName, float floatValue);
			
			//! Write a double precision number element (see SetDecimalPrecision())
			void WriteDouble(const char* memberName, double doubleValue);

			//! Write a string element
			void WriteString(const char* memberName, const std::string& stringValue, bool skipIfEmpty = false);

			/// @}

		protected:

			//! Internal document buffer
			rapidjson::StringBuffer Buffer;

			//! Internal document writer
			rapidjson::PrettyWriter<rapidjson::StringBuffer> Writer;

		};

	}
}
