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


#pragma region 기본 코드
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
    __super::Update(fTimeDelta);
    Update_KeyInput();
    HandleState(fTimeDelta);

    if (m_pGameInstance->Get_KeyPress(DIK_W))
        m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
    if (m_pGameInstance->Get_KeyPress(DIK_A))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
    if (m_pGameInstance->Get_KeyPress(DIK_D))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
    

}

void CPlayer::Late_Update(_float fTimeDelta)
{

    _float4 vDefaultPos = {}; 
    XMStoreFloat4(&vDefaultPos, m_pTransformCom->Get_State(STATE::POSITION));
    OutPutDebugFloat4(vDefaultPos);
    m_pTransformCom->Set_State(STATE::POSITION,
        m_pNavigationCom->Compute_OnCell(m_pTransformCom->Get_State(STATE::POSITION)));

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;

    __super::Late_Update(fTimeDelta);

}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
    ImGuiIO& io = ImGui::GetIO();

    // 기존 Player Debug Window
    ImVec2 windowPos = ImVec2(10.f, io.DisplaySize.y - 350.f);
    ImVec2 windowSize = ImVec2(300.f, 300.f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("Player Debug", nullptr, ImGuiWindowFlags_NoCollapse);

    _float3 vPos = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    ImGui::Text("Player Pos: (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);

    // === Navigation Debug UI 추가 ===
    ImGui::Separator();
    ImGui::Text("=== Navigation Debug ===");

    if (m_pNavigationCom)
    {
        _int cellCount = m_pNavigationCom->Get_CellCount();
        ImGui::Text("Total Cells: %d", cellCount);

        // 현재 플레이어가 어떤 Cell에 있는지 확인
        _int currentCell = m_pNavigationCom->Find_Cell_By_Position(vPos);
        ImGui::Text("Current Cell: %d", currentCell);

        // Cell별 정보 표시
        //static bool showAllCells = false;
        //ImGui::Checkbox("Show All Cells Info", &showAllCells);
    }
    else
    {
        ImGui::Text("Navigation Component: NULL");
    }

    ImGui::End();

    // Navigation 렌더링
    if (m_pNavigationCom)
        m_pNavigationCom->Render();

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

#pragma endregion


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

void CPlayer::Toggle_LockOn()
{
    if (m_isLockOn)
        Clear_LockOn_Target();
    else
        Search_LockOn_Target();
}

void CPlayer::Search_LockOn_Target()
{
    // 현재 레벨의 적 객체들 검사
    CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(m_eCurLevel), TEXT("Layer_Monster"));
    if (nullptr == pLayer)
    {
        return; // 레이어가 없으면 조용히 리턴
    }

    list<CGameObject*>& pEnemyObjects = pLayer->Get_GameObjects();
    if (pEnemyObjects.size() == 0)
    {
        return; // 적이 없으면 조용히 리턴
    }

    // === 카메라 기반 정보 가져오기 ===
    if (!m_pPlayerCamera)
        return;

    _vector vCameraPos = m_pPlayerCamera->Get_Transform()->Get_State(STATE::POSITION);
    _vector vCameraLook = m_pPlayerCamera->Get_Transform()->Get_State(STATE::LOOK);
    _vector vCameraRight = m_pPlayerCamera->Get_Transform()->Get_State(STATE::RIGHT);
    _vector vCameraUp = m_pPlayerCamera->Get_Transform()->Get_State(STATE::UP);

    vCameraLook = XMVector3Normalize(vCameraLook);
    vCameraRight = XMVector3Normalize(vCameraRight);
    vCameraUp = XMVector3Normalize(vCameraUp);

    // 카메라 FOV 정보 (카메라에서 가져오거나 하드코딩)
    _float fCameraFovY = m_pPlayerCamera->Get_FovY();
    _float fAspectRatio = m_pPlayerCamera->Get_Aspect();
    _float fCameraFovX = 2.0f * atanf(tanf(fCameraFovY * 0.5f) * fAspectRatio); // 가로 FOV
    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);

    CGameObject* pBestTarget = nullptr;
    _float fBestScore = -1.0f;

    for (auto& pGameObject : pEnemyObjects)
    {
        if (!pGameObject || pGameObject == this)
            continue;

        if (!Is_Valid_LockOn_Target(pGameObject))
            continue;

        _vector vTargetPos = pGameObject->Get_Transform()->Get_State(STATE::POSITION);

        // === 1. 카메라 시야 내 체크 ===
        if (!m_pPlayerCamera->Is_In_Camera_Frustum(vTargetPos))
            continue;

        // === 2. 플레이어와의 거리 체크 ===
        _vector vPlayerToTarget = vTargetPos - vPlayerPos;
        _float fDistance = XMVectorGetX(XMVector3Length(vPlayerToTarget));

        if (fDistance > m_fLockOnRange || fDistance < 1.0f)
            continue;

        // === 3. 점수 계산 ===
        _float fDistanceScore = (m_fLockOnRange - fDistance) / m_fLockOnRange;
        // 화면 중앙으로부터의 거리 (0에 가까울수록 높은 점수)
        _float fScreenDistance = m_pPlayerCamera->Get_Screen_Distance_From_Center(vTargetPos);
        _float fCenterScore = max(0.0f, 1.0f - min(fScreenDistance, 1.0f)); // 정규화

        _float fTotalScore = fDistanceScore * 0.3f + fCenterScore * 0.7f; // 화면 중앙 우선

        if (fTotalScore > fBestScore)
        {
            fBestScore = fTotalScore;
            pBestTarget = pGameObject;
        }
    }

    if (pBestTarget)
        Set_LockOn_Target(pBestTarget);
}

void CPlayer::Set_LockOn_Target(CGameObject* pTarget)
{
    m_pLockOn_Target = pTarget;
    m_isLockOn = true;
    m_fLockOnTimer = 0.0f;

    // 카메라에 LockOn 타겟 설정
    if (m_pPlayerCamera)
    {
        m_pPlayerCamera->Start_Zoom_In(0.3f);
    }
}

void CPlayer::Clear_LockOn_Target()
{
    m_pLockOn_Target = nullptr;
    m_isLockOn = false;
    m_fLockOnTimer = 0.0f;

    // 카메라 LockOn 해제
    if (m_pPlayerCamera)
    {
        m_pPlayerCamera->Start_Zoom_Out(0.3f); 
    }

    // UI나 이펙트 숨기기 (필요시)
    // m_pGameInstance->Hide_LockOn_UI();
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

}

_bool CPlayer::Is_Valid_LockOn_Target(CGameObject* pTarget)
{
    if (!pTarget)
        return false;

    // 1. 객체가 살아있는지 확인
    if (pTarget->Is_Destroy())
        return false;

    // 2. 거리 체크
    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPlayerPos));

    if (fDistance > m_fLockOnRange)
        return false;

    // 3. 추가 조건들 (필요에 따라)
    // - 적이 스턴 상태인지
    // - 적이 특정 상태(무적, 숨김 등)인지
    // - 시야에 가려져 있는지 (레이캐스팅)

    return true;
}

void CPlayer::Rotate_To_LockOn_Target(_float fTimeDelta, _float fSpeed)
{
    if (!m_isLockOn || !m_pLockOn_Target)
        return;

    _vector vDirection = Calculate_LockOn_Direction();
    if (XMVector3Equal(vDirection, XMVectorZero()))
        return;

    // Y축만 고려한 방향으로 회전
    vDirection = XMVectorSetY(vDirection, 0.f);
    vDirection = XMVector3Normalize(vDirection);

    // 목표 Yaw 각도 계산
    _float x = XMVectorGetX(vDirection);
    _float z = XMVectorGetZ(vDirection);
    _float fTargetYaw = atan2f(x, z);

    // 각도 정규화
    while (fTargetYaw > XM_PI) fTargetYaw -= XM_2PI;
    while (fTargetYaw < -XM_PI) fTargetYaw += XM_2PI;

    // 현재 회전과 부드러운 보간
    _float fCurrentYaw = m_pTransformCom->GetYawFromQuaternion();
    _float fYawDiff = fTargetYaw - fCurrentYaw;

    // 최단 경로로 회전
    while (fYawDiff > XM_PI) fYawDiff -= XM_2PI;
    while (fYawDiff < -XM_PI) fYawDiff += XM_2PI;

    // 회전 속도 적용
    _float fMaxRotation = fSpeed * fTimeDelta;
    if (fabsf(fYawDiff) > fMaxRotation)
    {
        fYawDiff = (fYawDiff > 0) ? fMaxRotation : -fMaxRotation;
    }

    // 새로운 회전 적용
    _float fNewYaw = fCurrentYaw + fYawDiff;
    _vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fNewYaw);
    m_pTransformCom->Set_Quaternion(qNewRot);
}

_vector CPlayer::Calculate_LockOn_Direction() const
{
    if (!m_isLockOn || !m_pLockOn_Target)
        return XMVectorZero();

    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pLockOn_Target->Get_Transform()->Get_State(STATE::POSITION);

    _vector vDirection = vTargetPos - vPlayerPos;
    vDirection = XMVectorSetY(vDirection, 0.f); // Y축 제거

    return XMVector3Normalize(vDirection);
}

_vector CPlayer::Get_LockOn_Attack_Direction() const
{
    if (!m_isLockOn || !m_pLockOn_Target)
        return XMVectorZero();

    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pLockOn_Target->Get_Transform()->Get_State(STATE::POSITION);

    _vector vDirection = vTargetPos - vPlayerPos;
    vDirection = XMVectorSetY(vDirection, 0.f); // Y축 제거

    return XMVector3Normalize(vDirection);
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

    //if (nullptr != m_pFsmCom)
    //    m_pFsmCom->Update(fTimeDelta);

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
        Toggle_LockOn(); // 기존 줌 코드 대신 LockOn 토글로 변경
    if (m_pGameInstance->Get_MouseKeyPress(MOUSEKEYSTATE::RB))
        m_KeyInput |= static_cast<uint16_t>(PLAYER_KEY::STRONG_ATTACK);

    // 방향 계산 추가 => Player State에 추가했음. HandleInput
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

void CPlayer::Take_Damage(_float fDamage, CMonster* pMonster)
{
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

    CNavigation::NAVIGATION_DESC        NaviDesc{};
    NaviDesc.iCurrentCellIndex = 0;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
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
    for (_uint i = PLAYER_ANIM_ATTACK1; i < PLAYER_ANIM_ATTACK16; ++i)
    {
        // 모델 재생속도 증가.
        m_pModelCom->Set_CurrentTickPerSecond(i
            , m_pModelCom->Get_CurrentTickPerSecond(i) * 2.f);
    }
    
    m_pModelCom->Set_CurrentTickPerSecond(PLAYER_ANIM_RUN_F_LOOP, m_pModelCom->Get_CurrentTickPerSecond(PLAYER_ANIM_RUN_F_LOOP) * 1.5f);
    m_pModelCom->Set_CurrentTickPerSecond(PLAYER_ANIM_DODGE_F, m_pModelCom->Get_CurrentTickPerSecond(PLAYER_ANIM_DODGE_F) * 2.f);
    m_pModelCom->Set_CurrentTickPerSecond(PLAYER_ANIM_STRONG_ATTACK, m_pModelCom->Get_CurrentTickPerSecond(PLAYER_ANIM_DODGE_F) * 2.f);

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
    CPlayerWeapon::PLAYER_WEAPON_DESC Weapon{};
    Weapon.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Weapon.pSocketMatrix = m_pModelCom->Get_BoneMatrix("IKSocket_RightHandAttach");
    Weapon.eCurLevel = LEVEL::STATIC;
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
    Safe_Release(m_pNavigationCom);
    
        
}
