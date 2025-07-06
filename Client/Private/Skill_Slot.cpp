#include "Skill_Slot.h"

CSkill_Slot::CSkill_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CSkill_Slot::CSkill_Slot(const CSkill_Slot& Prototype)
    : CUIObject(Prototype)
{
}

void CSkill_Slot::Change_Skill(const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_pSkill->Change_Skill(strTextureTag, iTextureIndex);
}

void CSkill_Slot::Execute_Skill()
{
    // 이미 쿨타임이 돌고있다면?
    if (nullptr == m_pSkill || m_IsCoolTime)
        return;

    // 스킬 실행.
    m_IsCoolTime = true;
    m_fTime = 0.f;

    // 실제 스킬 실행 로직도 나중에 아래에 추가.

    /*
    * 실행 로직 예정. => 아님 이 때 Event를 Push 하던가?
    * 이벤트를 던져서 플레이어가 스킬 쓰게 하던가?
    */
}

HRESULT CSkill_Slot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_Slot::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    SKILLSLOT_DESC* pDesc = static_cast<SKILLSLOT_DESC*>(pArg);
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    return S_OK;
}

void CSkill_Slot::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}


void CSkill_Slot::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    // CoolTime인데 시간이 다되었으면.
    if (m_IsCoolTime)
    {
        m_fTime += fTimeDelta;

        if (m_fTime >= m_fCoolTime)
        {
            m_fTime = 0.f;
            m_IsCoolTime = false;
        }
    }
}

void CSkill_Slot::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Slot::Render()
{
    __super::Begin();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Int("g_iTextureIndex", m_iTextureIndex)))
        return E_FAIL;


    _float fFillRatio = 1.f;
    if (m_IsCoolTime)                        // 쿨타임이면 0~1로 노말라이즈
    {
        if (m_fCoolTime > 0.f)
            fFillRatio = Clamp(m_fTime / m_fCoolTime, 0.f, 1.f);
        else
            fFillRatio = 1.f;                // 방어 코드: 쿨타임 값이 0이면 그냥 다 찬 걸로
    }

    if (FAILED(m_pShaderCom->Bind_Float("g_fFillRatio", fFillRatio)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;

    m_pShaderCom->Begin(2);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();


    __super::End();

    return S_OK;
}

HRESULT CSkill_Slot::Ready_Childs()
{
    CSkill_Icon::SKILLICON_DESC Desc{};
    // 정중앙 위치
    Desc.fX = 0;
    Desc.fY = 0; 
    Desc.fSizeX = m_fSizeX;
    Desc.fSizeY = m_fSizeY;
    
    CUIObject* pUIObject = nullptr;
    
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_SkillIcon"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_pSkill = static_cast<CSkill_Icon*>(pUIObject);

    return S_OK;
}

HRESULT CSkill_Slot::Ready_Components(SKILLSLOT_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SkillSlot"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}


CSkill_Slot* CSkill_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkill_Slot* pInstance = new CSkill_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CLoading_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkill_Slot::Clone(void* pArg)
{
    CSkill_Slot* pInstance = new CSkill_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CLoading_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkill_Slot::Destroy()
{
    __super::Destroy();
}

void CSkill_Slot::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
