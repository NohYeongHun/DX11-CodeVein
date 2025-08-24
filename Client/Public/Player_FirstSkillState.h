#pragma once
#include "PlayerState.h"

NS_BEGIN(Client)
class CPlayer_FirstSkillState final : public CPlayerState
{
public:
	typedef struct tagPlayerFirstSkillEnterDesc
	{
		_uint iAnimation_Idx = {};
		ACTORDIR eDirection = {};
	}FIRSTSKILL_ENTER_DESC;


public:
	typedef struct tagPlayerFirstSkillStateDesc : public CPlayerState::PLAYER_STATE_DESC
	{

	}FIRSTSKILL_STATE_DESC;

private:
	explicit CPlayer_FirstSkillState();
	virtual ~CPlayer_FirstSkillState() = default;

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


public:
	static CPlayer_FirstSkillState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;
};
NS_END

