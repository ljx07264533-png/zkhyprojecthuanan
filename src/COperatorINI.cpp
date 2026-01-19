
#include "alglibrary/zkhyProjectHuaNan/COperatorINI.h"

#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>

#ifdef _WIN64

#define NOVICE_ENTER "\\"
#endif

int novice::stoi(const std::string& Str)
{
	try
	{
		return std::stoi(Str);
	}
	catch (std::exception& exc)
	{
		auto Error = std::string(exc.what());
		return 0;
	}
}

float novice::stof(const std::string& Str)
{
	try
	{
		return std::stof(Str);
	}
	catch (std::exception& exc)
	{
		auto Error = std::string(exc.what());
		return 0;
	}
}

std::string novice::to_string(int value)
{
	return std::to_string(value);
}

std::string novice::to_string(float value, int reserve)
{
	auto str = std::to_string(value);
	if (!str.empty())
	{
		auto Index = str.find('.');
		if (Index != std::string::npos)
		{
			auto Size = Index + 1 + reserve;
			if (Size < str.size())
			{
				str.resize(Size);
			}
		}
	}
	return str;
}

std::string novice::to_string(double value, int reserve)
{
	auto str = std::to_string(value);
	if (!str.empty())
	{
		auto Index = str.find('.');
		if (Index != std::string::npos)
		{
			auto Size = Index + 1 + reserve;
			if (Size < str.size())
			{
				str.resize(Size);
			}
		}
	}
	return str;
}

namespace novice
{
	struct StructData
	{
		StructData()
			: Num(0)
		{

		}
		StructData(int num, const std::string& str)
			: Num(num)
			, Str(str)
		{

		}
		int Num;
		std::string Str;
	};


	class COperatorINIPrivate
	{
	public:
		COperatorINIPrivate();
		~COperatorINIPrivate();

		static std::string GetAppPath();
		static void  DiviStr(const std::string& Str, std::vector<std::string>& VecStr);
		static bool  CreatePath(std::string FileName, bool FullPathName = false);
		static bool  IsExistence(std::string FileName, bool FullPathName = false);

		bool Attach(std::string FileName, bool FullPathName = false);

		bool IsContions(std::string Section, std::string Param);

		std::string GetParameterString(std::string Section, std::string Param);
		int GetParameterInt(std::string Section, std::string Param);
		float GetParameterFloat(std::string Section, std::string Param);

		//Param=aaa,bbb,ccc 提取到vector中。
		bool GetParameterVecString(std::string Section, std::string Param, std::vector<std::string>& vecString);

		//Param=100,101,102 提取到vector中。
		bool GetParameterVecInt(std::string Section, std::string Param, std::vector<int>& vecInt);

		//Param=20.5,40.7,60.3 提取到vector中。
		bool GetParameterVecFloat(std::string Section, std::string Param, std::vector<float>& vecFloat);

		// 试着写入
		bool SetParameterString(std::string Section, std::string Param, const std::string& String);
		bool SetParameterInt(std::string Section, std::string Param, int Value);
		bool SetParameterFloat(std::string Section, std::string Param, float Value);

		bool GetParameterString(std::string Section, std::string Param, std::string& Value);
		bool GetParameterInt(std::string Section, std::string Param, int& Value);
		bool GetParameterFloat(std::string Section, std::string Param, float& Value);

		// 表示强制写入
		bool WriteParameterString(std::string Section, std::string Param, const std::string& String);
		bool WriteParameterInt(std::string Section, std::string Param, int Value);
		bool WriteParameterFloat(std::string Section, std::string Param, float Value);

		bool WriteParamterVecString(std::string Section, std::string Param, const std::vector<std::string>& VecStr);
		bool WriteParamterVecInt(std::string Section, std::string Param, const std::vector<int>& VecInt);
		bool WriteParamterVecFloat(std::string Section, std::string Param, const std::vector<float>& VecFloat);


	private:
		void Clear();
		bool ReadFile();
		bool WriteFile();

		bool FindKey(const std::string Str, std::string& Key);
		bool FindValue(const std::string& Str, std::string& Key, std::string& Value);
		void MapFileToMapKey(const std::map<int, std::string>& MapFileStr, std::map<std::string, std::map<std::string, StructData>>& MapKeyValue);
	private:
		std::string                                                                 m_FileName;     // 文件名称
		std::map<int, std::string>                                                  m_MapFileStr;   // 文件内容
		std::map<std::string, std::map<std::string, StructData>>                    m_MapKeyValue;  // 文件分级 <Section,<Param,（num,List<Value>)>
	};
}


using namespace novice;

COperatorINIPrivate::COperatorINIPrivate()
{

}

COperatorINIPrivate::~COperatorINIPrivate()
{
	Clear();
}

void COperatorINIPrivate::Clear()
{
	m_FileName.clear();
	m_MapFileStr.clear();
	m_MapKeyValue.clear();
}

bool COperatorINIPrivate::ReadFile()
{
	if (m_FileName.empty()) return  false;
	std::ifstream inFile;
	inFile.open(m_FileName);
	if (!inFile.is_open()) return false;

#define MAX_BUFF 2048
	char Buffer[MAX_BUFF] = { 0 };

	int LineNum = 1;
	while (inFile.getline(Buffer, MAX_BUFF))
	{
		auto Str = std::string(Buffer);
		if (!Str.empty())
		{
			m_MapFileStr.insert(std::make_pair(LineNum, Str));
		}
		++LineNum;
	}

	MapFileToMapKey(m_MapFileStr, m_MapKeyValue);
	inFile.close();
	return true;
}

bool COperatorINIPrivate::WriteFile()
{
	std::string End = "\n";
	std::string strFileData;
	int num = 1;
	for (auto& ref : m_MapFileStr)
	{
		if (ref.first != num)
		{
			for (int i = num; i < ref.first; ++i)
			{
				strFileData += End;
			}
		}

		strFileData += ref.second + End;
		num = ref.first + 1;
	}

	//写入文件
	std::ofstream out;
	out.open(m_FileName);

	if (out.is_open())
	{
		out.flush();
		out << strFileData;
		out.close();
		return true;
	}
	return false;
}
#ifdef _WIN64
#include <windows.h>
std::string COperatorINIPrivate::GetAppPath()
{
#define MAX_PATH_NUM 1024
	WCHAR Path[MAX_PATH_NUM] = { 0 };
	GetModuleFileName(NULL, (LPWSTR)Path, sizeof(Path));

	wchar_t* wtext = Path;
	DWORD dwNmu = WideCharToMultiByte(CP_OEMCP, NULL, wtext, -1, NULL, 0, NULL, FALSE);
	char sTest[MAX_PATH_NUM] = { 0 };
	WideCharToMultiByte(CP_OEMCP, NULL, wtext, -1, sTest, dwNmu, NULL, FALSE);

	auto ExePathName = std::string(sTest);
	auto pos = ExePathName.find_last_of(NOVICE_ENTER);
	if (pos != ExePathName.npos)
	{
		ExePathName = ExePathName.substr(0, pos);
		return ExePathName;
	}
	return "";
}

bool COperatorINIPrivate::CreatePath(std::string FileName, bool FullPathName)
{
	if (!FullPathName)
	{
		FileName = GetAppPath() + NOVICE_ENTER + FileName;
	}

	std::ofstream out;
	out.open(FileName, std::ios_base::out | std::ios_base::trunc);
	if (out.is_open())
	{
		out.close();
		return true;
	}
	return false;
}

bool COperatorINIPrivate::IsExistence(std::string FileName, bool FullPathName)
{
	if (!FullPathName)
	{
		FileName = GetAppPath() + NOVICE_ENTER + FileName;
	}
	std::ifstream inFile;
	inFile.open(FileName);
	if (inFile.is_open())
	{
		inFile.close();
		return true;
	}
	return false;
}


#endif

bool COperatorINIPrivate::FindKey(const std::string Str, std::string& Key)
{
	if (Str.size() < 3) return false;

	if (Str[0] == '[' && Str[Str.size() - 1] == ']')
	{
		Key = Str.substr(1, Str.size() - 2);
		return true;
	};

	return false;
}

bool COperatorINIPrivate::FindValue(const std::string& Str, std::string& Key, std::string& Value)
{
	if (Str.size() < 2) return false;
	if (Str[0] == '#') return false;
	if (Str[0] == ';') return false;
	std::size_t Found = Str.find('=');
	if (Found == std::string::npos) return false;
	if (Found == 0)  return false;
	Key = Str.substr(0, Found);

	if (Found < Str.size() - 1)
	{
		Value = Str.substr(Found + 1, Str.size() - Found - 1);
	}
	else
	{
		Value = "";
	}

	if (Value.size() >= 2 && (Value[0] == '\"' && Value[Value.size() - 1] == '\"'))
	{
		if (Value.size() == 2)
		{
			Value = "";
		}
		else
		{
			Value = Value.substr(1, Value.size() - 2);
		}
	}

	return true;
}

void COperatorINIPrivate::MapFileToMapKey(const std::map<int, std::string>& MapFileStr, std::map<std::string, std::map<std::string, StructData>>& MapKeyValue)
{
	MapKeyValue.clear();
	std::string Key;
	for (auto& ref : MapFileStr)
	{
		if (FindKey(ref.second, Key))
		{
			MapKeyValue.insert(std::make_pair(Key, std::map<std::string, StructData>()));
		}
		else if (!Key.empty())
		{
			std::string KeyValue;
			std::string KeyValueStr;
			if (FindValue(ref.second, KeyValue, KeyValueStr))
			{
				auto iter = MapKeyValue.find(Key);
				if (iter != MapKeyValue.end())
				{
					iter->second.insert(std::make_pair(KeyValue, StructData(ref.first, KeyValueStr)));
				}
			}
		}
	}
}



void COperatorINIPrivate::DiviStr(const std::string& Str, std::vector<std::string>& VecStr)
{
	if (Str.empty()) return;

	std::string StrDiv = Str;

	// 判断是否“”
	if (Str.size() >= 2 && (Str[0] == '\"' && Str[Str.size() - 1] == '\"'))
	{
		if (Str.size() == 2)
		{
			StrDiv = "";
		}
		else
		{
			StrDiv = Str.substr(1, Str.size() - 2);
		}
	}

	size_t BeginPos = 0;
	while (1)
	{
		auto EndPos = StrDiv.find(",", BeginPos);
		if (EndPos == StrDiv.npos)
		{
			EndPos = StrDiv.size();
		}
		//[BeginPos,EndPos)
		if (EndPos - BeginPos > 0)
		{
			VecStr.push_back(StrDiv.substr(BeginPos, EndPos - BeginPos));
		}
		BeginPos = EndPos + 1;
		if (BeginPos >= StrDiv.size())
		{
			break;
		}
	}

}

bool COperatorINIPrivate::Attach(std::string FileName, bool FullPathName)
{
	Clear();

	if (FullPathName)
	{
		m_FileName = FileName;
	}
	else
	{
		m_FileName = GetAppPath() + NOVICE_ENTER + FileName;
	}

	return ReadFile();
}

bool COperatorINIPrivate::IsContions(std::string Section, std::string Param)
{
	auto iterKey = m_MapKeyValue.find(Section);
	if (iterKey != m_MapKeyValue.end() && iterKey->second.end() != iterKey->second.find(Param))
	{
		return true;
	}
	return false;
}


std::string COperatorINIPrivate::GetParameterString(std::string Section, std::string Param)
{
	auto iterKey = m_MapKeyValue.find(Section);
	if (iterKey != m_MapKeyValue.end())
	{
		auto iterValue = iterKey->second.find(Param);
		if (iterValue != iterKey->second.end())
		{
			return iterValue->second.Str;
		}
	}

	return "";
}

int COperatorINIPrivate::GetParameterInt(std::string Section, std::string Param)
{
	auto Str = GetParameterString(Section, Param);
	return novice::stoi(Str);
}

float COperatorINIPrivate::GetParameterFloat(std::string Section, std::string Param)
{
	auto Str = GetParameterString(Section, Param);
	return novice::stof(Str);
}


bool COperatorINIPrivate::GetParameterVecString(std::string Section, std::string Param, std::vector<std::string>& vecString)
{
	std::string  Str;
	if (GetParameterString(Section, Param, Str))
	{
		DiviStr(Str, vecString);
		return true;
	}
	return false;
}


bool COperatorINIPrivate::GetParameterVecInt(std::string Section, std::string Param, std::vector<int>& vecInt)
{
	std::string  Str;
	if (GetParameterString(Section, Param, Str))
	{
		std::vector<std::string> vecString;
		DiviStr(Str, vecString);
		vecInt.clear();
		for (auto ref : vecString)
		{
			vecInt.push_back(novice::stoi(ref));
		}
		return true;
	}

	return false;
}



bool COperatorINIPrivate::GetParameterVecFloat(std::string Section, std::string Param, std::vector<float>& vecFloat)
{
	std::string  Str;
	if (GetParameterString(Section, Param, Str))
	{
		std::vector<std::string> vecString;
		DiviStr(Str, vecString);
		vecFloat.clear();
		for (auto ref : vecString)
		{
			vecFloat.push_back(novice::stof(ref));
		}
		return true;
	}

	return false;
}

bool COperatorINIPrivate::SetParameterString(std::string Section, std::string Param, const std::string& String)
{
	//m_MapKeyValue std::map<std::string, std::map<std::string, StructData>>  
	auto iterKey = m_MapKeyValue.find(Section);
	if (iterKey != m_MapKeyValue.end())
	{
		auto iterValue = iterKey->second.find(Param);
		if (iterValue != iterKey->second.end())
		{
			iterValue->second.Str = String;
			auto iterFile = m_MapFileStr.find(iterValue->second.Num);
			if (iterFile != m_MapFileStr.end())
			{
				iterFile->second = iterValue->first + "=" + String;
				WriteFile();
				return true;
			}
		}
	}
	return false;
}


bool COperatorINIPrivate::SetParameterInt(std::string Section, std::string Param, int Value)
{
	return SetParameterString(Section, Param, novice::to_string(Value));
}


bool COperatorINIPrivate::SetParameterFloat(std::string Section, std::string Param, float Value)
{
	return SetParameterString(Section, Param, novice::to_string(Value));
}



bool COperatorINIPrivate::GetParameterString(std::string Section, std::string Param, std::string& Value)
{
	auto iterKey = m_MapKeyValue.find(Section);
	if (iterKey != m_MapKeyValue.end())
	{
		auto iterValue = iterKey->second.find(Param);
		if (iterValue != iterKey->second.end())
		{
			Value = iterValue->second.Str;
			return true;
		}
	}
	return false;
}

bool COperatorINIPrivate::GetParameterInt(std::string Section, std::string Param, int& Value)
{
	std::string Str;
	if (GetParameterString(Section, Param, Str))
	{
		Value = novice::stoi(Str);
		return true;
	}
	return false;
}

bool COperatorINIPrivate::GetParameterFloat(std::string Section, std::string Param, float& Value)
{
	std::string Str;
	if (GetParameterString(Section, Param, Str))
	{
		Value = novice::stof(Str);
		return true;
	}
	return false;
}


bool COperatorINIPrivate::WriteParameterString(std::string Section, std::string Param, const std::string& String)
{
	//m_MapKeyValue std::map<std::string, std::map<std::string, StructData>>  
	auto iterKey = m_MapKeyValue.find(Section);
	if (iterKey != m_MapKeyValue.end())
	{
		auto iterValue = iterKey->second.find(Param);
		if (iterValue != iterKey->second.end())
		{
			iterValue->second.Str = String;
			auto iterFile = m_MapFileStr.find(iterValue->second.Num);
			if (iterFile != m_MapFileStr.end())
			{
				iterFile->second = iterValue->first + "=" + String;
				WriteFile();
				return true;
			}
		}
		else
		{
			// 进行添加 param=
			// 添加两个map
			// [] 所在行(如果这个下面没有参数，就有必要)
			int MinNum = 0;
			std::string Key = "[" + Section + "]";
			for (auto& ref : m_MapFileStr)
			{
				if (ref.second == Key)
				{
					MinNum = ref.first;
					break;
				}
			}

			//肯定能找到
			if (MinNum == 0) return false;

			// 中param 最大行
			for (auto& ref : iterKey->second)
			{
				if (ref.second.Num > MinNum)
				{
					MinNum = ref.second.Num;
				}
			}

			int InsertNum = MinNum;

			for (auto& ref : m_MapFileStr)
			{
				if (ref.first > MinNum)
				{
					InsertNum = ref.first;
					break;
				}
			}

			// 什么时候能插入呢
			// 1. InsertNum == MinNum 没找到插入地方，行号都比这个小,插入最后MinNum+1
			// 2. 找到了空间也够

			//找到了(MinNum,InsertNum-1] && 无位置可以插
			if (InsertNum != MinNum && InsertNum <= MinNum + 1)
			{
				// 需要把大于MinNum 的值都加1
				std::map<int, std::string> TempMapFiler;
				for (auto& ref : m_MapFileStr)
				{
					if (ref.first > MinNum)
					{
						TempMapFiler.insert(std::make_pair(ref.first + 1, ref.second));
					}
					else
					{
						TempMapFiler.insert(ref);
					}
				}

				m_MapFileStr.clear();
				m_MapFileStr = TempMapFiler;
			}
			InsertNum = MinNum + 1;


			// 进行插入
			m_MapFileStr.insert(std::make_pair(InsertNum, Param + "=" + String));
		}
	}
	else
	{
		// 进行添加 [] Param =
		int InsertNum = 1;
		if (!m_MapFileStr.empty())
		{
			InsertNum = m_MapFileStr.rbegin()->first + 1;
		}

		// 进行插入
		m_MapFileStr.insert(std::make_pair(InsertNum++, "[" + Section + "]"));
		m_MapFileStr.insert(std::make_pair(InsertNum++, Param + "=" + String));
	}
	//进行转换
	MapFileToMapKey(m_MapFileStr, m_MapKeyValue);

	// 写入
	WriteFile();

	return true;
}

bool COperatorINIPrivate::WriteParameterInt(std::string Section, std::string Param, int Value)
{
	return WriteParameterString(Section, Param, novice::to_string(Value));
}

bool COperatorINIPrivate::WriteParameterFloat(std::string Section, std::string Param, float Value)
{
	return WriteParameterString(Section, Param, novice::to_string(Value));
}

bool COperatorINIPrivate::WriteParamterVecString(std::string Section, std::string Param, const std::vector<std::string>& VecStr)
{
	std::string Str;
	if (!VecStr.empty())
	{
		Str += VecStr.front();
	}

	for (size_t i = 1; i < VecStr.size(); ++i)
	{
		Str += "," + VecStr[i];
	}

	return WriteParameterString(Section, Param, Str);
}

bool COperatorINIPrivate::WriteParamterVecInt(std::string Section, std::string Param, const std::vector<int>& VecInt)
{
	std::vector<std::string> VecStr;
	for (auto& ref : VecInt)
	{
		VecStr.push_back(novice::to_string(ref));
	}
	return WriteParamterVecString(Section, Param, VecStr);
}

bool COperatorINIPrivate::WriteParamterVecFloat(std::string Section, std::string Param, const std::vector<float>& VecFloat)
{
	std::vector<std::string> VecStr;
	for (auto& ref : VecFloat)
	{
		VecStr.push_back(novice::to_string(ref));
	}
	return WriteParamterVecString(Section, Param, VecStr);
}



COperatorINI::COperatorINI()
	: m_pPrivate(nullptr)
{

}

COperatorINI::~COperatorINI()
{

}

std::string COperatorINI::GetAppPath()
{
	return COperatorINIPrivate::GetAppPath();
}

void COperatorINI::DiviStr(const std::string& Str, std::vector<std::string>& VecStr)
{
	COperatorINIPrivate::DiviStr(Str, VecStr);
}

bool COperatorINI::CreatePath(std::string FileName, bool FullPathName)
{
	return COperatorINIPrivate::CreatePath(FileName, FullPathName);
}

bool COperatorINI::IsExistence(std::string FileName, bool FullPathName)
{
	return COperatorINIPrivate::IsExistence(FileName, FullPathName);
}


bool COperatorINI::Attach(std::string FileName, bool FullPathName)
{
	m_pPrivate = std::make_unique<COperatorINIPrivate>();
	if (!m_pPrivate) return false;

	return m_pPrivate->Attach(FileName, FullPathName);
}



bool COperatorINI::IsContions(std::string Section, std::string Param)
{
	if (!m_pPrivate) return false;

	return m_pPrivate->IsContions(Section, Param);
}


std::string COperatorINI::GetParameterString(std::string Section, std::string Param)
{
	if (!m_pPrivate) return "";

	return m_pPrivate->GetParameterString(Section, Param);
}

int COperatorINI::GetParameterInt(std::string Section, std::string Param)
{
	if (!m_pPrivate) return 0;

	return m_pPrivate->GetParameterInt(Section, Param);
}

float COperatorINI::GetParameterFloat(std::string Section, std::string Param)
{
	if (!m_pPrivate) return 0;

	return m_pPrivate->GetParameterFloat(Section, Param);
}

double COperatorINI::GetParameterDouble(std::string Section, std::string Param)
{
	return (double)GetParameterFloat(Section, Param);
}

//Param=aaa,bbb,ccc 提取到vector中。
bool COperatorINI::GetParameterVecString(std::string Section, std::string Param, std::vector<std::string>& vecString)
{
	if (!m_pPrivate)  return false;

	return m_pPrivate->GetParameterVecString(Section, Param, vecString);
}


//Param=100,101,102 提取到vector中。
bool COperatorINI::GetParameterVecInt(std::string Section, std::string Param, std::vector<int>& vecInt)
{
	if (!m_pPrivate)  return false;

	return m_pPrivate->GetParameterVecInt(Section, Param, vecInt);
}


//Param=20.5,40.7,60.3 提取到vector中。
bool COperatorINI::GetParameterVecFloat(std::string Section, std::string Param, std::vector<float>& vecFloat)
{
	if (!m_pPrivate) return false;

	return m_pPrivate->GetParameterVecFloat(Section, Param, vecFloat);
}


bool COperatorINI::SetParameterString(std::string Section, std::string Param, const std::string& String)
{
	if (!m_pPrivate) return false;

	return m_pPrivate->SetParameterString(Section, Param, String);
}

bool COperatorINI::SetParameterInt(std::string Section, std::string Param, int Value)
{
	if (!m_pPrivate) return false;

	return m_pPrivate->SetParameterInt(Section, Param, Value);
}

bool COperatorINI::SetParameterFloat(std::string Section, std::string Param, float Value)
{
	if (!m_pPrivate) return false;

	return m_pPrivate->SetParameterFloat(Section, Param, Value);
}

bool COperatorINI::SetParameterDouble(std::string Section, std::string Param, double Value)
{
	if (!m_pPrivate) return false;

	return m_pPrivate->SetParameterFloat(Section, Param, (float)Value);
}


bool COperatorINI::GetParameterString(std::string Section, std::string Param, std::string& Value)
{
	if (!m_pPrivate) return false;
	return m_pPrivate->GetParameterString(Section, Param, Value);
}

bool COperatorINI::GetParameterInt(std::string Section, std::string Param, int& Value)
{
	if (!m_pPrivate) return false;
	return m_pPrivate->GetParameterInt(Section, Param, Value);
}

bool COperatorINI::GetParameterFloat(std::string Section, std::string Param, float& Value)
{
	if (!m_pPrivate) return false;
	return m_pPrivate->GetParameterFloat(Section, Param, Value);
}

bool COperatorINI::GetParameterDouble(std::string Section, std::string Param, double& Value)
{
	if (!m_pPrivate) return false;

	float FValue;
	auto Ok = m_pPrivate->GetParameterFloat(Section, Param, FValue);
	Value = FValue;
	return Ok;
}

bool COperatorINI::WriteParameterString(std::string Section, std::string Param, const std::string& Value)
{
	if (!m_pPrivate) return false;
	return m_pPrivate->WriteParameterString(Section, Param, Value);
}

bool COperatorINI::WriteParameterInt(std::string Section, std::string Param, int Value)
{
	if (!m_pPrivate) return false;
	return m_pPrivate->WriteParameterInt(Section, Param, Value);
}

bool COperatorINI::WriteParameterFloat(std::string Section, std::string Param, float Value)
{
	if (!m_pPrivate) return false;
	return m_pPrivate->WriteParameterFloat(Section, Param, Value);
}

bool COperatorINI::WriteParameterDouble(std::string Section, std::string Param, double Value)
{
	if (!m_pPrivate) return false;
	return m_pPrivate->WriteParameterFloat(Section, Param, (float)Value);
}

bool COperatorINI::WriteParamterVecString(std::string Section, std::string Param, const std::vector<std::string>& VecStr)
{
	if (!m_pPrivate) return false;
	return m_pPrivate->WriteParamterVecString(Section, Param, VecStr);
}

bool COperatorINI::WriteParamterVecInt(std::string Section, std::string Param, const std::vector<int>& VecInt)
{
	if (!m_pPrivate) return false;
	return m_pPrivate->WriteParamterVecInt(Section, Param, VecInt);
}

bool COperatorINI::WriteParamterVecFloat(std::string Section, std::string Param, const std::vector<float>& VecFloat)
{
	if (!m_pPrivate) return false;
	return m_pPrivate->WriteParamterVecFloat(Section, Param, VecFloat);
}

bool COperatorINI::WriteParamterVecDouble(std::string Section, std::string Param, const std::vector<double>& VecDouble)
{
	if (!m_pPrivate) return false;
	std::vector<float> VecFloat;
	for (auto& ref : VecDouble)
	{
		VecFloat.push_back((float)ref);
	}
	return m_pPrivate->WriteParamterVecFloat(Section, Param, VecFloat);
}

bool novice::ReadFile(std::string FileName, char* pBuffer, int Size, std::string& OutPut, bool FullPathName)
{
	if (!FullPathName)
	{
		FileName = COperatorINI::GetAppPath() + NOVICE_ENTER + FileName;
	}

	std::ifstream file(FileName, std::ios::binary);
	if (file)
	{
		file.seekg(0, std::ios_base::end);
		if (Size != file.tellg() || Size == 0)
		{
			OutPut = FileName + "Size:" + std::to_string(file.tellg()) + "!= Size:" + std::to_string(Size);

			file.close();
			return false;
		}
		file.seekg(0, std::ios_base::beg);
		file.read(pBuffer, Size);
		if (!file)
		{
			OutPut = FileName + " read error!";
			file.close();
			return false;
		}
		file.close();
		return true;
	}
	else
	{
		OutPut = "can't find " + FileName + "!";
		return false;
	}

}
bool novice::WriteFile(std::string FileName, char* pBuffer, int Size, std::string& OutPut, bool FullPathName)
{
	if (!FullPathName)
	{
		FileName = COperatorINI::GetAppPath() + NOVICE_ENTER + FileName;
	}

	return true;
}