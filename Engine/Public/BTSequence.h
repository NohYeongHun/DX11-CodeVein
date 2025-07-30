#pragma once
#include "BTNode.h"

NS_BEGIN(Engine)
// ====== 3. Sequence 노드 (AND 논리) ======
class ENGINE_DLL CBTSequence : public CBTNode
{
private:
    vector<CBTNode*> m_Children;
    _uint m_iCurrentChild = 0;

public:
    CBTSequence() = default;
    virtual ~CBTSequence() = default;

public:
    void Add_Child(CBTNode* pChild);

    BT_RESULT Execute(_float fTimeDelta) override;

    void Reset() override;

public:
    static CBTSequence* Create();

    virtual void Free() override;

};
NS_END

