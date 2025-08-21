#pragma once

/* 피격 상태 - 공격받은 방향에 따라 다른 애니메이션 재생 */
NS_BEGIN(Client)
class CPlayer_DamageState final : public CPlayerState
{
public:
    typedef struct tagPlayerDamageEnterDesc
    {
        _uint iAnimation_Idx = {};
        ACTORDIR eDamageDirection = {};  // 피격 방향
    }DAMAGE_ENTER_DESC;

public:
    typedef struct tagPlayerDamageStateDesc : public CPlayerState::PLAYER_STATE_DESC
    {
        
    }DAMAGE_STATE_DESC;

private:
    explicit CPlayer_DamageState();
    virtual ~CPlayer_DamageState() = default;

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

private:
    _uint Get_Damage_Animation_By_Direction(ACTORDIR eDirection);

private:
    ACTORDIR m_eDamageDirection = ACTORDIR::END;
    _bool m_bAnimationFinished = false;
    _float m_fInvincibilityTime = 0.5f;  // 무적 시간
    _float m_fCurrentInvincibilityTime = 0.f;

public:
    static CPlayer_DamageState* Create(_uint iStateNum, void* pArg);
    virtual void Free() override;
};
NS_END