/*
  文件名称:
  1. 暂时不支持中文, 不支持中间包含空格
*/
#ifndef    COPERATORINI_H_
#define    COPERATORINI_H_

#include <string>
#include <vector>
#include <memory>


namespace novice
{

	int stoi(const std::string& Str);

	float stof(const std::string& Str);


	std::string to_string(int value);

	std::string to_string(float value, int reserve = 3);

	std::string to_string(double value, int reserve = 3);

	template <class ReturnType>
	ReturnType stringToNum(const std::string& str)
	{
		std::istringstream iss(str);
		ReturnType num;
		iss >> num;
		return num;
	}


	class COperatorINIPrivate;
	class COperatorINI
	{
	public:
		COperatorINI();
		~COperatorINI();

		static std::string GetAppPath();
		static void DiviStr(const std::string& Str, std::vector<std::string>& VecStr);
		static bool CreatePath(std::string FileName, bool FullPathName = false);
		static bool IsExistence(std::string FileName, bool FullPathName = false);

		bool Attach(std::string FileName, bool FullPathName = false);

		bool IsContions(std::string Section, std::string Param);

		std::string GetParameterString(std::string Section, std::string Param);
		int GetParameterInt(std::string Section, std::string Param);
		float GetParameterFloat(std::string Section, std::string Param);
		double GetParameterDouble(std::string Section, std::string Param);

		//Param=aaa,bbb,ccc 提取到vector中。
		bool GetParameterVecString(std::string Section, std::string Param, std::vector<std::string>& vecString);

		//Param=100,101,102 提取到vector中。
		bool GetParameterVecInt(std::string Section, std::string Param, std::vector<int>& vecInt);

		//Param=20.5,40.7,60.3 提取到vector中。
		bool GetParameterVecFloat(std::string Section, std::string Param, std::vector<float>& vecFloat);

		bool SetParameterString(std::string Section, std::string Param, const std::string& String);
		bool SetParameterInt(std::string Section, std::string Param, int Value);
		bool SetParameterFloat(std::string Section, std::string Param, float Value);
		bool SetParameterDouble(std::string Section, std::string Param, double Value);

		bool GetParameterString(std::string Section, std::string Param, std::string& Value);
		bool GetParameterInt(std::string Section, std::string Param, int& Value);
		bool GetParameterFloat(std::string Section, std::string Param, float& Value);
		bool GetParameterDouble(std::string Section, std::string Param, double& Value);

		bool WriteParameterString(std::string Section, std::string Param, const std::string& Value);
		bool WriteParameterInt(std::string Section, std::string Param, int Value);
		bool WriteParameterFloat(std::string Section, std::string Param, float Value);
		bool WriteParameterDouble(std::string Section, std::string Param, double Value);

		bool WriteParamterVecString(std::string Section, std::string Param, const std::vector<std::string>& VecStr);
		bool WriteParamterVecInt(std::string Section, std::string Param, const std::vector<int>& VecInt);
		bool WriteParamterVecFloat(std::string Section, std::string Param, const std::vector<float>& VecFloat);
		bool WriteParamterVecDouble(std::string Section, std::string Param, const std::vector<double>& VecDouble);

	private:
		std::unique_ptr<COperatorINIPrivate> m_pPrivate;
	};

	bool ReadFile(std::string FileName, char* pBuffer, int Size, std::string& OutPut, bool FullPathName = false);
	bool WriteFile(std::string FileName, char* pBuffer, int Size, std::string& OutPut, bool FullPathName = false);

};

#endif
