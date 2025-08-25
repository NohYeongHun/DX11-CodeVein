#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/* 몬스터가 소유하고 있다가 출력 되게? */
class CSlash final : public CGameObject
{
public:
    typedef struct tagSlashUIDesc : public CGameObject::GAMEOBJECT_DESC
    {
        LEVEL eCurLevel = { LEVEL::END };
        _float fX = 0.f;
        _float fY = 0.f;
        _float fSizeX = 300.f;   // 가로 길게
        _float fSizeY = 12.f;    // 세로 얇게
        class CGameObject* pTarget = { nullptr };
        _float fTargetRadius = 1.0f;    // 타겟의 반지름 (크기 조정용)
        _float fRotationAngle = 0.0f;   // Z축 회전 각도 (라디안)

    }SLASHUI_DESC;

private:
    explicit CSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CSlash(const CSlash& Prototype);
    virtual ~CSlash() = default;

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
    
    // 회전 설정
    void Set_Rotation(_float fAngle) { m_fRotationAngle = fAngle; }

    // UI 활성화/비활성화
    void Set_Active(_bool bActive) { 
        m_bActive = bActive; 
        if (bActive) {
            m_fCurrentTime = 0.0f; // 활성화할 때 타이머 초기화
            m_fAnimationTime = 0.0f; // 애니메이션도 초기화
        }
    }
    _bool Is_Active() const { return m_bActive; }

    // 위치 설정
    void Set_Position(_fvector vPosition) { 
        m_vWorldPosition = vPosition; 
        m_bDirectionCalculated = false; // 위치 변경 시 방향 재계산 필요
    }
    
    // 로컬 위치 설정 (몬스터 기준)
    void Set_Local_Position(_fvector vLocalPosition) { 
        m_vLocalPosition = vLocalPosition; 
        m_bUseLocalPosition = true; 
    }
    
    // 공격 방향 설정 (회전 계산용)
    void Set_Attack_Direction(_fvector vDirection) { m_vAttackDirection = vDirection; }
    
    // 타이머 리셋
    void Reset_Timer() { m_fCurrentTime = 0.0f; m_fAnimationTime = 0.0f; }


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
    
    // 타이머
    _float m_fDisplayTime = 5.0f;        // 표시 시간 (초)
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간

    // 화면 크기
    _float m_fWinSizeX = 0.0f;
    _float m_fWinSizeY = 0.0f;

    _float m_fSizeX = 0.f;
    _float m_fSizeY = 0.f;
    _float m_fX = 0.f;
    _float m_fY = 0.f;
    
    // 회전 정보
    _float m_fRotationAngle = 0.0f; // Z축 회전 각도 (라디안)
    
    // 위치 정보
    _vector m_vWorldPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f); // 월드 좌표
    _vector m_vLocalPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f); // 몬스터 기준 로컬 좌표
    _vector m_vAttackDirection = XMVectorSet(0.f, 0.f, 1.f, 0.f); // 월드 좌표
    _bool m_bUseLocalPosition = false; // 로컬 좌표 사용 여부
    
    // 고정된 빌보드 방향 (처음 설정될 때만 계산)
    _vector m_vFixedRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
    _vector m_vFixedUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector m_vFixedLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    _bool m_bDirectionCalculated = false; // 방향이 계산되었는지 여부
    

private:
    HRESULT Bind_ShaderResources();

public:
    static CSlash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
NS_END

