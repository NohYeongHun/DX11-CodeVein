#pragma once
#include "Base.h"

/*
* 폰트만 따로 관리하는 매니저.
*/
NS_BEGIN(Engine)
class CFont_Manager final : public CBase
{
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();

#pragma region ENGINE 제공
public:
	/* 1. Load Font */
	HRESULT Load_Font(const _wstring& strFontID, const _tchar* pFontFilePath);

	/* 2. Render Font*/
	HRESULT Render_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale);
#pragma endregion



private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	unordered_map<_wstring, class CFont*> m_Fonts = {};

private:
	CFont* Find_Font(const _wstring& strFontID);

public:
	static CFont_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
NS_END

