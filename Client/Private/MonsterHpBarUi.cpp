CMonsterHpBar::CMonsterHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext }
{

}

CMonsterHpBar::CMonsterHpBar(const CMonsterHpBar& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CMonsterHpBar::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
    {
        CRASH("Failed Initialize Prototype MonsterHpBar UI");
        return E_FAIL;
    }


    return S_OK;
}

HRESULT CMonsterHpBar::Initialize_Clone(void* pArg)
{
    MONSTERHPUI_DESC* pDesc = static_cast<MONSTERHPUI_DESC*>(pArg);

    if (FAILED(CGameObject::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone MonsterHPBar_UI");
        return E_FAIL;
    }

    m_eCurLevel = pDesc->eCurLevel;
    m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath);
    // 화면 크기 가져오기
    RECT rcClient;
    GetClientRect(g_hWnd, &rcClient);
    m_vOffset = pDesc->vOffset;
    m_vScale = pDesc->vScale;
    m_fSizeX = pDesc->fSizeX;
    m_fSizeY = pDesc->fSizeY;
    m_fMaxHp = pDesc->fMaxHp;
    m_fHp = m_fMaxHp;
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

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MonsterHPBar"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }

    return S_OK;
}

void CMonsterHpBar::Priority_Update(_float fTimeDelta)
{
    // 시간 계산.
    Time_Calc(fTimeDelta);
    CGameObject::Priority_Update(fTimeDelta);

}

void CMonsterHpBar::Update(_float fTimeDelta)
{
    CGameObject::Update(fTimeDelta);
}

void CMonsterHpBar::Late_Update(_float fTimeDelta)
{
    // UI 렌더 그룹에 추가
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CMonsterHpBar::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Ready Bind Shader Resources Failed");
        return E_FAIL;
    }


    // UI용 쉐이더 패스 (Monster HPBar 11)
    if (FAILED(m_pShaderCom->Begin(m_iShaderPath)))
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


// LateUpdate 이후에 호출? => Monster에서.
void CMonsterHpBar::Calculate_Screen_Position(_fvector vMonsterPos)
{
    
    // 위치.
    _vector vCalcPos = XMVectorSetY(vMonsterPos, XMVectorGetY(vMonsterPos) + m_vOffset.y);
    vCalcPos = XMVectorSetX(vCalcPos, XMVectorGetX(vCalcPos) + m_vOffset.x);

    // 텍스처 크기를 기반으로 스케일 계산 (픽셀 -> 월드 단위 변환)
    _float fTextureWidth = m_fSizeX * 0.01f * 0.8f;
    _float fTextureHeight = m_fSizeY * 0.01f;

    // 카메라 위치 가져오기
    _vector vCamPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
    _vector vLook = XMVector3Normalize(vCalcPos - vCamPos);
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
    vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    // 빌보드 행렬 구성 (텍스처 원본 크기 사용)
    _matrix matWorld;
    matWorld.r[0] = XMVectorScale(vRight, fTextureWidth);
    matWorld.r[1] = XMVectorScale(vUp, fTextureHeight);
    matWorld.r[2] = XMVectorScale(vLook, 1.0f);
    matWorld.r[3] = XMVectorSetW(vCalcPos, 1.0f);

    XMStoreFloat4x4(&m_EquipWorld, matWorld);
}

void CMonsterHpBar::Increase_Hp(_float fHp, _float fTime)
{
    if (m_bDecrease)
    {
        m_fLeftRatio = m_fRightRatio;
        m_fHp = m_fHp < fHp ? 0 : m_fHp + fHp;
        m_fRightRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
        m_bDecrease = false;
    }
    else
    {
        m_fLeftRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
        m_fHp = min(m_fHp + fHp, m_fMaxHp);
        m_fRightRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
    }

    m_bIncrease = true;
}

void CMonsterHpBar::Decrease_HpUI(_float fHp, _float fTime)
{
    if (m_bIncrease)
    {
        m_fRightRatio = m_fLeftRatio;
        m_fHp = m_fHp < fHp ? 0 : m_fHp - fHp;
        m_fLeftRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
        m_bIncrease = false;
    }
    else
    {
        m_fRightRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
        m_fHp = max(m_fHp - fHp, 0);
        m_fLeftRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
    }

    m_bDecrease = true;
}

void CMonsterHpBar::Time_Calc(_float fTimeDelta)
{
    if (m_bDecrease)
    {
        if (m_fRightRatio <= m_fLeftRatio)
        {
            m_fRightRatio = 0;
            m_fLeftRatio = 0;
            m_bDecrease = false;
        }
        else
            m_fRightRatio -= fTimeDelta * 0.2f;
    }
    if (m_bIncrease)
    {
        if (m_fRightRatio <= m_fLeftRatio)
        {
            m_fRightRatio = 0;
            m_fLeftRatio = 0;
            m_bIncrease = false;
        }
        else
            m_fLeftRatio += fTimeDelta * 0.2f;
    }
}





HRESULT CMonsterHpBar::Bind_ShaderResources()
{
    // 쉐이더에 바인딩
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_EquipWorld)))
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

    /* HP 비율 쉐이더 계산을 하기 위한. */
    _float fFillRatio = 1.f - (static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp));
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillRatio", static_cast<void*>(&fFillRatio), sizeof(fFillRatio))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fLeftRatio", static_cast<void*>(&m_fLeftRatio), sizeof(m_fLeftRatio))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRightRatio", static_cast<void*>(&m_fRightRatio), sizeof(m_fRightRatio))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_bIncrease", static_cast<void*>(&m_bIncrease), sizeof(m_bIncrease))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
    {
        CRASH("Failed Bind Texture LockOnUI");
        return E_FAIL;
    }


    return S_OK;
}

CMonsterHpBar* CMonsterHpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonsterHpBar* pInstance = new CMonsterHpBar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CMonsterHpBar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonsterHpBar::Clone(void* pArg)
{
    CMonsterHpBar* pInstance = new CMonsterHpBar(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CMonsterHpBar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonsterHpBar::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    CGameObject::Free();
}

#ifdef _DEBUG



void CMonsterHpBar::ImGui_Render()
{
   
}
#endif // _DEBUG