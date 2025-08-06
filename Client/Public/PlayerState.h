#pragma once
#include "State.h"

/* 루트모션 있는 상태들은 Enter와 동시에 RootMotion을 Set해주자. */
NS_BEGIN(Client)
class CPlayerState abstract : public CState
{
public:
	typedef struct tagColliderActiveInfo
	{
		_float fStartRatio;     
		_float fEndRatio;       
		_bool bIsActive;        
	}COLLIDER_ACTIVE_INFO;

public:
	typedef struct tagPlayerStateDesc : public CState::STATE_DESC
	{
	}PLAYER_STATE_DESC;

protected:
	virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
	void Handle_Input();

	virtual void Enter(void* pArg) override;

#pragma region COLLIDER 활성화 관련 변수
protected:
	unordered_map<_uint, COLLIDER_ACTIVE_INFO> m_ColliderActiveMap;
	_bool m_bPrevColliderState = false;


protected:
	virtual void Reset_ColliderActiveInfo();
	void Update_Collider_State();

#pragma endregion


#pragma region LOCK ON
protected:
	_vector Calculate_Unified_Attack_Direction();
	_vector Determine_Final_Direction(_vector vInputDirection, _vector vLockOnDirection);
	void Handle_Attack_Movement(_vector vDirection, _float fTimeDelta);
	_float Calculate_Attack_Move_Speed();

	void Handle_Unified_Direction_Input(_float fTimeDelta);
	void Rotate_Player_To_Direction(_vector vTargetDirection, _float fTimeDelta);
	_float Get_Adaptive_Rotation_Speed();

protected:
	// 새로운 멤버 변수 추가
	_bool m_bCanChangeDirection = true;     // 방향 변경 가능 여부
	_float m_fDirectionLockTime = 0.3f;     // 방향 고정 시간 (초)
	_float m_fCurrentLockTime = 0.0f;       // 현재 경과 시간
	_bool m_bIsDirectionLocked = false;     // 방향이 고정되었는지 여부
#pragma endregion


	

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

