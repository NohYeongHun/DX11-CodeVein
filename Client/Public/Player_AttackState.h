#pragma once
#include "PlayerState.h"

/* 모든 동작의 기본이 되는 동작. */
// 23
NS_BEGIN(Client)
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
		_uint iComboStep = 0;           // 현재 콤보 단계 (0: 첫 공격, 1: 두번째 공격...)
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

	// State 실행 로직
	virtual void Update(_float fTimeDelta) override;

	// State Exit
	virtual void Exit() override;

	// State 초기값으로 설정
	virtual void Reset() override;

	void Change_State(_float fTimeDelta);

private:
	_float m_fAttackTime = {}; // 최소 진행 시간.
	_float m_fCurTime = {};

public:
	static CPlayer_AttackState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;


};
NS_END
