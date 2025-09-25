#pragma once

NS_BEGIN(Client)
class CBT_SkyBoss_NormalHitReaction final : public CBTAction
{
public:
    explicit CBT_SkyBoss_NormalHitReaction(class CSkyBoss* pOwner);
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CSkyBoss* m_pOwner;

    _float m_fReactionTime = 0.f;
    const _float m_fMaxReactionTime = 0.6f;  // 일반 피격은 짧은 경직
    _bool m_IsAnimationStarted = false;

public:
    static CBT_SkyBoss_NormalHitReaction* Create(class CSkyBoss* pOwner);
    virtual void Free() override;
};
NS_END
