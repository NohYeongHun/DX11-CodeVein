#pragma once
#include "State.h"

NS_BEGIN(Client)
class CPlayerState abstract : public CState
{
public:
	typedef struct tagPlayerStateDesc : public CState::STATE_DESC
	{
	}PLAYER_STATE_DESC;

protected:
	virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;

protected:
	class CPlayer* m_pPlayer = { nullptr };
	class CLoad_Model* m_pModelCom = { nullptr };

public:
	virtual void Free() override;
};
NS_END

