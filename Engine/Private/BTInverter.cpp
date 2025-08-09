#include "BTInverter.h"

BT_RESULT CBTInverter::Execute(_float fTimeDelta)
{
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
    CBTNode::Free();

    for (auto& pNode : m_Children)
        Safe_Release(pNode);
}
