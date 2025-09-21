#pragma once
#include "PlayerState.h"

/* 모든 동작의 기본이 되는 동작. */
NS_BEGIN(Client)
/* 6 */
class CPlayer_RunState final : public CPlayerState
{
public:


public:
	typedef struct tagPlayerRunEnterDesc
	{
		_uint iAnimation_Idx = {};
		ACTORDIR eDirection;
	}RUN_ENTER_DESC;

public:
	typedef struct tagPlayerRunStateDesc : public CPlayerState::PLAYER_STATE_DESC
	{

	}RUNSTATE_DESC;

private:
	explicit CPlayer_RunState();
	virtual ~CPlayer_RunState() = default;

public:
	// 시작 초기화
	virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;

	// State 시작 시.
	virtual void Enter(void* pArg) override;

	// State 실행 로직
	virtual void Update(_float fTimeDelta) override;

	void Update_Sound(_float fTimeDelta);

	// State Exit
	virtual void Exit() override;

	// State 초기값으로 설정
	virtual void Reset() override;

	void Change_State(_float fTimeDelta);
	//void RockOn_State(_float fTimeDelta);

private:
	_bool		   m_isLoop = { true };
	
	_float m_fFootStepFirst = {};
	_float m_fFootStepSecond = {};
	_float m_strSoundFile = {};
	_wstring m_strFootSoundFile = {};

	_bool m_bFirstSoundPlayed = false;
	_bool m_bSecondSoundPlayed = false;

public:
	static CPlayer_RunState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;


};
NS_END
