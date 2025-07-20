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
	void Save_File(string filePath);
	void Load_File(string filePath, LEVEL eLevel);


private:
	class CGameInstance* m_pGameInstance = { nullptr };
	LEVEL m_eCurLevel = { LEVEL::LOGO };


private: 
	bool Read_ModelInfo(std::ifstream& ifs, MODEL_INFO& outModel);

	void WriteWString(std::ofstream & ofs, const std::wstring & ws)
	{
		uint32_t len = static_cast<uint32_t>(ws.size());
		ofs.write(reinterpret_cast<const char*>(&len), sizeof(uint32_t));
		if (len > 0)
			ofs.write(reinterpret_cast<const char*>(ws.data()), len * sizeof(wchar_t));
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


public:
	static CSaveFile_Loader* Create();
	virtual void Free();
};
NS_END
