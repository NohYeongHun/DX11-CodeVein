#include "Skill_Panel.h"

CSkill_Panel::CSkill_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CSkill_Panel::CSkill_Panel(const CSkill_Panel& Prototype)
    : CUIObject(Prototype)
    , m_iSkillSlot { Prototype.m_iSkillSlot }
{
}

void CSkill_Panel::Change_Skill(_uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_SkillSlots[iSkillSlot]->Change_Skill(strTextureTag, iTextureIndex);
}

void CSkill_Panel::Execute_Skill(_uint iSkillSlot)
{
    m_SkillSlots[iSkillSlot]->Execute_Skill();
}

HRESULT CSkill_Panel::Initialize_Prototype()
{
    m_iSkillSlot = 4;
    return S_OK;
}

HRESULT CSkill_Panel::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    SKILLPANEL_DESC* pDesc = static_cast<SKILLPANEL_DESC*>(pArg);

    if (FAILED(Ready_Childs(pDesc)))
        return E_FAIL;


    return S_OK;
}

void CSkill_Panel::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CSkill_Panel::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CSkill_Panel::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Panel::Render()
{

    return S_OK;
}



HRESULT CSkill_Panel::Ready_Components()
{

    return S_OK;
}

HRESULT CSkill_Panel::Ready_Childs(SKILLPANEL_DESC* pDesc)
{
    // Panel 객체는 위치만 존재하고 Skill Slot을 소유하는 객체임.

    _float fSizeX = pDesc->fSkillSizeX;
    _float fSizeY = pDesc->fSkillSizeY;

    CSkill_Slot::SKILLSLOT_DESC Desc{};
    Desc.fX = 0;
    Desc.fY = 0;
    Desc.fSizeX = fSizeX;
    Desc.fSizeY = fSizeY;

    const _float posX[4] = { -fSizeX * 0.5f - 7.f, 0.f, fSizeX * 0.5f + 7.f, 0.f };
    const _float posY[4] = { 0.f,  fSizeY * 0.5f + 7.f, 0.f, -fSizeY * 0.5f - 7.f };

    CUIObject* pUIObject = nullptr;

    for (_uint i = 0; i < m_iSkillSlot; ++i)
    {
        Desc.fX = posX[i];
        Desc.fY = posY[i];

        pUIObject = dynamic_cast<CUIObject*>(
            m_pGameInstance->Clone_Prototype(
                PROTOTYPE::GAMEOBJECT
                , ENUM_CLASS(LEVEL::STATIC)
                , TEXT("Prototype_GameObject_SkillSlot"), &Desc));

        if (nullptr == pUIObject)
            return E_FAIL;

        AddChild(pUIObject);
        m_SkillSlots.push_back(static_cast<CSkill_Slot*>(pUIObject));
    }

    

    return S_OK;
}

CSkill_Panel* CSkill_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkill_Panel* pInstance = new CSkill_Panel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CLoading_Panel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkill_Panel::Clone(void* pArg)
{
    CSkill_Panel* pInstance = new CSkill_Panel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CLoading_Panel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkill_Panel::Destroy()
{
    __super::Destroy();
}

void CSkill_Panel::Free()
{
    __super::Free();
}
