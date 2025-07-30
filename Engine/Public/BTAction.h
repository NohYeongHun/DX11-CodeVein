#pragma once
#include "BTNode.h"

NS_BEGIN(Engine)
// ====== 5. Action 노드 (행동 수행) ======
class ENGINE_DLL CBTAction abstract : public CBTNode
{
protected:
    CBTAction() = default;
    virtual ~CBTAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) = 0;

    BT_RESULT Execute(_float fTimeDelta) override {
        return Perform_Action(fTimeDelta);
    };

public:
    virtual void Free() override;
};
NS_END

