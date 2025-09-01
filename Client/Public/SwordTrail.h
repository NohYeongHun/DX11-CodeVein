#pragma once

NS_BEGIN(Client)
class CSwordTrail final : public CGameObject
{
public:
	typedef struct tagSwordTrailDesc : CGameObject::GAMEOBJECT_DESC
	{
		LEVEL eCurLevel;
		CGameObject* pTarget = { nullptr };
	}SWORDTRAIL_DESC;
private:
	CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwordTrail(const CSwordTrail& Prototype);
	virtual ~CSwordTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Update_Trail_Point(_float3 vPointDown, _float3 vPointUp, _fmatrix WeaponMatrix);


private:
	virtual HRESULT Ready_Components(SWORDTRAIL_DESC* pDesc);
	virtual HRESULT Bind_ShaderResources();
	

private:


private:
	class CPlayer* m_pPlayer = { nullptr };
	class CVIBuffer_SwordTrail* m_pVIBufferCom = { nullptr } ;
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	_vector	m_vColorBack = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	_vector	m_vColorFront = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	_float			m_fAlpha = 1.f;

	LEVEL			m_eCurLevel = {};
	_float3			m_vPointDown = {};
	_float3			m_vPoint_Up = {} ;
	_float4x4		m_WeaponMatrix = {};
	_uint			m_iShaderID = {};

public:
	static CSwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
NS_END

