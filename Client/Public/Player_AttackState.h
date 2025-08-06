#pragma once
#include "PlayerState.h"


NS_BEGIN(Client)
/* 바로 못돌아오게하는 처리도 필요해보임*/
/* 32 */
class CPlayer_AttackState final : public CPlayerState
{
public:
	// 콤보 공격 정보 구조체
	typedef struct tagComboInfo
	{
		_uint iCurrentAttackIndex;      // 현재 공격 애니메이션 인덱스
		_uint iNextAttackIndex;         // 다음 공격 애니메이션 인덱스
		_float fComboStartTime;         // 연계 가능 시작 시간
		_float fComboEndTime;           // 연계 가능 종료 시간
		_bool bCanCombo;                // 연계 가능 여부
	}COMBO_INFO;

	typedef struct tagPlayerAttackEnterDesc
	{
		_uint iAnimation_Idx = {};
		ACTORDIR eDirection = {};
		_float fCurrentTrackPosition = { 0.f };
	}ATTACK_ENTER_DESC;

public:
	typedef struct tagPlayerAttackStateDesc : public CPlayerState::PLAYER_STATE_DESC
	{

	}ATTACK_STATE_DESC;

private:
	explicit CPlayer_AttackState();
	virtual ~CPlayer_AttackState() = default;

public:
	// 시작 초기화
	virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;


	// State 시작 시.
	virtual void Enter(void* pArg) override;

#pragma region 충돌 관리
//private:
//	virtual void Reset_ColliderActiveInfo();
//	virtual void Update_Collider_State();
#pragma endregion


	// State 실행 로직
	virtual void Update(_float fTimeDelta) override;

	// State Exit
	virtual void Exit() override;

	// State 초기값으로 설정
	virtual void Reset() override;

private:
	void Change_State(_float fTimeDelta);
	
	_vector Calculate_Input_Direction_From_Camera();




private:
	// ⭐ AttackState만의 특화된 함수들 (부모 클래스 함수들 활용)
	//void Handle_Unified_Direction_Input(_float fTimeDelta);
	//void Rotate_Player_To_Direction(_vector vTargetDirection, _float fTimeDelta);
	//_float Get_Adaptive_Rotation_Speed();

	


public:
	static CPlayer_AttackState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;


};
NS_END
