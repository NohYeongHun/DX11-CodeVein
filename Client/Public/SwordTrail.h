#pragma once

NS_BEGIN(Client)
class CSwordTrail final : public CGameObject
{
public:
	typedef struct tagSwordTrailDesc : CGameObject::GAMEOBJECT_DESC
	{
		LEVEL eCurLevel;
		CGameObject* pTarget = { nullptr };
		TRAIL_DIFFUSE eDiffuseType = {};
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
	// 멀티 텍스처 시스템
	class CTexture* m_pBaseTexture = { nullptr };      // 기본 검 궤적
	_uint			m_iBaseTextureIndex = {};
	class CTexture* m_pDetailTexture = { nullptr };    // 슬래시 디테일
	class CTexture* m_pGlowTexture = { nullptr };      // 발광 효과
	_vector	m_vColorBack = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	_vector	m_vColorFront = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	_float			m_fAlpha = 1.f;

	LEVEL			m_eCurLevel = {};
	_float3			m_vPointDown = {};
	_float3			m_vPoint_Up = {} ;
	_float4x4		m_WeaponMatrix = {};
	_uint			m_iShaderPath = {};
	
	// 블렌딩 파라미터
	_float			m_fDetailBlendRatio = 0.3f;    // 디테일 텍스처 블렌드 비율
	_float			m_fGlowIntensity = 1.5f;       // 발광 강도 (하얀색 방지)
	_float			m_fUVScrollSpeed = 1.0f;       // UV 스크롤 속도
	
	// Ribbon Trail 파라미터
	_float			m_fTime = 0.0f;                // 시간 누적
	_float			m_fScrollSpeed = 0.8f;         // 스크롤 속도 (적당히)
	_float			m_fFadePower = 1.0f;           // 페이드 거듭제곱 (자연스럽게)
	_float			m_fIntensity = 2.0f;           // 전체 강도 (더 높임)
	_vector			m_vTrailColor = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f); // 원본 색상 유지

	

public:
	static CSwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

#ifdef _DEBUG
private:
	void ImGui_Render();
#endif // _DEBUG


};
NS_END

