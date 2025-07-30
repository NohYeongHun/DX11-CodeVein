#pragma once
#include "Base.h"

NS_BEGIN(Engine)

/*
* 복사 생성을 하는 것이 아닌 자주 사용하는 Texture Component들을 모아둔 매니저
* 리소스 공유 용도임.
*/
class CTexture_Manager final : public CBase
{
private:
	CTexture_Manager();
	virtual ~CTexture_Manager() = default;


public:
	HRESULT Initialize(_uint iNumLevels);

#pragma region ENGINE에 제공
public:
	HRESULT Add_Texture(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strTextureTag);
	void	Change_Texture_ToGameObject(class CGameObject* pGameObject, const _wstring& strComponentTag, class CComponent** ppOut, _uint iLevelIndex, const _wstring& strTextureTag);
	void	Clear(_uint iLevelIndex);
#pragma endregion


private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_uint											m_iNumLevels = {  };
	map<const _wstring, class CTexture*>*			m_pTextures = { nullptr };
	typedef map<const _wstring, class CTexture*>	TEXTURES;


private:
	class CTexture* Find_Texture(_uint iLevelIndex, const _wstring& strTextureTag);
	

public:
	static CTexture_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};
NS_END

