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
	void Handle_Input();

protected:
	class CPlayer* m_pPlayer = { nullptr };
	class CLoad_Model* m_pModelCom = { nullptr };
	class CTransform* m_pTransformCom = { nullptr };
	_uint m_iNextAnimIdx = {}; // 다음 애니메이션 인덱스 => PlayerDefine.h에 정의.
	_uint m_iCurAnimIdx = {};  // 현재 애니메이션 인덱스  => PlayerDefine.h에 정의
	_int m_iNextState = {};  // 다음 State => Player에 정의


protected:
	uint16_t m_KeyInput = {};
	_float4 m_vMoveDir = {};
	_bool m_LockOn = {}; // LockOn
	_bool m_bFirstFrame = false;
	ACTORDIR	  m_eDir = { ACTORDIR::END };
	_bool  m_isLoop = { true };
	

public:
	virtual void Free() override;
};
NS_END

