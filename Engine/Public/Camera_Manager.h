#pragma once
#include "Base.h"
/* 카메라를 등록하고 수정하는 매니저 클래스*/

NS_BEGIN(Engine)
class CCamera_Manager final : public CBase
{
public:
	explicit CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	void Update(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
#pragma region ENGINE에 제공
public:
	class CCamera* Get_MainCamera();

	HRESULT Add_Camera(const _wstring& strCameraTag, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CCamera* Find_Camera(const _wstring& strCameraTag, _uint iLevelIndex);
	HRESULT Change_Camera(const _wstring& strCameraTag, _uint iLevelIndex);
	
#pragma endregion
public:
	

private:
	_uint m_iNumLevels = {};
	class CGameInstance* m_pGameInstance = { nullptr };
	map<const _wstring, class CCamera*>* m_Cameras = {};

	class CCamera* m_pCurrentCamera = { nullptr };
	typedef map<const _wstring, class CCamera*> CAMERAS;

public:
	static CCamera_Manager* Create(_uint iNumLevels);
	virtual void Free() override;

};
NS_END

