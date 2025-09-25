#pragma once
#include "PlayerState.h"


NS_BEGIN(Client)
/* Index = 25. */
class CPlayer_DodgeState final : public CPlayerState
{
public:


public:
	typedef struct tagPlayerDodgeEnterDesc
	{
		_uint iAnimation_Idx = {};
		ACTORDIR eDirection;
	}DODGE_ENTER_DESC;

public:
	typedef struct tagPlayerDodgeStateDesc : public CPlayerState::PLAYER_STATE_DESC
	{

	}DODGESTATE_DESC;

private:
	explicit CPlayer_DodgeState();
	virtual ~CPlayer_DodgeState() = default;

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

	void Change_State();
	void Handle_Invincible_Buff(); // 무적 버프 처리

	void Update_Sound(_float fTimeDelta);

private:
	ACTORDIR			   m_eDir = { ACTORDIR::END };
	_bool m_IsPrevInvincible = {};
	_bool m_IsSoundPlayed = false;
	_float m_fSoundTime = {};


public:
	static CPlayer_DodgeState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;


};
NS_END
