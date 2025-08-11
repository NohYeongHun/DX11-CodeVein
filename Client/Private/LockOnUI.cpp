
CLockOnUI::CLockOnUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CLockOnUI::CLockOnUI(const CLockOnUI& Prototype)
    : CGameObject(Prototype)
    , m_fTargetRadius(Prototype.m_fTargetRadius)
    , m_fRotationSpeed(Prototype.m_fRotationSpeed)
    , m_fScaleSpeed(Prototype.m_fScaleSpeed)
    , m_fWinSizeX(Prototype.m_fWinSizeX)
    , m_fWinSizeY(Prototype.m_fWinSizeY)
{
}

HRESULT CLockOnUI::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLockOnUI::Initialize_Clone(void* pArg)
{
    LOCKONUI_DESC* pDesc = static_cast<LOCKONUI_DESC*>(pArg);

    if (FAILED(CGameObject::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone LockOn UI");
        return E_FAIL;
    }
        

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
        


    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LockOnSite"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }

    


    return S_OK;
}

void CLockOnUI::Priority_Update(_float fTimeDelta)
{
    if (!m_bActive || !m_pTarget)
        return;

}

void CLockOnUI::Update(_float fTimeDelta)
{
    if (!m_bActive || !m_pTarget)
        return;

    m_fAnimationTime += fTimeDelta;
    _float fPulse = (sinf(m_fAnimationTime * m_fScaleSpeed) + 1.0f) * 0.1f + 0.9f;
    m_fSizeX = 64.0f * fPulse;  // 크기 키움
    m_fSizeY = 64.0f * fPulse;
}

void CLockOnUI::Late_Update(_float fTimeDelta)
{
    if (!m_bActive || !m_pTarget)
        return;

    // UI 렌더 그룹에 추가
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CLockOnUI::Render()
{
    if (!m_bActive || !m_pTarget)
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

void CLockOnUI::Set_Target(CGameObject* pTarget)
{
    m_pTarget = pTarget;
    m_bActive = (pTarget != nullptr);
    m_fAnimationTime = 0.0f; // 애니메이션 초기화
}

void CLockOnUI::Clear_Target()
{
    m_pTarget = nullptr;
    m_bActive = false;
}

/* World -> View -> 투영 으로 좌표 계산. */
_bool CLockOnUI::World_To_Screen(_vector vWorldPos, _float& fScreenX, _float& fScreenY)
{
    // 현재 카메라의 뷰/프로젝션 행렬 가져오기
    _matrix matView = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
    _matrix matProj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

    // 월드 -> 뷰 공간 변환
    _vector vViewPos = XMVector3TransformCoord(vWorldPos, matView);

    // 카메라 뒤쪽에 있으면 표시하지 않음
    if (XMVectorGetZ(vViewPos) < 0.0f)
        return false;

    // 뷰 -> 투영 공간 변환
    _vector vProjPos = XMVector3TransformCoord(vViewPos, matProj);

    // NDC 좌표 -> 스크린 좌표 변환
    fScreenX = (XMVectorGetX(vProjPos) + 1.0f) * 0.5f * m_fWinSizeX;
    fScreenY = (1.0f - XMVectorGetY(vProjPos)) * 0.5f * m_fWinSizeY;

    // 화면 범위 확인 (여유를 둬서 화면 가장자리까지 표시)
    if (fScreenX < -50.0f || fScreenX > m_fWinSizeX + 50.0f ||
        fScreenY < -50.0f || fScreenY > m_fWinSizeY + 50.0f)
        return false;


    return true;
}

HRESULT CLockOnUI::Bind_ShaderResources()
{
    // 타겟의 월드 위치 가져오기
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + m_fTargetRadius * 1.5f); // 조금 더 위로
    
    _float fUISize = 0.5f; // 매우 작게 시작해서 보이는지 확인
    
    // 카메라 위치 가져오기
    _vector vCamPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
    _vector vLook = XMVector3Normalize(vTargetPos - vCamPos); // 방향 수정: 카메라에서 타겟으로
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
    vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
    
    // 빌보드 행렬 구성 (스케일 + 회전 + 이동을 한번에)
    _matrix matWorld;
    matWorld.r[0] = XMVectorScale(vRight, fUISize); 
    matWorld.r[1] = XMVectorScale(vUp, fUISize);
    matWorld.r[2] = XMVectorScale(vLook, fUISize);
    matWorld.r[3] = XMVectorSetW(vTargetPos, 1.0f);
    
    _float4x4 g_matWorld;
    XMStoreFloat4x4(&g_matWorld, matWorld);

    // 쉐이더에 바인딩
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &g_matWorld)))
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

CLockOnUI* CLockOnUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLockOnUI* pInstance = new CLockOnUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CLockOnUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLockOnUI::Clone(void* pArg)
{
    CLockOnUI* pInstance = new CLockOnUI(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CLockOnUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLockOnUI::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    CGameObject::Free();
}