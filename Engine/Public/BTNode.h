#pragma once
#include "Base.h"
NS_BEGIN(Engine)

// ====== 1. 기본 BT 노드 ======
class ENGINE_DLL CBTNode abstract : public CBase
{
protected:
    CBTNode() = default;
    virtual ~CBTNode() = default;

public:
    virtual void Destroy_Recursive()
    {
        // 모든 자식 노드들 재귀적 해제
        for (auto& pChild : m_Children)
        {
            if (pChild)
            {
                pChild->Destroy_Recursive();
                Safe_Release(pChild);
            }
        }
        m_Children.clear();
    }

    virtual BT_RESULT Execute(_float fTimeDelta) = 0;
    // Action Node들의 경우에는 해당 Reset시 Phase를 초기화해줍니다.
    virtual void Reset() {}

protected:
    vector<CBTNode*> m_Children;
    _uint m_iCurrentChild = 0;

public:
    virtual void Free() override {}
};
NS_END 

