#pragma once
#include "BTNode.h"
NS_BEGIN(Engine)
// ====== 4. Condition 노드 (조건 체크) ======
class ENGINE_DLL CBTCondition abstract : public CBTNode
{
protected:
    CBTCondition() = default;
    virtual ~CBTCondition() = default;

public:
    virtual _bool Check_Condition() = 0;

    BT_RESULT Execute(_float fTimeDelta) override {
        return Check_Condition() ? BT_RESULT::SUCCESS : BT_RESULT::FAILURE;
    }

    virtual void Free() override;
};
NS_END

