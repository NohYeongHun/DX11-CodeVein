#include "Slash.h"
CSlash::CSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CSlash::CSlash(const CSlash& Prototype)
    : CGameObject(Prototype)
    , m_fTargetRadius(Prototype.m_fTargetRadius)
    , m_fRotationSpeed(Prototype.m_fRotationSpeed)
    , m_fScaleSpeed(Prototype.m_fScaleSpeed)
    , m_fWinSizeX(Prototype.m_fWinSizeX)
    , m_fWinSizeY(Prototype.m_fWinSizeY)
{
}

HRESULT CSlash::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
    {
        CRASH("Failed Initialize Prototype Slash UI");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSlash::Initialize_Clone(void* pArg)
{
    SLASHUI_DESC* pDesc = static_cast<tagSlashUIDesc*>(pArg);

    if (FAILED(CGameObject::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone SlashUI");
        return E_FAIL;
    }
    m_eCurLevel = pDesc->eCurLevel;

    // 화면 크기 가져오기
    RECT rcClient;
    GetClientRect(g_hWnd, &rcClient);
    m_fWinSizeX = static_cast<_float>(rcClient.right - rcClient.left);
    m_fWinSizeY = static_cast<_float>(rcClient.bottom - rcClient.top);

    // 컴포넌트 추가
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        CRASH("Failed Load Shader");
        return E_FAIL;
    }

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
    {
        CRASH("Failed Load VIBuffer");
        return E_FAIL;
    }



    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SlashUI"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }




    return S_OK;
}

void CSlash::Priority_Update(_float fTimeDelta)
{
    if (!m_bActive)
        return;
}

void CSlash::Update(_float fTimeDelta)
{
    if (!m_bActive)
        return;

    // 타이머 업데이트
    m_fCurrentTime += fTimeDelta;
    
    // 시간이 지나면 비활성화
    if (m_fCurrentTime >= m_fDisplayTime)
    {
        Set_Active(false);
        return;
    }

    m_fAnimationTime += fTimeDelta;
    _float fPulseX = (sinf(m_fAnimationTime * m_fScaleSpeed) + 1.0f) * 0.1f + 0.9f;
    _float fPulseY = (sinf(m_fAnimationTime * m_fScaleSpeed * 0.5f) + 1.0f) * 0.05f + 0.95f; // Y축은 덜 움직이게
    m_fSizeX = 250.0f * fPulseX; // 가로를 더 길게
    m_fSizeY = 6.0f * fPulseY;   // 세로를 더 얇게, 덜 움직이게
}

void CSlash::Late_Update(_float fTimeDelta)
{
    if (!m_bActive)
        return;

    // UI 렌더 그룹에 추가
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CSlash::Render()
{
    if (!m_bActive)
    {
        // Active, Target도 아닌데 Render되고 있다면 문제가 있음.
        CRASH("Failed Render LockOnUI");
        return S_OK;
    }


    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Ready Bind Shader Resources Failed");
        return E_FAIL;
    }


    // UI용 쉐이더 패스 (LockOnPass = 패스 7)
    if (FAILED(m_pShaderCom->Begin(7)))
    {
        CRASH("Ready Shader Begin Failed");
        return E_FAIL;
    }


    if (FAILED(m_pVIBufferCom->Bind_Resources()))
    {
        CRASH("Ready Bind Buffer Resources Failed");
        return E_FAIL;
    }

    if (FAILED(m_pVIBufferCom->Render()))
    {
        CRASH("Ready Render LockOnUI Failed");
        return E_FAIL;
    }

    return S_OK;
}

void CSlash::OnActivate(void* pArg)
{
}

void CSlash::OnDeactivate()
{
}

void CSlash::Set_Target(CGameObject* pTarget)
{
    m_pTarget = pTarget;
    m_bActive = (pTarget != nullptr);
    m_fAnimationTime = 0.0f; // 애니메이션 초기화
    m_fCurrentTime = 0.0f;   // 타이머 초기화
}

void CSlash::Clear_Target()
{
    m_pTarget = nullptr;
    m_bActive = false;
}

void CSlash::Set_Position(_fvector vPosition)
{
    // 1. 그려질 곳. Position 설정.
    m_pTransformCom->Set_State(STATE::POSITION, vPosition);
    
    // 2. 설정되었을 때 Camera에 따른 방향을 재계산할 필요성이 존재합니다.
    m_bDirectionCalculated = false;
}

/* 무기의 공격 방향 가져오기. */
void CSlash::Set_Hit_Direction(_fvector vDirection)
{
     m_vHitDirection = vDirection;
}


void CSlash::Rotate_Slash()
{
    // 이미 계산되었다면 다시 계산하지 않음
    if (m_bDirectionCalculated)
        return;

    // 1. 카메라 위치 가져오기
    _vector vCamPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
    _vector vSlashPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 2. 빌보드 기본 벡터 계산 (카메라를 향하도록)
    _vector vLook = XMVector3Normalize(vSlashPos - vCamPos);
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
    vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    // 3. 공격 방향을 빌보드 평면에 투영하여 회전 각도 계산
    _vector vAttackDirection = XMVector3Normalize(m_vHitDirection);
    _float fRightComponent = XMVectorGetX(XMVector3Dot(vAttackDirection, vRight));
    _float fUpComponent = XMVectorGetX(XMVector3Dot(vAttackDirection, vUp));
    _float fRotationAngle = atan2f(-fUpComponent, fRightComponent);

    
    // 4. Z축 회전된 Right와 Up 벡터 계산
    _float fCos = cosf(fRotationAngle);
    _float fSin = sinf(fRotationAngle);
    _vector vRotatedRight = XMVectorAdd(XMVectorScale(vRight, fCos), XMVectorScale(vUp, -fSin));
    _vector vRotatedUp = XMVectorAdd(XMVectorScale(vRight, fSin), XMVectorScale(vUp, fCos));
    
    // 5. TransformCom에 상태 설정 (한 번만)
    m_pTransformCom->Set_State(STATE::RIGHT, vRotatedRight);
    m_pTransformCom->Set_State(STATE::UP, vRotatedUp);
    m_pTransformCom->Set_State(STATE::LOOK, vLook);

    // 6. 크기 설정.
    m_pTransformCom->Set_Scale({ 1.4f, 0.2f, 1.f });

    // 7. 계산 완료 플래그 설정
    m_bDirectionCalculated = true;
}

HRESULT CSlash::Bind_ShaderResources()
{
    

    // 쉐이더에 바인딩
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
    {
        CRASH("Failed Bind World Matrix LockOnUI");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
    {
        CRASH("Failed Bind View Matrix LockOnUI");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
    {
        CRASH("Failed Bind Proj Matrix LockOnUI");
        return E_FAIL;
    }

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
    {
        CRASH("Failed Bind Texture LockOnUI");
        return E_FAIL;
    }

    return S_OK;
}

CSlash* CSlash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSlash* pInstance = new CSlash(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CSlash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSlash::Clone(void* pArg)
{
    CSlash* pInstance = new CSlash(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CSlash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSlash::Free()
{
    CGameObject::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    
}
