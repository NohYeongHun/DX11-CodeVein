#include "BTInverter.h"

CBTInverter::CBTInverter()
{
    m_eNodeType = BT_NODE_TYPE::DECORATOR;
}

BT_RESULT CBTInverter::Execute(_float fTimeDelta)
{
    if (m_Children.empty())
        return BT_RESULT::FAILURE;

    BT_RESULT result = m_Children[0]->Execute(fTimeDelta);

    switch (result)
    {
    case BT_RESULT::SUCCESS:
        return BT_RESULT::FAILURE;
    case BT_RESULT::FAILURE:
        return BT_RESULT::SUCCESS;
    case BT_RESULT::RUNNING:
        return BT_RESULT::RUNNING;
    default:
        return BT_RESULT::FAILURE;
    }
}

void CBTInverter::Free()
{
    __super::Free();
}
