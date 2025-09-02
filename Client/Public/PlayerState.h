#pragma once
#include "State.h"

/* 루트모션 있는 상태들은 Enter와 동시에 RootMotion을 Set해주자. */
NS_BEGIN(Client)
class CPlayerState abstract : public CState
{
public:
	typedef struct tagColliderActiveInfo
	{
		_float fStartRatio;  // 1. 시작 비율
		_float fEndRatio;    // 2. 끝 비율
		_bool bShouldEnable = true; // 3. true: Enable, false: Disable (해당 구간에서 콜라이더를 활성화할지 비활성화할지)
		CPlayer::COLLIDER_PARTS eColliderType = CPlayer::PART_WEAPON; // 4. 어떤 콜라이더 타입을 제어할지
		_uint iColliderID = 0;  // 5. 콜라이더 고유 식별자 (같은 타입의 여러 구간 구분용)
		// 내부 상태 관리 (매 프레임 중복 호출 방지)
		_bool bHasTriggeredStart = false; // 시작 지점에서 Enable/Disable 호출했는지
		_bool bIsCurrentlyActive = false; // 현재 해당 구간에 있는지

	}COLLIDER_ACTIVE_INFO;


	typedef struct tagAnimationSpeedInfo
	{
		_float fStartRatio; // 1. 시작 비율
		_float fEndRatio;	// 2. 끝 비율
		_uint iAnimationID = 0;
		_uint iAnimationIndex = 0;
		_float fOriginSpeed = 1.f;
		_float fModifySpeed = 1.f;
		_bool bHasTriggeredStart = false; // 시작 지점에서 Enable/Disable 호출했는지
		_bool bIsCurrentlyActive = false; // 현재 해당 구간에 있는지
	}ANIMATION_SPEED_INFO;

	typedef struct tagTrailInfo
	{
		_float fStartRatio; // 1. 시작 비율
		_float fEndRatio;	// 2. 끝 비율
		_uint iAnimationIndex = 0; // 3. 지정할 애니메이션 인덱스
		_bool bTriggerVisible = false; // 4. 해당 이벤트가 Off용인지 On 용도인지. => 기본 Off
		_bool bHasTriggeredStart = false; // 5. 시작 지점에서 Enable/Disable 호출했는지
		_bool bIsCurrentlyActive = false; // 6. 현재 해당 구간에 있는지
		std::function<void(void*)> events = nullptr; // 7. 특정 이벤트
		
	}TRAIL_ACTIVE_INFO;

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
	unordered_map<_uint, vector<COLLIDER_ACTIVE_INFO>> m_ColliderActiveMap;


protected:
	virtual void Handle_Collider_State();
	virtual void Reset_ColliderActiveInfo();
	virtual void Update_Collider_State();
	virtual void Force_Disable_All_Colliders(); // 모든 콜라이더 강제 비활성화

protected:
	void Add_Collider_Info(_uint iAnimIdx, const COLLIDER_ACTIVE_INFO& info);
	void Clear_Collider_Info(_uint iAnimIdx);
#pragma endregion

#pragma region PLAYER 애니메이션 속도 제어 변수.
protected:
	virtual void Handle_AnimationSpeed_State();
	virtual void Reset_AnimationSpeedInfo();

protected:
	void Add_AnimationSpeed_Info(_uint iAnimIdx, const ANIMATION_SPEED_INFO& info);

protected:
	unordered_map<_uint, vector<ANIMATION_SPEED_INFO>> m_AnimationSpeedMap;
#pragma endregion

#pragma region TRAIL 제어 변수.

protected:
	virtual void Handle_AnimationTrail_State();
	virtual void Reset_AnimationTrailInfo();

protected:
	void Add_AnimationTrail_Info(_uint iAnimIdx, const TRAIL_ACTIVE_INFO& info);

protected:
	unordered_map<_uint, vector<TRAIL_ACTIVE_INFO>> m_AnimationTrailMap;
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

