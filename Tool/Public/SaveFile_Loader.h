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
	

#pragma region Navigation 저장
public:
	void Save_NavigationFile(string filePath, const vector<class CCell*>& Cells);
	void Save_NaviDesc(NAVIGATIONSAVE_DESC& naviDesc, const vector<class CCell*>& Cells);

public:
	NAVIGATIONSAVE_DESC Load_NavigationFile(string filePath);
#pragma endregion

	

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	LEVEL m_eCurLevel = { LEVEL::LOGO };


private: 
	bool Read_MapInfo(std::ifstream& ifs, MAP_PART_INFO& outModel);


public:
	static CSaveFile_Loader* Create();
	virtual void Free();
};
NS_END
