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
    m_pTarget = pDesc->pTarget;

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

_bool CSlash::World_To_Screen(_vector vWorldPos, _float& fScreenX, _float& fScreenY)
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

HRESULT CSlash::Bind_ShaderResources()
{
    // 실제 렌더링 위치는 Set_Position으로 설정된 월드 좌표 사용
    _vector vRenderPosition = m_vWorldPosition;
    
    // 만약 위치가 설정되지 않았다면 기존 방식 사용 (타겟 따라다니기)
    if (XMVectorGetX(vRenderPosition) == 0.0f && XMVectorGetY(vRenderPosition) == 0.0f && XMVectorGetZ(vRenderPosition) == 0.0f)
    {
        vRenderPosition = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
        vRenderPosition = XMVectorSetY(vRenderPosition, XMVectorGetY(vRenderPosition) + m_fTargetRadius);
    }
    else
    {
        // 충돌 지점에서 카메라 방향으로 약간 앞으로 이동 (콜라이더 표면에 딱 붙게)
        _vector vCamPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
        _vector vCamDirection = XMVector3Normalize(vCamPos - vRenderPosition);
        vRenderPosition = XMVectorAdd(vRenderPosition, XMVectorScale(vCamDirection, 0.1f)); // 0.1f만큼 카메라 쪽으로
    }

    // 처음 한 번만 빌보드 방향 계산 (카메라 + 공격 방향)
    if (!m_bDirectionCalculated && !(XMVectorGetX(m_vWorldPosition) == 0.0f && XMVectorGetY(m_vWorldPosition) == 0.0f && XMVectorGetZ(m_vWorldPosition) == 0.0f))
    {
        // 카메라 위치 가져오기
        _vector vCamPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
        _vector vLook = XMVector3Normalize(vRenderPosition - vCamPos);
        _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
        vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
        
        // 3D 공격 방향을 빌보드 평면에 투영하여 회전 각도 계산
        _vector vAttackDirection3D = XMVector3Normalize(m_vAttackDirection);
        
        // 빌보드 평면의 Right와 Up 벡터에 투영
        _float fRightComponent = XMVectorGetX(XMVector3Dot(vAttackDirection3D, vRight));
        _float fUpComponent = XMVectorGetX(XMVector3Dot(vAttackDirection3D, vUp));
        
        // Y축 성분만 반전 (빌보드 좌표계와 월드 좌표계 차이 보정)
        _float fRotationAngle = atan2f(-fUpComponent, fRightComponent);
        
        // 회전 행렬 적용
        _float fCos = cosf(fRotationAngle);
        _float fSin = sinf(fRotationAngle);
        
        // 회전된 Right와 Up 벡터 계산하여 저장
        m_vFixedRight = XMVectorAdd(XMVectorScale(vRight, fCos), XMVectorScale(vUp, -fSin));
        m_vFixedUp = XMVectorAdd(XMVectorScale(vRight, fSin), XMVectorScale(vUp, fCos));
        m_vFixedLook = vLook;
        
        m_bDirectionCalculated = true; // 계산 완료 표시
    }

    // 기본값 사용 (방향이 계산되지 않은 경우)
    _vector vRight = m_vFixedRight;
    _vector vUp = m_vFixedUp;  
    _vector vLook = m_vFixedLook;

    // 행렬 구성 (저장된 고정 방향 사용)
    _matrix matWorld;
    matWorld.r[0] = XMVectorScale(vRight, 1.4f);
    matWorld.r[1] = XMVectorScale(vUp, 0.2f);
    matWorld.r[2] = XMVectorScale(vLook, 0.1f);
    matWorld.r[3] = XMVectorSetW(vRenderPosition, 1.0f);

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
