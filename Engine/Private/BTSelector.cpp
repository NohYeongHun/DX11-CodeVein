#include "BTSelector.h"

void CBTSelector::Add_Child(CBTNode* pChild)
{
    if (pChild) {
        m_Children.push_back(pChild);
        Safe_AddRef(pChild);
    }
}

BT_RESULT CBTSelector::Execute(_float fTimeDelta)
{
    for (_uint i = m_iCurrentChild; i < m_Children.size(); ++i) {
        BT_RESULT result = m_Children[i]->Execute(fTimeDelta);

        if (result == BT_RESULT::SUCCESS) {
            Reset();
            return BT_RESULT::SUCCESS;
        }

        if (result == BT_RESULT::RUNNING) {
            m_iCurrentChild = i;
            return BT_RESULT::RUNNING;
        }

        // FAILURE인 경우 다음 자식으로 계속
    }

    Reset();
    return BT_RESULT::FAILURE;
}

CBTSelector* CBTSelector::Create()
{
    return new CBTSelector;
}

void CBTSelector::Reset()
{
    m_iCurrentChild = 0;
    for (auto& child : m_Children) {
        child->Reset();
    }
}

void CBTSelector::Free()
{
    __super::Free();
    for (auto& child : m_Children) {
        Safe_Release(child);
    }
    m_Children.clear();
    __super::Free();
}
