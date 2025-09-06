#pragma once
NS_BEGIN(Tool)
class CTestParticle final : public CGameObject
{
public:
    typedef struct tagTestParticleDesc : CGameObject::GAMEOBJECT_DESC
    {
        _float  fRadius;
        _float3 vDirection;
    }TESTPARTICLE_DESC;

#pragma region 기본 함수들
private:
    explicit CTestParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CTestParticle(const CTestParticle& Prototype);
    virtual ~CTestParticle() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();
#pragma endregion


#pragma region 0. 기본 변수들
private:
    class CShader* m_pShaderCom = { nullptr };
    class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
    class CVIBuffer_PointParticleDir_Instance* m_pVIBufferCom = { nullptr };

#pragma endregion



private:
    HRESULT Bind_ShaderResources();

public:
    static CTestParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

};
NS_END

