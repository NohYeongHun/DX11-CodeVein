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
    virtual BT_RESULT Execute(_float fTimeDelta) = 0;
    virtual void Reset() {}

public:
    virtual void Free() override {}
};
NS_END 

