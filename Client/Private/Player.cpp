#include "Player.h"

#pragma region KEY CODE 미리 정의
const _uint CPlayer::m_KeyboardMappingsCount = 12;
const std::pair<PLAYER_KEY, _ubyte> CPlayer::m_KeyboardMappings[] = {
    { PLAYER_KEY::MOVE_FORWARD,  DIK_W },
    { PLAYER_KEY::MOVE_BACKWARD, DIK_S },
    { PLAYER_KEY::MOVE_LEFT,     DIK_A },
    { PLAYER_KEY::MOVE_RIGHT,    DIK_D },
    { PLAYER_KEY::DODGE,         DIK_SPACE },
    { PLAYER_KEY::INTERACT,      DIK_F },
    { PLAYER_KEY::INVENTORY,     DIK_I },
    { PLAYER_KEY::SKILL_1,       DIK_Z },
    { PLAYER_KEY::SKILL_2,       DIK_X },
    { PLAYER_KEY::SKILL_3,       DIK_C },
    { PLAYER_KEY::SKILL_4,       DIK_V },
    { PLAYER_KEY::GUARD,        DIK_LSHIFT }
};
#pragma endregion



CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject(Prototype)
{
}

/* 이동 함수 8방향 .*/

HRESULT CPlayer::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Initialize_Clone(void* pArg)
{
    PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);
    
    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    m_eCurLevel = pDesc->eCurLevel;
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Fsm()))
        return E_FAIL;

    _vector qInitRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.0f);
    m_pTransformCom->Set_Quaternion(qInitRot);

    _float3 vPos = { 0.f, 5.f, 0.f };
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&vPos));


    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    Update_KeyInput();

    Update_LockOn(fTimeDelta);
    HandleState(fTimeDelta);

    __super::Update(fTimeDelta);

    // 🔧 플레이어 움직임 완료 후 즉시 카메라 업데이트!
   // 이렇게 하면 카메라가 실시간으로 플레이어를 추적함
    //if (m_pPlayerCamera)
    //{
    //    // 카메라의 타겟 위치를 현재 플레이어 위치로 즉시 갱신
    //    m_pPlayerCamera->Force_Update_Target_Position();
    //}
    // 플레이어 Input 제어.
    
}

void CPlayer::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;

    __super::Late_Update(fTimeDelta);
    
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos = ImVec2(10.f, io.DisplaySize.y - 350.f);
    ImVec2 windowSize = ImVec2(300.f, 300.f);

    // Cond_Once: 최초 한 번만 위치 적용 → 이후 드래그 가능
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoCollapse);
    _float3 vPos = {};
    _float3 vAngle = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    ImGui::Text("Player Pos: (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);
    //ImGui::Text("Player Rotation : (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);
    ImGui::End();
#endif // _DEBUG

    if (FAILED(Ready_Render_Resources()))
    {
        CRASH("Ready Render Resource Failed");
        return E_FAIL;
    }

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            CRASH("Ready Bine Materials Failed");

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            CRASH("Ready Bone Matrices Failed");


        if (FAILED(m_pShaderCom->Begin(0)))
            CRASH("Ready Shader Begin Failed");

        if (FAILED(m_pModelCom->Render(i)))
            CRASH("Ready Render Failed");
    }


    return S_OK;

}

#pragma region 움직임 구현
void CPlayer::Move_By_Camera_Direction_8Way(ACTORDIR eDir, _float fTimeDelta, _float fSpeed)
{
    CCamera* pCamera = m_pGameInstance->Get_MainCamera();
    if (!pCamera)
    {
        CRASH("Camera Not Found");
        return;
    }

    _vector vLook = pCamera->Get_LookVector();
    _vector vRight = pCamera->Get_RightVector();

    vLook = XMVectorSetY(vLook, 0.f);
    vRight = XMVectorSetY(vRight, 0.f);
    vLook = XMVector3Normalize(vLook);
    vRight = XMVector3Normalize(vRight);

    _vector vMoveDir = XMVectorZero();

    // 1. Camera Look Right key를 이용한 방향벡터 생성.
    switch (eDir)
    {
    case ACTORDIR::U:   vMoveDir = vLook; break;        // 전방
    case ACTORDIR::D:   vMoveDir = -vLook; break;       // 후방 
    case ACTORDIR::L:   vMoveDir = -vRight; break;      // 좌측
    case ACTORDIR::R:   vMoveDir = vRight; break;       // 우측
    case ACTORDIR::LU:  vMoveDir = XMVector3Normalize(vLook - vRight); break;
    case ACTORDIR::LD:  vMoveDir = XMVector3Normalize(-vLook - vRight); break;
    case ACTORDIR::RU:  vMoveDir = XMVector3Normalize(vLook + vRight); break;
    case ACTORDIR::RD:  vMoveDir = XMVector3Normalize(-vLook + vRight); break;
    default: return;
    }

    vMoveDir = XMVectorSetY(vMoveDir, 0.f);
    vMoveDir = XMVector3Normalize(vMoveDir);

    if (XMVector3Equal(vMoveDir, XMVectorZero()))
        return;

    vMoveDir = XMVector3Normalize(vMoveDir);

    // 2. 목표 방향 계산 => Yaw 만들기.
    _float x = XMVectorGetX(vMoveDir);
    _float z = XMVectorGetZ(vMoveDir);
    _float fTargetYaw = atan2f(x, z);

    // 3. 각도 정규화
    while (fTargetYaw > XM_PI) fTargetYaw -= XM_2PI;
    while (fTargetYaw < -XM_PI) fTargetYaw += XM_2PI;

    // 4. 현재 회전과 부드러운 보간
    _float fCurrentYaw = m_pTransformCom->GetYawFromQuaternion();
    _float fYawDiff = fTargetYaw - fCurrentYaw;

    // 5. 최단 경로
    while (fYawDiff > XM_PI) fYawDiff -= XM_2PI;
    while (fYawDiff < -XM_PI) fYawDiff += XM_2PI;

    // 6. 회전 속도 조절
    _float fRotationSpeed = 8.0f;
    if (m_pModelCom && m_pModelCom->Is_Blending())
    {
        fRotationSpeed *= 0.2f;
    }

    // 7. MaxRotation
    _float fMaxRotation = fRotationSpeed * fTimeDelta;
    if (fabsf(fYawDiff) > fMaxRotation)
    {
        fYawDiff = (fYawDiff > 0) ? fMaxRotation : -fMaxRotation;
    }

    // 8. 새로운 회전 적용
    _float fNewYaw = fCurrentYaw + fYawDiff;
    _vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fNewYaw);
    m_pTransformCom->Set_Quaternion(qNewRot);

    // 9. 이동 적용
    m_pTransformCom->Move_Direction(vMoveDir, fTimeDelta * fSpeed);
}

void CPlayer::Debug_CameraVectors()
{
    m_pPlayerCamera->Debug_CameraVectors();
}

void CPlayer::Rotate_Player_To_Camera_Direction()
{
    // 1. 카메라의 Look Vector 가져오기
    
    _vector vCameraLook = m_pPlayerCamera->Get_LookVector();

    // 2. Y축 제거 (지면에서만 회전)
    vCameraLook = XMVectorSetY(vCameraLook, 0.f);
    vCameraLook = XMVector3Normalize(vCameraLook);

    // 3. 목표 회전 각도 계산
    _float x = XMVectorGetX(vCameraLook);
    _float z = XMVectorGetZ(vCameraLook);
    _float fTargetYaw = atan2f(x, z);

    // 4. 플레이어를 카메라 방향으로 즉시 회전
    _vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTargetYaw);
    m_pTransformCom->Set_Quaternion(qNewRot);
}

/* 
* Animation
*/
void CPlayer::Change_Animation(_uint iAnimIndex, _bool IsLoop, _float fDuration, _uint iStartFrame, _bool bEitherBoundary, _bool bSameChange)
{

}
#pragma endregion

void CPlayer::On_Collision_Enter(CGameObject* pOther)
{
}

void CPlayer::On_Collision_Stay(CGameObject* pOther)
{
}

void CPlayer::On_Collision_Exit(CGameObject* pOther)
{
}

#pragma region 플레이어 상태 함수들
void CPlayer::HandleState(_float fTimeDelta)
{
    // Lock On 상태는 플레이어 전체에 적용되므로 플레이어가 제어.
    //if (m_pGameInstance->Get_MouseKeyUp(MOUSEKEYSTATE::MB))
    //    m_isLockOn = !m_isLockOn;


    if (nullptr != m_pFsmCom)
        m_pFsmCom->Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        //m_pModelCom->Animation_Reset();
    }

   
}


_vector CPlayer::Calculate_Move_Direction(ACTORDIR eDir)
{
    CCamera* pCamera = m_pGameInstance->Get_MainCamera();

    _vector vLook = pCamera->Get_LookVector();
    _vector vRight = pCamera->Get_RightVector();

    vLook = XMVectorSetY(vLook, 0.f);
    vRight = XMVectorSetY(vRight, 0.f);
    vLook = XMVector3Normalize(vLook);
    vRight = XMVector3Normalize(vRight);

    switch (eDir)
    {
    case ACTORDIR::U:   return vLook;
    case ACTORDIR::D:   return -vLook;
    case ACTORDIR::L:   return -vRight;
    case ACTORDIR::R:   return vRight;
    case ACTORDIR::LU:  return XMVector3Normalize(vLook - vRight);
    case ACTORDIR::LD:  return XMVector3Normalize(-vLook - vRight);
    case ACTORDIR::RU:  return XMVector3Normalize(vLook + vRight);
    case ACTORDIR::RD:  return XMVector3Normalize(-vLook + vRight);
    default: return XMVectorZero();
    }

}
void CPlayer::Update_KeyInput()
{
    m_PrevKeyInput = m_KeyInput;

    // 이전 프레임 키 상태 초기화
    m_KeyInput = 0;

    // 키보드 키만 순회
    for (_uint i = 0; i < m_KeyboardMappingsCount; ++i)
    {
        const auto& keyMapping = m_KeyboardMappings[i];
        if (m_pGameInstance->Get_KeyPress(keyMapping.second))
        {
            m_KeyInput |= static_cast<uint16_t>(keyMapping.first);
        }
    }

    // 마우스 입력 확인
    if (m_pGameInstance->Get_MouseKeyPress(MOUSEKEYSTATE::LB))
        m_KeyInput |= static_cast<uint16_t>(PLAYER_KEY::ATTACK);
    if (m_pGameInstance->Get_MouseKeyUp(MOUSEKEYSTATE::MB))
    {
        //m_isLockOn = !m_isLockOn;
        // LockOn 토글 처리 (마우스 휠 클릭)
        Toggle_LockOn();

        _wstring wstrDebug = L"";
        if (m_isLockOn)
             wstrDebug = L"LockOn\n";
        else 
            wstrDebug = L"LockOff\n";
        OutputDebugString(wstrDebug.c_str());
    }
        
    if (m_pGameInstance->Get_MouseKeyPress(MOUSEKEYSTATE::RB))
        m_KeyInput |= static_cast<uint16_t>(PLAYER_KEY::STRONG_ATTACK);

    // 방향 계산 추가
    //m_eCurrentDirection = Calculate_Direction();
}
ACTORDIR CPlayer::Calculate_Direction()
{
    _bool bW = Is_KeyPressed(PLAYER_KEY::MOVE_FORWARD);
    _bool bS = Is_KeyPressed(PLAYER_KEY::MOVE_BACKWARD);
    _bool bA = Is_KeyPressed(PLAYER_KEY::MOVE_LEFT);
    _bool bD = Is_KeyPressed(PLAYER_KEY::MOVE_RIGHT);

    // 방향 결정
    if (bW && bA)      return ACTORDIR::LU;
    else if (bW && bD) return ACTORDIR::RU;
    else if (bS && bA) return ACTORDIR::LD;
    else if (bS && bD) return ACTORDIR::RD;
    else if (bW)       return ACTORDIR::U;
    else if (bS)       return ACTORDIR::D;
    else if (bA)       return ACTORDIR::L;
    else if (bD)       return ACTORDIR::R;

    return ACTORDIR::END;
}

void CPlayer::Toggle_LockOn()
{
    if (m_isLockOn)
    {
        // LockOn 해제
        Clear_LockOn_Target();
    }
    else
    {
        // LockOn 활성화 - 가장 적합한 타겟 찾기
        Search_LockOn_Target();
    }
}

void CPlayer::Update_LockOn(_float fTimeDelta)
{
    if (!m_isLockOn || !m_pLockOn_Target)
        return;

    m_fLockOnTimer += fTimeDelta;

    // 주기적으로 LockOn 타겟 유효성 검사
    if (m_fLockOnTimer >= m_fLockOnCheckInterval)
    {
        m_fLockOnTimer = 0.0f;

        if (!Is_Valid_LockOn_Target(m_pLockOn_Target))
        {
            Clear_LockOn_Target();
            return;
        }
    }

    // LockOn 시 플레이어 자동 회전 (옵션에 따라)
    if (m_bLockOnRotationEnabled)
    {
        Rotate_To_LockOn_Target(fTimeDelta, m_fLockOnRotationSpeed);
    }
}

void CPlayer::Search_LockOn_Target()
{
    // 현재 레벨의 적 객체들 검사
    CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(m_eCurLevel), TEXT("Layer_Monster"));
    if (nullptr == pLayer)
    {
        MSG_BOX(TEXT("Not Find Layer"));
        return;
    }

    list<CGameObject*>& pEnemyObjects = pLayer->Get_GameObjects();

    if (pEnemyObjects.size() == 0)
    {
        MSG_BOX(TEXT("Not Find Object"));
        return;
    }
        

    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vPlayerLook = m_pTransformCom->Get_State(STATE::LOOK);
    vPlayerLook = XMVector3Normalize(vPlayerLook);

    CGameObject* pBestTarget = nullptr;
    _float fBestScore = -1.0f;

    for (auto& pGameObject : pEnemyObjects)
    {
        if (!pGameObject || pGameObject == this)
            continue;

        // 타겟 유효성 검사
        if (!Is_Valid_LockOn_Target(pGameObject))
            continue;

        _vector vTargetPos = pGameObject->Get_Transform()->Get_State(STATE::POSITION);
        _vector vToTarget = vTargetPos - vPlayerPos;
        _float fDistance = XMVectorGetX(XMVector3Length(vToTarget));

        // 거리 체크
        if (fDistance > m_fLockOnRange || fDistance < 1.0f)
            continue;

        vToTarget = XMVector3Normalize(vToTarget);

        // 각도 체크 (플레이어 정면 기준)
        _float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, vToTarget));
        _float fAngle = XMConvertToDegrees(acosf(max(-1.0f, min(1.0f, fDot))));

        if (fAngle > m_fLockOnAngle * 0.5f)
            continue;

        // 점수 계산 (거리와 각도를 고려 - 가까우면서 정면에 있을수록 높은 점수)
        _float fDistanceScore = (m_fLockOnRange - fDistance) / m_fLockOnRange;
        _float fAngleScore = (m_fLockOnAngle * 0.5f - fAngle) / (m_fLockOnAngle * 0.5f);
        _float fTotalScore = fDistanceScore * 0.6f + fAngleScore * 0.4f;

        if (fTotalScore > fBestScore)
        {
            fBestScore = fTotalScore;
            pBestTarget = pGameObject;
        }
    }

    if (pBestTarget)
    {
        Set_LockOn_Target(pBestTarget);
    }
}

void CPlayer::Set_LockOn_Target(CGameObject* pTarget)
{
    m_pLockOn_Target = pTarget;
    m_isLockOn = true;
    m_fLockOnTimer = 0.0f;

    //// 카메라에 LockOn 타겟 설정
    if (m_pPlayerCamera)
    {
        m_pPlayerCamera->Set_LockOn_Target(pTarget);
    }

    // UI나 이펙트 표시 (필요시)
    // m_pGameInstance->Show_LockOn_UI(pTarget);
}

void CPlayer::Clear_LockOn_Target()
{
    m_pLockOn_Target = nullptr;
    m_isLockOn = false;
    m_fLockOnTimer = 0.0f;

    //// 카메라 LockOn 해제
    if (m_pPlayerCamera)
    {
        m_pPlayerCamera->Clear_LockOn_Target();
    }

    // UI나 이펙트 숨기기 (필요시)
    // m_pGameInstance->Hide_LockOn_UI();

}

_vector CPlayer::Calculate_LockOn_Direction() const
{
    if (!m_isLockOn || !m_pLockOn_Target)
        return XMVectorZero();

    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pLockOn_Target->Get_Transform()->Get_State(STATE::POSITION);

    _vector vDirection = vTargetPos - vPlayerPos;
    vDirection = XMVectorSetY(vDirection, 0.0f); // Y축 제거 (지면에서만 회전)

    return XMVector3Normalize(vDirection);
}

void CPlayer::Rotate_To_LockOn_Target(_float fTimeDelta, _float fRotSpeed)
{
    if (!m_isLockOn || !m_pLockOn_Target)
        return;

    _vector vLockOnDir = Calculate_LockOn_Direction();
    if (XMVectorGetX(XMVector3Length(vLockOnDir)) < 0.1f)
        return;

    _vector vCurrentLook = m_pTransformCom->Get_State(STATE::LOOK);
    vCurrentLook = XMVectorSetY(vCurrentLook, 0.0f);
    vCurrentLook = XMVector3Normalize(vCurrentLook);

    // 회전 각도 계산
    _float fDotProduct = XMVectorGetX(XMVector3Dot(vCurrentLook, vLockOnDir));
    fDotProduct = max(-1.0f, min(1.0f, fDotProduct));
    _float fAngle = acosf(fDotProduct);

    // 회전이 필요한 경우에만 처리
    if (fAngle > 0.01f) // 약 0.57도 이상일 때만 회전
    {
        // 회전 속도 제한
        _float fMaxRotationThisFrame = fRotSpeed * fTimeDelta;
        _float fActualRotation = min(fAngle, fMaxRotationThisFrame);

        // 부드러운 회전을 위한 Slerp 사용
        _float fLerpFactor = fActualRotation / fAngle;
        _vector vNewLook = XMVectorLerp(vCurrentLook, vLockOnDir, fLerpFactor);
        vNewLook = XMVector3Normalize(vNewLook);

        // Transform 업데이트
        m_pTransformCom->Set_State(STATE::LOOK, vNewLook);

        // Right와 Up 벡터 재계산
        _vector vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        _vector vRight = XMVector3Cross(vUp, vNewLook);
        m_pTransformCom->Set_State(STATE::RIGHT, XMVector3Normalize(vRight));

        _vector vNewUp = XMVector3Cross(vNewLook, vRight);
        m_pTransformCom->Set_State(STATE::UP, XMVector3Normalize(vNewUp));
    }
}

void CPlayer::Move_With_LockOn(_float fTimeDelta, _float fSpeed)
{
    if (!m_isLockOn || !Is_MovementKeyPressed())
        return;

    // 현재 방향 계산 (카메라 기준)
    ACTORDIR eDirection = Calculate_Direction();
    if (eDirection == ACTORDIR::END)
        return;

    // 이동 방향 벡터 계산 (카메라 기준)
    _vector vMoveDirection = Calculate_Move_Direction(eDirection);

    // LockOn 중에는 스트레이핑 이동 (타겟을 바라보면서 옆으로 이동)
    _vector vCurrentPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vNewPos = vCurrentPos + vMoveDirection * fSpeed * fTimeDelta;

    m_pTransformCom->Set_State(STATE::POSITION, vNewPos);
}

_bool CPlayer::Is_Valid_LockOn_Target(CGameObject* pTarget) const
{
    if (!pTarget)
        return false;

    // 거리 체크
    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPlayerPos));

    if (fDistance > m_fLockOnLoseRange)
        return false;

    // 타겟이 죽었거나 비활성화되었는지 확인 (필요시)
     if (!pTarget->Is_Active() || pTarget->Is_Dead())
         return false;

    return true;
}

#pragma endregion



HRESULT CPlayer::Ready_Components(PLAYER_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Model_Player")
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_Fsm()
{
    CFsm::FSM_DESC Desc{};
    Desc.pOwner = this;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Fsm")
        , TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), &Desc)))
        return E_FAIL;


    CPlayerState::PLAYER_STATE_DESC PlayerDesc{};
    PlayerDesc.pFsm = m_pFsmCom;
    PlayerDesc.pOwner = this;

    m_pFsmCom->Add_State(CPlayer_IdleState::Create(PLAYER_STATE::IDLE, &PlayerDesc));
    m_pFsmCom->Add_State(CPlayer_WalkState::Create(PLAYER_STATE::WALK, &PlayerDesc));
    m_pFsmCom->Add_State(CPlayer_RunState::Create(PLAYER_STATE::RUN, &PlayerDesc));
    m_pFsmCom->Add_State(CPlayer_DodgeState::Create(PLAYER_STATE::DODGE, &PlayerDesc));
    m_pFsmCom->Add_State(CPlayer_StrongAttackState::Create(PLAYER_STATE::STRONG_ATTACK, &PlayerDesc));
    m_pFsmCom->Add_State(CPlayer_GuardState::Create(PLAYER_STATE::GUARD, &PlayerDesc));
    m_pFsmCom->Add_State(CPlayer_AttackState::Create(PLAYER_STATE::ATTACK, &PlayerDesc));


    Register_CoolTime();

    // DODGE TickPerseoncd 증가.
    m_pModelCom->Set_CurrentTickPerSecond(PLAYER_ANIM_DODGE_F, m_pModelCom->Get_CurrentTickPerSecond(PLAYER_ANIM_DODGE_F) * 1.5f);
    m_pModelCom->Set_CurrentTickPerSecond(PLAYER_ANIM_STRONG_ATTACK, m_pModelCom->Get_CurrentTickPerSecond(PLAYER_ANIM_DODGE_F) * 1.5f);
    //m_pModelCom->Set_CurrentTickPerSecond(PLAYER_ANIM_SPECIAL_DOWN3, m_pModelCom->Get_CurrentTickPerSecond(PLAYER_ANIM_DODGE_F) * 1.5f);

    CPlayer_IdleState::IDLE_ENTER_DESC enter{};
    enter.iAnimation_Idx = PLAYER_ANIM_IDLE_SWORD;

    CPlayer_RunState::RUN_ENTER_DESC Run{};
    Run.iAnimation_Idx = PLAYER_ANIM_RUN_F_LOOP;
    m_pFsmCom->Change_State(PLAYER_STATE::RUN, &Run);

    m_pFsmCom->Change_State(PLAYER_STATE::IDLE, &enter);
    return S_OK;
}

void CPlayer::Register_CoolTime()
{
    _float fTimeDelta = m_pGameInstance->Get_TimeDelta();
    

    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::IDLE, 0.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::WALK, 0.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::RUN, 0.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::DODGE, 1.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::STRONG_ATTACK, 2.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::ATTACK, 1.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::GUARD, 0.5f);


    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::IDLE, 0.f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::WALK, 0.f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::RUN, 0.f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::DODGE, 1.5f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::STRONG_ATTACK, 2.f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::ATTACK, 0.7f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::GUARD, 0.4f);
}

HRESULT CPlayer::Ready_Render_Resources()
{
      
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{   
    CWeapon::WEAPON_DESC Weapon{};
    Weapon.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Weapon.pSocketMatrix = m_pModelCom->Get_BoneMatrix("IKSocket_RightHandAttach");
    Weapon.eCurLevel = m_eCurLevel;
    if (FAILED(__super::Add_PartObject(TEXT("Com_Weapon"),
        ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Weapon")
        , reinterpret_cast<CPartObject**>(& m_pPlayerWeapon), &Weapon)))
    {
        CRASH("Failed Create Weapon");
        return E_FAIL;
    }

    

    return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Destroy()
{
    __super::Destroy();
}

void CPlayer::Free()
{
    __super::Free();
    Safe_Release(m_pFsmCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pPlayerWeapon);
    
        
}
