#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CLockOnUI final : public CGameObject
{
public:
    typedef struct tagLockOnUIDesc : public CGameObject::GAMEOBJECT_DESC
    {
        LEVEL eCurLevel = { LEVEL::END };
        _float fX = 0.f;
        _float fY = 0.f;
        _float fSizeX = 64.f;
        _float fSizeY = 64.f;
        class CGameObject* pTarget = { nullptr };
        _float fTargetRadius = 1.0f;    // 타겟의 반지름 (크기 조정용)

    }LOCKONUI_DESC;

private:
    explicit CLockOnUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CLockOnUI(const CLockOnUI& Prototype);
    virtual ~CLockOnUI() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

public:
    // LockOn 대상 설정/해제
    void Set_Target(CGameObject* pTarget);
    void Clear_Target();
    _bool Has_Target() const { return m_pTarget != nullptr; }
    
    // UI 활성화/비활성화
    void Set_Active(_bool bActive) { m_bActive = bActive; }
    _bool Is_Active() const { return m_bActive; }

private:
    // 월드 좌표를 스크린 좌표로 변환
    _bool World_To_Screen(_vector vWorldPos, _float& fScreenX, _float& fScreenY);

private:
    // 컴포넌트
    class CShader* m_pShaderCom = { nullptr };
    class CTexture* m_pTextureCom = { nullptr };
    class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    
    LEVEL m_eCurLevel = { LEVEL::END };
    // LockOn 관련
    CGameObject* m_pTarget = { nullptr };
    _float m_fTargetRadius = 1.0f;
    _bool m_bActive = false;
    
    // 애니메이션
    _float m_fAnimationTime = 0.0f;
    _float m_fRotationSpeed = 2.0f;
    _float m_fScaleSpeed = 1.5f;
    
    // 화면 크기
    _float m_fWinSizeX = 0.0f;
    _float m_fWinSizeY = 0.0f;

    _float m_fSizeX = 0.f;
    _float m_fSizeY = 0.f;
    _float m_fX = 0.f;
    _float m_fY = 0.f;

private:
    HRESULT Bind_ShaderResources();

public:
    static CLockOnUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
NS_END