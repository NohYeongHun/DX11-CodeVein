#pragma once
#include "BTNode.h"

NS_BEGIN(Engine)
class ENGINE_DLL CBTInverter : public CBTNode
{
public:
	explicit CBTInverter() = default;
	virtual ~CBTInverter() = default;

public:
	BT_RESULT Execute(_float fTimeDelta) override;

private:
	vector<CBTNode*> m_Children;

public:
	virtual void Free() override;

	// CBTNode을(를) 통해 상속됨


	
};
NS_END

