#pragma once
#include "State.h"

NS_BEGIN(Client)
class CPlayerState abstract : public CState
{
	/* 플레이어 인풋 정보를 상위 객체에서 관리. */
protected:
	struct INPUT_INFO
	{
		bool bW, bS, bA, bD, bMouseL;
		DIR  eDirection;
		bool bAnyMovementKey;
	};

public:
	typedef struct tagPlayerStateDesc : public CState::STATE_DESC
	{
	}PLAYER_STATE_DESC;

protected:
	virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;

protected:
	INPUT_INFO Get_MoveMentInfo();
	DIR Calculate_Direction(_bool bW, _bool bS, _bool bA, _bool bD); // 방향 계산

protected:
	class CPlayer* m_pPlayer = { nullptr };
	class CLoad_Model* m_pModelCom = { nullptr };
	class CTransform* m_pTransformCom = { nullptr };
	_float4 m_vMoveDir = {};
	_bool m_LockOn = {}; // LockOn

	_bool m_bFirstFrame = false;
	_bool m_bKeyState[MAX_PATH];

public:
	virtual void Free() override;
};
NS_END

