//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2019-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <gpvulc/json/RapidJsonWriter.h>

namespace gpvulc
{
	namespace json
	{
		using namespace gpvulc;
		using namespace rapidjson;

		RapidJsonWriter::RapidJsonWriter()
			: Writer(Buffer)
		{
			Buffer.Clear();
			Writer.SetMaxDecimalPlaces(5);
		}


		void RapidJsonWriter::ResetDocument()
		{
			Buffer.Clear();
		}


		void RapidJsonWriter::CopyDocument(std::string& jsonText)
		{
			jsonText = Buffer.GetString();
		}

		void RapidJsonWriter::SetDecimalPrecision(int numDigits)
		{
			Writer.SetMaxDecimalPlaces(numDigits);
		}


		void RapidJsonWriter::StartDocument()
		{
			ResetDocument();
			//SetDecimalPrecision(5);

			StartObject();
		}


		void RapidJsonWriter::EndDocument(std::string& jsonText)
		{
			EndObject();

			CopyDocument(jsonText);
			ResetDocument();
		}


		void RapidJsonWriter::StartObject(const char* memberName)
		{
			if (memberName)
			{
				Writer.Key(memberName);
			}
			Writer.StartObject();
		}


		void RapidJsonWriter::EndObject()
		{
			Writer.EndObject();
		}


		void RapidJsonWriter::StartArray(const char* memberName)
		{
			if (memberName)
			{
				Writer.Key(memberName);
			}
			Writer.StartArray();
		}


		void RapidJsonWriter::EndArray()
		{
			Writer.EndArray();
		}


		void RapidJsonWriter::WriteBool(const char* memberName, bool boolValue)
		{
			if (memberName)
			{
				Writer.Key(memberName);
			}
			Writer.Bool(boolValue);
		}


		void RapidJsonWriter::WriteInt(const char* memberName, int intValue)
		{
			if (memberName)
			{
				Writer.Key(memberName);
			}
			Writer.Int(intValue);
		}


		void RapidJsonWriter::WriteFloat(const char* memberName, float floatValue)
		{
			if (memberName)
			{
				Writer.Key(memberName);
			}
			Writer.Double(floatValue);
		}


		void RapidJsonWriter::WriteDouble(const char* memberName, double doubleValue)
		{
			if (memberName)
			{
				Writer.Key(memberName);
			}
			Writer.Double(doubleValue);
		}


		void RapidJsonWriter::WriteString(const char* memberName, const std::string& stringValue, bool skipIfEmpty)
		{
			if (skipIfEmpty && stringValue.empty())
			{
				return;
			}
			if (memberName)
			{
				Writer.Key(memberName);
			}
			Writer.String(stringValue.c_str());
		}


	}
}
