CSteminaBar::CSteminaBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CSteminaBar::CSteminaBar(const CSteminaBar& Prototype)
    : CUIObject(Prototype)
{
}



HRESULT CSteminaBar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSteminaBar::Initialize_Clone(void* pArg)
{
    if (FAILED(CUIObject::Initialize_Clone(pArg)))
        return E_FAIL;

    m_iTextureIndex = 0;
    m_fMaxStemina = 200.f;
    m_fStemina = m_fMaxStemina;
    m_iMaxStemina = 200;
    m_iStemina = m_iMaxStemina;
    m_iShaderPath = static_cast<_uint>(POSTEX_SHADERPATH::HPPROGRESSBAR);
    m_fIncreaseSpeed = 30.f; // 초당 30.f 차게끔?

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Events()))
        return E_FAIL;

    return S_OK;
}

void CSteminaBar::Priority_Update(_float fTimeDelta)
{
    Ratio_Calc(fTimeDelta);
    CUIObject::Priority_Update(fTimeDelta);
}


void CSteminaBar::Update(_float fTimeDelta)
{
    CUIObject::Update(fTimeDelta);
}

void CSteminaBar::Late_Update(_float fTimeDelta)
{
    CUIObject::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;
}

HRESULT CSteminaBar::Render()
{
    CUIObject::Begin();

  
    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    m_pShaderCom->Begin(m_iShaderPath);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    CUIObject::End();

    return S_OK;
}


/*
* 주의점 : 플레이어의 무적 시간 만큼 제공해야합니다. (피격 시간)
* 피격 시간 이내에 한번 더 맞았을 경우 효과가 제대로 안나올 수도 있음.
*/

// 한번에 늘리면 안되고 시간에 따라 늘리게?
//void CSteminaBar::Increase_Stemina(_uint iStemina, _float fTime)
//{
//    m_bIncrease = true;
//}
//
//void CSteminaBar::Decrease_Stemina(_uint iStemina, _float fTime)
//{
//    // 증가 중인데 감소한다면.
//    if (m_bIncrease)
//    {
//        m_fRightRatio = m_fLeftRatio;
//        m_iStemina = m_iStemina < iStemina ? 0 : m_iStemina - iStemina;
//        m_fLeftRatio = static_cast<_float>(m_iStemina) / static_cast<_float>(m_iMaxStemina);
//        m_bIncrease = false;
//    }
//    else
//    {
//        m_fRightRatio = static_cast<_float>(m_iStemina) / static_cast<_float>(m_iMaxStemina);
//        m_iStemina = m_iStemina < iStemina ? 0 : m_iStemina - iStemina;
//        m_fLeftRatio = static_cast<_float>(m_iStemina) / static_cast<_float>(m_iMaxStemina);
//    }
//    
//    m_bDecrease = true;
//
//}


#pragma region 증감 효과
void CSteminaBar::Increase_Stemina(_float iStemina, _float fTime)
{
    m_bIncrease = true;
}

void CSteminaBar::Decrease_Stemina(_float fStemina, _float fTime)
{
    // 증가 중인데 감소한다면.
    if (m_bIncrease)
    {
        m_fRightRatio = m_fLeftRatio;
        m_fStemina = m_fStemina < fStemina ? 0 : m_fStemina - fStemina;
        m_fLeftRatio = m_fStemina / m_fMaxStemina;
        m_bIncrease = false;
    }
    else
    {
        m_fRightRatio = m_fStemina / m_fMaxStemina;
        m_fStemina = m_fStemina < fStemina ? 0 : m_fStemina - fStemina;
        m_fLeftRatio = m_fStemina / m_fMaxStemina;
    }

    m_bDecrease = true;

}

#pragma endregion


HRESULT CSteminaBar::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SteminaBar"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSteminaBar::Ready_Render_Resources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    //_float fFillRatio = 1.f - (static_cast<_float>(m_iStemina) / static_cast<_float>(m_iMaxStemina));
    _float fFillRatio = 1.f - (static_cast<_float>(m_fStemina) / static_cast<_float>(m_fMaxStemina));
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillRatio", static_cast<void*>(&fFillRatio), sizeof(fFillRatio))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fLeftRatio", static_cast<void*>(&m_fLeftRatio), sizeof(m_fLeftRatio))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRightRatio", static_cast<void*>(&m_fRightRatio), sizeof(m_fRightRatio))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_bIncrease", static_cast<void*>(&m_bIncrease), sizeof(m_bIncrease))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSteminaBar::Ready_Events()
{
//#pragma region STEMINA CHANGE
//    m_pGameInstance->Subscribe(EventType::STEMINA_CHANGE, Get_ID(), [this](void* pData)
//        {
//            STEMINA_CHANGE_DESC* desc = static_cast<STEMINA_CHANGE_DESC*>(pData);
//            if(desc->bIncrease)
//                this->Increase_Stemina(desc->iStemina, desc->fTime);
//            else
//                this->Decrease_Stemina(desc->iStemina, desc->fTime);
//        });
//
//    // Event 목록 관리.
//    m_Events.push_back(EventType::STEMINA_CHANGE);
//#pragma endregion

#pragma region STEMINA CHANGE
    m_pGameInstance->Subscribe(EventType::STEMINA_CHANGE, Get_ID(), [this](void* pData)
        {
            STEMINA_CHANGE_DESC* desc = static_cast<STEMINA_CHANGE_DESC*>(pData);
            if (desc->bIncrease)
                this->Increase_Stemina(desc->fStemina, desc->fTime);
            else
                this->Decrease_Stemina(desc->fStemina, desc->fTime);
        });

    // Event 목록 관리.
    m_Events.push_back(EventType::STEMINA_CHANGE);
#pragma endregion
    return S_OK;
}

//void CSteminaBar::Ratio_Calc(_float fTimeDelta)
//{
//    if (m_bDecrease)
//    {
//        if (m_fRightRatio <= m_fLeftRatio)
//        {
//            m_bDecrease = false;
//            m_bIncrease = true;
//        }
//        else
//            m_fRightRatio -= fTimeDelta * 0.1f;
//    }
//    else if (m_bIncrease)
//    {
//        m_fLeftRatio = static_cast<_float>(m_iStemina) / static_cast<_float>(m_iMaxStemina);
//        if (m_iStemina < m_iMaxStemina)
//        {
//            m_iStemina++; // 이부분이 증가부분.
//            m_fRightRatio = static_cast<_float>(m_iStemina) / static_cast<_float>(m_iMaxStemina);
//        }
//        else
//            m_bIncrease = false;
//    }
//}

void CSteminaBar::Ratio_Calc(_float fTimeDelta)
{
    if (m_bDecrease)
    {
        if (m_fRightRatio <= m_fLeftRatio)
        {
            m_bDecrease = false;
            m_bIncrease = true;
        }
        else
            m_fRightRatio -= fTimeDelta * 0.2f;
    }
    else if (m_bIncrease)
    {
        m_fLeftRatio = m_fStemina / m_fMaxStemina;
        if (m_fStemina < m_fMaxStemina)
        {
            m_fStemina += fTimeDelta * m_fIncreaseSpeed;
            m_fRightRatio = m_fStemina / m_fMaxStemina;
        }
        else
            m_bIncrease = false;
    }
}

CSteminaBar* CSteminaBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSteminaBar* pInstance = new CSteminaBar(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CSteminaBar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSteminaBar::Clone(void* pArg)
{
    CSteminaBar* pInstance = new CSteminaBar(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CSteminaBar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSteminaBar::Destroy()
{
    CUIObject::Destroy();

    for (auto& val : m_Events)
        m_pGameInstance->UnSubscribe(val, Get_ID());

    m_Events.clear();
}

void CSteminaBar::Free()
{
    CUIObject::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    
    Safe_Release(m_pTextureCom);
}
