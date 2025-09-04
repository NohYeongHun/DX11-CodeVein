#include "Weapon.h"
CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
    : CPartObject(Prototype)
{

}

HRESULT CWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon::Initialize_Clone(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
    m_pSocketMatrix = pDesc->pSocketMatrix;
    m_eCurLevel = pDesc->eCurLevel;
    m_pOwner = pDesc->pOwner;
    m_fAttackPower = pDesc->fAttackPower;

    if (FAILED(CPartObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone Weapon");
        return E_FAIL;
    }
        

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Clone Component");
        return E_FAIL;
    }
        

    return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
    CGameObject::Priority_Update(fTimeDelta);
}

void CWeapon::Update(_float fTimeDelta)
{
    CGameObject::Update(fTimeDelta);
}


void CWeapon::Finalize_Update(_float fTimeDelta)
{
    CGameObject::Finalize_Update(fTimeDelta);

    /* Collider의 Set Active는 Finalize_Update 이전에 호출됩니다. */
    if (m_pColliderCom)
    {
        _matrix WorldMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);
        m_pColliderCom->Update(WorldMatrix);
    }

    Update_AttackDirection(fTimeDelta);
}

void CWeapon::Late_Update(_float fTimeDelta)
{
    CGameObject::Late_Update(fTimeDelta);
}

HRESULT CWeapon::Render()
{
    return S_OK;
}

// 타이머 관련한 함수들을 모아둔 클래스
void CWeapon::Update_Timer(_float fTimeDelta)
{
    Update_DissolveFrame(fTimeDelta);
}

/* Transform의 Prev Position을 사용할 수 없음 => 오로지 부모뼈에 의해서만 위치가 이동하기 때문. */
void CWeapon::Update_AttackDirection(_float fTimeDelta)
{
    // 1. 스윙 방향 계산 (무기 팁 위치 기준)
    _vector vCurrentPosition = XMLoadFloat4(reinterpret_cast<const _float4*>(&m_CombinedWorldMatrix.m[3][0]));

    if (!m_bFirstFrame)
    {
        // 슬래시 방향 계산 (이전 위치에서 현재 위치로의 벡터를 반전)
        _vector vMovement = XMVectorSubtract(m_vPreviousPosition, vCurrentPosition);
        _float fMovementLength = XMVectorGetX(XMVector3Length(vMovement));

        // 움직임이 충분히 클 때만 스윙 방향 업데이트
        if (fMovementLength > 0.01f) // 임계값
        {
            m_vSwingDirection = XMVector3Normalize(vMovement);
        }
    }
    else
    {
        m_bFirstFrame = false;
    }

    // 다음 프레임을 위해 현재 위치 저장
    m_vPreviousPosition = vCurrentPosition;
}

#pragma region 1. 무기는 충돌에 대한 상태제어를 할 수 있어야한다.=> 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.

// 기본적으로 Owner랑은 충돌 처리 자체를 안한다.
void CWeapon::On_Collision_Enter(CGameObject* pOther)
{
}

void CWeapon::On_Collision_Stay(CGameObject* pOther)
{
}

void CWeapon::On_Collision_Exit(CGameObject* pOther)
{
}

/* 무기 콜라이더 활성화 */
void CWeapon::Activate_ColliderFrame(_float fDuration)
{
    m_pColliderCom->Set_Active(true);
    m_IsColliderActive = true;
    m_fColliderLifeTime = fDuration;

}

void CWeapon::Activate_Collider()
{
    m_pColliderCom->Set_Active(true);
    m_IsColliderActive = true;
}

void CWeapon::Deactivate_Collider()
{
    m_pColliderCom->Set_Active(false);
    m_IsColliderActive = false;
}

/* 무기 콜라이더 업데이트 및 비활성화 관리 */
void CWeapon::Update_ColliderFrame(_float fTimeDelta)
{
    if (m_IsColliderActive)
    {
        m_fColliderLifeTime -= fTimeDelta;
        if (m_fColliderLifeTime <= 0.f)
        {
            m_pColliderCom->Set_Active(false);
            m_IsColliderActive = false;
        }
    }
}
#pragma endregion

#pragma region 3. 데미지 증가.
void CWeapon::Increase_Damage(_float fDamage)
{
    m_fAttackPower += fDamage;
}

void CWeapon::Decrease_Damage(_float fDamage)
{
    m_fAttackPower -= fDamage;
}

#pragma endregion

#pragma region 4. Traill 관리 => 몬스터가 현재 애니메이션 Frame을 확인하고 관리해야함.

#pragma endregion

#pragma region 5. DISSOLVE 처리
void CWeapon::Set_DissolveTime(_float fDissolveTime)
{
    m_fMaxDissolveTime = fDissolveTime;
}
void CWeapon::Set_ReverseDissolveTime(_float fDissolveTime)
{
    m_fMaxReverseDissolveTime = fDissolveTime;
}

void CWeapon::Start_Dissolve(_float fDuration)
{
    if (fDuration != 0.f)
        m_fMaxDissolveTime = fDuration;

    m_fCurDissolveTime = 0.f;
    m_bDissolve = true;
    m_iShaderPath = static_cast<_uint>(MESH_SHADERPATH::DISSOLVE);
}
void CWeapon::ReverseStart_Dissolve(_float fDuration)
{
    if (fDuration != 0.f)
        m_fMaxReverseDissolveTime = fDuration;

    m_fCurDissolveTime = m_fMaxReverseDissolveTime;
    m_bReverseDissolve = true;
    m_iShaderPath = static_cast<_uint>(MESH_SHADERPATH::DISSOLVE);

}

void CWeapon::End_Dissolve()
{
    m_fCurDissolveTime = 0.f;
    m_iShaderPath = static_cast<_uint>(ANIMESH_SHADERPATH::DEFAULT);
    m_bDissolve = false;
    m_bReverseDissolve = false;
}


void CWeapon::Update_DissolveFrame(_float fTimeDelta)
{
    if (m_bDissolve)
    {
        m_fCurDissolveTime += fTimeDelta;

        if (m_fCurDissolveTime >= m_fMaxDissolveTime)
            m_bDissolve = false;
    }

    if (m_bReverseDissolve)
    {
        m_fCurDissolveTime -= fTimeDelta;
        if (m_fCurDissolveTime <= 0.f)
            m_bReverseDissolve = false;
    }
}

#pragma endregion




HRESULT CWeapon::Ready_Components()
{
    return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
    return S_OK;
}


void CWeapon::Free()
{
    CPartObject::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pDissolveTexture);
}
