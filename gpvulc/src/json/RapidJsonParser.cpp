//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2019-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once 

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <gpvulc/json/RapidJsonParser.h>

namespace gpvulc
{
	namespace json
	{

		RapidJsonParser::RapidJsonParser()
		{
		}


		bool RapidJsonParser::HasRootElement(const char* name)
		{
			return DocumentBuffer.HasMember(name);
		}


		const rapidjson::Value& RapidJsonParser::GetRootElement(const char* name)
		{
			if (!DocumentBuffer.HasMember(name))
			{
				AddJsonError(ErrorType::MISSING_ROOT, name);
				throw ContentException(GetJsonErrorSummary(true));
			}

			return DocumentBuffer[name];
		}


		bool RapidJsonParser::ErrorsOccurred()
		{
			return !ErrorList.empty();
		}


		void RapidJsonParser::StartContext(const char* name)
		{
			PathToMember.push_back(name);
		}


		void RapidJsonParser::StartContext(const std::string& name)
		{
			PathToMember.push_back(name);
		}


		void RapidJsonParser::EndContext()
		{
			PathToMember.pop_back();
		}


		void RapidJsonParser::AddJsonError(ErrorType errorType, const std::string& elementName)
		{
			ErrorList.push_back({ GetPathToMember(), errorType, elementName });
		}

		const char* RapidJsonParser::GetJsonErrorSummary(bool notNull)
		{
			static std::string errorSummary;
			std::ostringstream oStr;
			for (size_t i = 0; i < ErrorList.size(); i++)
			{
				if (!ErrorList[i].Context.empty())
				{
					oStr << ErrorList[i].Context << ": ";
				}
				oStr << ErrorTypeToString(ErrorList[i].Type) << ErrorList[i].Element << "\n";
			}
			errorSummary = oStr.str();
			if (errorSummary.empty())
			{
				return notNull ? "" : nullptr;
			}
			return errorSummary.c_str();
		}


		bool RapidJsonParser::CheckHasMember(const rapidjson::Value& val, const char* name, bool optional)
		{
			if (!val.IsObject())
			{
				AddJsonError(ErrorType::NOT_OBJECT);
				return "";
			}
			if (!val.HasMember(name))
			{
				if (!optional)
				{
					AddJsonError(ErrorType::MISSING_MEMBER, name);
				}
				return false;
			}
			return true;
		}


		bool RapidJsonParser::CheckHasMembers(
			const rapidjson::Value& val,
			const std::vector<const char*>& membersNames
			)
		{
			if (!val.IsObject())
			{
				AddJsonError(ErrorType::NOT_OBJECT);
				return false;
			}
			bool missing = false;
			for (const char* memberName : membersNames)
			{
				if (!val.HasMember(memberName))
				{
					AddJsonError(ErrorType::MISSING_MEMBER, memberName);
					missing = true;
				}
			}
			return !missing;
		}


		bool RapidJsonParser::CheckIsObject(const rapidjson::Value& val)
		{
			if (!val.IsObject())
			{
				AddJsonError(ErrorType::NOT_OBJECT);
				return false;
			}
			return true;
		}


		bool RapidJsonParser::CheckIsArray(const rapidjson::Value& val)
		{
			if (!val.IsArray())
			{
				AddJsonError(ErrorType::NOT_ARRAY);
				return false;
			}
			return true;
		}


		const char* RapidJsonParser::GetString(const rapidjson::Value& val, const char* context)
		{
			if (!val.IsString())
			{
				AddJsonError(ErrorType::WRONG_TYPE, context);
				return "";
			}
			return val.GetString();
		}


		int RapidJsonParser::GetInt(const rapidjson::Value& val, const char * context)
		{
			if (!val.IsInt())
			{
				AddJsonError(ErrorType::WRONG_TYPE, context);
				return 0;
			}
			return val.GetInt();
		}


		bool RapidJsonParser::GetBool(const rapidjson::Value& val, const char * context)
		{
			if (!val.IsBool())
			{
				AddJsonError(ErrorType::WRONG_TYPE, context);
				return false;
			}
			return val.GetBool();
		}


		float RapidJsonParser::GetFloat(const rapidjson::Value& val, const char * context)
		{
			if (!val.IsFloat())
			{
				AddJsonError(ErrorType::WRONG_TYPE, context);
				return 0;
			}
			return val.GetFloat();
		}


		const char* RapidJsonParser::GetAsString(
			const rapidjson::Value& val,
			const char* name,
			bool optional
			)
		{
			if (!val.IsObject())
			{
				AddJsonError(ErrorType::NOT_OBJECT);
				return "";
			}
			if (!val.HasMember(name))
			{
				if (!optional)
				{
					AddJsonError(ErrorType::MISSING_MEMBER, name);
				}
				return "";
			}
			if (!val[name].IsString())
			{
				AddJsonError(ErrorType::WRONG_TYPE, name);
				return "";
			}
			return val[name].GetString();
		}


		int RapidJsonParser::GetAsInt(
			const rapidjson::Value& val,
			const char* name,
			bool optional,
			int defaultValue
			)
		{
			if (!val.IsObject())
			{
				AddJsonError(ErrorType::NOT_OBJECT);
				return defaultValue;
			}
			if (!val.HasMember(name))
			{
				if (!optional)
				{
					AddJsonError(ErrorType::MISSING_MEMBER, name);
				}
				return defaultValue;
			}
			if (!val[name].IsInt())
			{
				AddJsonError(ErrorType::WRONG_TYPE, name);
				return defaultValue;
			}
			return val[name].GetInt();
		}


		bool RapidJsonParser::GetAsBool(const rapidjson::Value& val, const char* name, bool optional, bool defaultValue)
		{
			if (!val.IsObject())
			{
				AddJsonError(ErrorType::NOT_OBJECT);
				return "";
			}
			if (!val.HasMember(name))
			{
				if (!optional)
				{
					AddJsonError(ErrorType::MISSING_MEMBER, name);
				}
				return defaultValue;
			}
			if (!val[name].IsBool())
			{
				AddJsonError(ErrorType::WRONG_TYPE, name);
				return defaultValue;
			}
			return val[name].GetBool();
		}


		float RapidJsonParser::GetAsFloat(
			const rapidjson::Value& val,
			const char* name,
			bool optional,
			float defaultValue
			)
		{
			if (!val.IsObject())
			{
				AddJsonError(ErrorType::NOT_OBJECT);
				return defaultValue;
			}
			if (!val.HasMember(name))
			{
				if (!optional)
				{
					AddJsonError(ErrorType::MISSING_MEMBER, name);
				}
				return defaultValue;
			}
			if (!val[name].IsFloat())
			{
				AddJsonError(ErrorType::WRONG_TYPE, name);
				return defaultValue;
			}
			return val[name].GetFloat();
		}


		std::string RapidJsonParser::GetPathToMember()
		{
			std::string path;
			for (size_t i = 0; i < PathToMember.size(); i++)
			{
				if (i > 0)
				{
					path += "/";
				}
				path += PathToMember[i];
			}
			return path;
		}


		void RapidJsonParser::Reset()
		{
			DocumentBuffer.Clear();
			PathToMember.clear();
			ErrorList.clear();
		}

		void RapidJsonParser::Parse(const std::string& jsonText)
		{
			//Reset();
			DocumentBuffer.Parse(jsonText);
		}


		void RapidJsonParser::CheckJsonErrors()
		{
			if (ErrorsOccurred())
			{
				throw ContentException(GetJsonErrorSummary(true));
			}
		}
	}
}