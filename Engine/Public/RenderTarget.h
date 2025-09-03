#pragma once
NS_BEGIN(Engine)
class CRenderTarget final : public CBase
{
private:
	explicit CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;


public:
	ID3D11RenderTargetView* Get_RTV() const;

public:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	void Clear();

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

#pragma region RENDER TARGET의 소유
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11Texture2D* m_pTexture2D = { nullptr };
	ID3D11RenderTargetView* m_pRTV = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };

private:
	_float4 m_vClearColor = {};
#pragma endregion

#ifdef _DEBUG
private:
	_float4x4 m_WorldMatrix = {};


#endif 


public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	virtual void Free() override;
};
NS_END

