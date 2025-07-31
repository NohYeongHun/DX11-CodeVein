#pragma once
#include "BTNode.h"
NS_BEGIN(Engine)
// ====== 2. Selector 노드 (OR 논리) ======
class ENGINE_DLL CBTSelector : public CBTNode
{

protected:
    CBTSelector() = default;
    virtual ~CBTSelector() = default;

public:
    void Add_Child(CBTNode* pChild);

    BT_RESULT Execute(_float fTimeDelta);

    void Reset();

public:
    static CBTSelector* Create();

    virtual void Free() override;



};
NS_END

