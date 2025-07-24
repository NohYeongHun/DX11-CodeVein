#pragma once
#include "Base.h"

/*
* 맵에 배치한 저장 파일들을 읽어오는 클래스 .dat로
*/
NS_BEGIN(Tool)
class CSaveFile_Loader final : public CBase
{
private:
	explicit CSaveFile_Loader();
	virtual ~CSaveFile_Loader() = default;

public:
	HRESULT Initialize();

public:
	//void Save_MapFile(string filePath);
	void Load_MapFile(string filePath, LEVEL eLevel);

	// Model Component만 저장
	void Save_ModelFile(string filePath, const _wstring& strPrototypeTag); 
	void Save_AnimModel(std::ofstream& ofs, class CTool_Model* pModel, const _wstring& strModelTag);
	//void Save_NonAnimModel(std::ofstream& ofs, class CTool_Model* pModel, const _wstring& strModelTag);

	void Load_ModelFile(string filePath, LEVEL eLevel);
	void Load_AnimModel(std::ifstream& ifs, class CTool_Model* pModel, const _wstring& strModelTag);
	

	

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	LEVEL m_eCurLevel = { LEVEL::LOGO };


private: 
	bool Read_MapInfo(std::ifstream& ifs, MAP_PART_INFO& outModel);

	/* void WriteWString(std::ofstream & ofs, const std::wstring & ws)
	{
		uint32_t len = static_cast<uint32_t>(ws.size());
		ofs.write(reinterpret_cast<const char*>(&len), sizeof(uint32_t));
		if (len > 0)
			ofs.write(reinterpret_cast<const char*>(ws.data()), len * sizeof(wchar_t));
	}

	void WriteString(std::ofstream& ofs, const std::string& st)
	{
		uint32_t len = static_cast<uint32_t>(st.size());
		ofs.write(reinterpret_cast<const char*>(&len), sizeof(uint32_t));
		if (len > 0)
			ofs.write(st.c_str(), len * sizeof(char));
	}

	inline bool ReadBytes(std::ifstream& ifs, void* dst, size_t bytes)
	{
		ifs.read(reinterpret_cast<char*>(dst), bytes);
		return !!ifs;
	}

	inline wstring ReadWString(std::ifstream& ifs)
	{
		uint32_t len = 0;
		if (!ReadBytes(ifs, &len, sizeof(uint32_t)))
			return {};

		std::wstring out;
		if (len > 0)
		{
			out.resize(len);
			if (!ReadBytes(ifs, out.data(), sizeof(wchar_t) * len))
			{
				out.clear();
			}
		}
		return out;
	}

	inline string ReadString(std::ifstream& ifs)
	{
		uint32_t len = 0;
		if (!ReadBytes(ifs, &len, sizeof(uint32_t)))
			return {};

		std::string out;
		if (len > 0)
		{
			out.resize(len);
			if (!ReadBytes(ifs, out.data(), sizeof(char) * len))
			{
				out.clear();
			}
		}

		return out;
	}*/


public:
	static CSaveFile_Loader* Create();
	virtual void Free();
};
NS_END
