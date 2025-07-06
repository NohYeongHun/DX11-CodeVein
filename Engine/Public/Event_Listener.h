#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CEventListener final : public CBase
{

private:
	explicit CEventListener();
	virtual ~CEventListener() = default;


public:
	virtual void Free();
};
NS_END

