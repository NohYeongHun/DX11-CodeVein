#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CFont final : public CBase
{
private:
	CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont() = default;

public:
	HRESULT Initialize_Clone(const _tchar* pFontFilePath);

	/* Render 시마다 Text 변경. */
	virtual HRESULT Render(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale = 1.f);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	SpriteBatch* m_pBatch = { nullptr };
	SpriteFont* m_pFont = { nullptr };

public:
	static CFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual void Free() override;

};
NS_END

