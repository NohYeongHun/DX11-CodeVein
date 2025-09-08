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
    { PLAYER_KEY::GUARD,        DIK_LCONTROL }
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
    if (FAILED(CContainerObject::Initialize_Prototype()))
        return E_FAIL;

    m_strObjTag = TEXT("Player");

    return S_OK;
}

HRESULT CPlayer::Initialize_Clone(void* pArg)
{
    PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);
    m_eCurLevel = pDesc->eCurLevel;
    m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath);
    m_Stats = {
        pDesc->fMaxHP,
        pDesc->fHP,
        pDesc->fAttackPower,
    };

    /* 1. 생성과 동시에 UI와 연동이 필요 */
    HPSYNCRONIZE_DESC HpSyncDesc = { m_Stats.fHP, m_Stats.fMaxHP };

    m_pGameInstance->Publish(EventType::HP_SYNCRONIZE, &HpSyncDesc);

    if (FAILED(CContainerObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed ContainerObject Initialize Clone");
        return E_FAIL;
    }
        

    m_eCurLevel = pDesc->eCurLevel;
    _vector vStartPos = XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f);
    m_pTransformCom->Set_State(STATE::POSITION, vStartPos);

    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }

    if (FAILED(Ready_Colliders(pDesc)))
    {
        CRASH("Failed Ready_Colliders");
        return E_FAIL;
    }
  

    if (FAILED(Ready_Navigations()))
    {
        CRASH("Failed Ready_Navigations");
        return E_FAIL;
    }

    if (FAILED(Ready_PartObjects()))
    {
        CRASH("Failed Ready_PartObjects");
        return E_FAIL;
    }
        
    if (FAILED(Initialize_BuffDurations()))
    {
        CRASH("Failed Init BuffDurations");
        return E_FAIL;
    }

    if (FAILED(InitializeAction_ToAnimationMap()))
    {
        CRASH("Failed InitAction AnimMap");
        return E_FAIL;
    }

    if (FAILED(Ready_Fsm()))
    {
        CRASH("Failed Ready FSM");
        return E_FAIL;
    }

    

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    CContainerObject::Update(fTimeDelta);

    Update_KeyInput();

   
    HandleState(fTimeDelta);
    
    // LockOn 상태를 카메라와 동기화
    Update_LockOn(fTimeDelta);
    
    // Update 분기의 마지막 시점에 실행되어야 하는 함수 모음
    Finalize_Update(fTimeDelta);
}

void CPlayer::Finalize_Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
    //m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
     
    m_pTransformCom->Set_State(STATE::POSITION
        , m_pNavigationCom->Compute_OnCell(
            m_pTransformCom->Get_State(STATE::POSITION)));

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_DebugComponent(m_pColliderCom)))
    {
        CRASH("Failed Add Debug Collider");
        return;
    }
        
#endif // _DEBUG


    CContainerObject::Late_Update(fTimeDelta);

}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
    //ImGui_Render();
#endif // _DEBUG


    if (FAILED(Bind_ShaderReosurces()))
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


        if (FAILED(m_pShaderCom->Begin(m_iShaderPath)))
            CRASH("Ready Shader Begin Failed");

        if (FAILED(m_pModelCom->Render(i)))
            CRASH("Ready Render Failed");
    }


    return S_OK;

}

#pragma endregion


#pragma region 움직임 구현
/* Navigation 버전*/
void CPlayer::RootMotion_Translate(_fvector vTranslate)
{
    if (nullptr == m_pTransformCom)
    {
	    CRASH("Transform Component Not Exist");
	    return;
    }

    m_pTransformCom->Translate(vTranslate, m_pNavigationCom);
}
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
    m_pTransformCom->Move_Direction(vMoveDir, fTimeDelta * fSpeed, m_pNavigationCom);
}

void CPlayer::Move_Direction(_fvector vDirection, _float fTimeDelta)
{
    m_pTransformCom->Move_Direction(vDirection, fTimeDelta, m_pNavigationCom);
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
    // 카메라 락온 시스템 사용
    if (m_pPlayerCamera)
    {
        m_pPlayerCamera->Toggle_LockOn_Mode();
        
        // 플레이어 상태도 카메라 상태에 맞춰 업데이트
        m_isLockOn = m_pPlayerCamera->Is_LockOn_Mode();
        m_pLockOn_Target = m_pPlayerCamera->Get_LockOn_Target();
    }
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
    //if (m_pPlayerCamera)
    //{
    //    m_pPlayerCamera->Start_Zoom_In(0.3f);
    //}
}

void CPlayer::Clear_LockOn_Target()
{
    m_pLockOn_Target = nullptr;
    m_isLockOn = false;
    m_fLockOnTimer = 0.0f;

    // 카메라 LockOn 해제
    //if (m_pPlayerCamera)
    //{
    //    m_pPlayerCamera->Start_Zoom_Out(0.3f); 
    //}

}

void CPlayer::Update_LockOn(_float fTimeDelta)
{
    // 카메라의 락온 상태와 동기화
    if (m_pPlayerCamera)
    {
        m_isLockOn = m_pPlayerCamera->Is_LockOn_Mode();
        m_pLockOn_Target = m_pPlayerCamera->Get_LockOn_Target();
    }
    
    if (!m_isLockOn || !m_pLockOn_Target)
        return;

    m_fLockOnTimer += fTimeDelta;
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




#pragma region 3. Animation 관리.
/* 사용하는 모든 Animation Map을 저장해둡니다. */
HRESULT CPlayer::InitializeAction_ToAnimationMap()
{

#pragma region 0. 이동
    m_Action_AnimMap.emplace(L"IDLE", PLAYER_ANIM_IDLE_LSWORD);
    m_Action_AnimMap.emplace(L"RUN", PLAYER_ANIM_RUN_F_LOOP);
    m_Action_AnimMap.emplace(L"DODGE", PLAYER_ANIM_LS_DODGE_ROLL_F);

    // LockOn시 일어나는 행동
    m_Action_AnimMap.emplace(L"RUN_F", PLAYER_ANIM_RUN_F_LOOP);
    m_Action_AnimMap.emplace(L"RUN_F_END", PLAYER_ANIM_RUN_F_END);
    m_Action_AnimMap.emplace(L"RUN_B", PLAYER_ANIM_RUN_B_LOOP);
    m_Action_AnimMap.emplace(L"RUN_B_END", PLAYER_ANIM_RUN_B_END);
    m_Action_AnimMap.emplace(L"RUN_L", PLAYER_ANIM_RUN_L_LOOP);
    m_Action_AnimMap.emplace(L"RUN_L_END", PLAYER_ANIM_RUN_L_END);
    m_Action_AnimMap.emplace(L"RUN_R", PLAYER_ANIM_RUN_R_LOOP);
    m_Action_AnimMap.emplace(L"RUN_R_END", PLAYER_ANIM_RUN_R_END);


    m_Action_AnimMap.emplace(L"DODGE_B", PLAYER_ANIM_LS_DODGE_ROLL_B);
    m_Action_AnimMap.emplace(L"DODGE_BL", PLAYER_ANIM_LS_DODGE_ROLL_BL);
    m_Action_AnimMap.emplace(L"DODGE_BR", PLAYER_ANIM_LS_DODGE_ROLL_BR);
    m_Action_AnimMap.emplace(L"DODGE_F", PLAYER_ANIM_LS_DODGE_ROLL_F);
    m_Action_AnimMap.emplace(L"DODGE_FL", PLAYER_ANIM_LS_DODGE_ROLL_FL);
    m_Action_AnimMap.emplace(L"DODGE_FR", PLAYER_ANIM_LS_DODGE_ROLL_FR);
    m_Action_AnimMap.emplace(L"DODGE_L", PLAYER_ANIM_LS_DODGE_ROLL_L);
    m_Action_AnimMap.emplace(L"DODGE_R", PLAYER_ANIM_LS_DODGE_ROLL_R);

#pragma endregion

#pragma region 1. HIT 판정
    m_Action_AnimMap.emplace(L"DAMAGE_B", PLAYER_ANIM_DAMAGE_B);
    m_Action_AnimMap.emplace(L"DAMAGE_F", PLAYER_ANIM_DAMAGE_F);
    m_Action_AnimMap.emplace(L"DAMAGE_L", PLAYER_ANIM_DAMAGE_L);
    m_Action_AnimMap.emplace(L"DAMAGE_R", PLAYER_ANIM_DAMAGE_R);

    m_Action_AnimMap.emplace(L"GUARD_START", PLAYER_ANIM_LS_GUARD_START);
    m_Action_AnimMap.emplace(L"GUARD_LOOP", PLAYER_ANIM_LS_GUARD_LOOP);
    m_Action_AnimMap.emplace(L"GUARD_END", PLAYER_ANIM_LS_GUARD_END);
#pragma endregion


#pragma region 2. 공격
    m_Action_AnimMap.emplace(L"ATTACK1", PLAYER_ANIM_S_ATK_NORMAL1);
    m_Action_AnimMap.emplace(L"ATTACK2", PLAYER_ANIM_S_ATK_NORMAL2);
    m_Action_AnimMap.emplace(L"ATTACK3", PLAYER_ANIM_S_ATK_NORMAL3);
    m_Action_AnimMap.emplace(L"ATTACK4", PLAYER_ANIM_S_ATK_NORMAL4);
    m_Action_AnimMap.emplace(L"ATTACK5", PLAYER_ANIM_S_ATK_NORMAL5);

    // 220 Frame => 100 Frame까지? 공격 판정?
    m_Action_AnimMap.emplace(L"STRONG_ATTACK1", PLAYER_ANIM_LS_ATK_STRONG1B);
#pragma endregion

#pragma region 3. 특수 공격. (스킬)
    m_Action_AnimMap.emplace(L"CIRCULATE_PURGE", PLAYER_ANIM_SKILL_STRONG3);
    m_Action_AnimMap.emplace(L"DRAGON_LUNGE", PLAYER_ANIM_SKILL_STRONG6);
#pragma endregion



#pragma region 99. 재생속도 증가.
    /* 재생 속도 증가*/
#pragma region  이동 애니메이션
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("RUN")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("RUN_B")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("RUN_F")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("RUN_L")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("RUN_R")], 1.5f);

    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DODGE_B")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DODGE_BL")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DODGE_BR")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DODGE_F")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DODGE_FL")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DODGE_FR")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DODGE_L")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DODGE_R")], 1.5f);
#pragma endregion



#pragma region 공격 애니메이션
    //m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("ATTACK1")], 0.7f);
    //m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("ATTACK2")], 0.7f);
    //m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("ATTACK3")], 0.7f);
    //m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("ATTACK4")], 0.7f);

    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("ATTACK1")],1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("ATTACK2")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("ATTACK3")], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("ATTACK4")], 1.5f);


    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("STRONG_ATTACK1")], 2.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("CIRCULATE_PURGE")], 1.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DRAGON_LUNGE")], 1.5f);
#pragma endregion



#pragma region HIT 애니메이션
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DAMAGE_B")], 2.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DAMAGE_F")], 2.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DAMAGE_L")], 2.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[TEXT("DAMAGE_R")], 2.f);
#pragma endregion



#pragma endregion

    return S_OK;
}

_uint CPlayer::Find_AnimationIndex(const _wstring& strAnimationTag)
{
    auto iter = m_Action_AnimMap.find(strAnimationTag);

    // 찾는 애니메이션이 없는 경우
    if (iter == m_Action_AnimMap.end())
    {
        CRASH("Failed Find Animation");
        return 0;
    }

    return iter->second;
}


void CPlayer::Change_Animation(_uint iAnimIndex, _bool IsLoop, _float fDuration, _uint iStartFrame, _bool bEitherBoundary, _bool bSameChange)
{

}
#pragma endregion


#pragma region 버프 관리.

void CPlayer::RemoveBuff(uint32_t buffFlag, _bool removeTimer)
{
    m_ActiveBuffs &= ~buffFlag;

    if (removeTimer)
        m_BuffTimers.erase(buffFlag);
}
const _bool CPlayer::AddBuff(_uint buffFlag, _float fCustomDuration)
{
    if (IsBuffOnCooldown(buffFlag))
        return false;

    m_ActiveBuffs |= buffFlag;

    // 시간을 지정한 경우에만 해당 시간으로 설정해줍니다. 
    if (m_BuffDefault_Durations[buffFlag] > 0.f)
    {
        m_BuffTimers[buffFlag] = fCustomDuration > 0.f ? fCustomDuration
            : m_BuffDefault_Durations[buffFlag];
        // 버프 타이머를 설정하면 자동으로 삭제되므로 상시 유지되는 버프도 삭제될 수도 있음
        // => Phase 상태.
    }


    return true;
}
const _bool CPlayer::IsBuffOnCooldown(_uint buffFlag)
{
    auto iter = m_BuffTimers.find(buffFlag);
    return (iter != m_BuffTimers.end() && iter->second > 0.f);
}
// 하나만 확인 가능.
_bool CPlayer::HasBuff(_uint buffFlag) const
{
    return (m_ActiveBuffs & buffFlag) != 0;
}

// 이 중에 아무거나 있으면 True | OR
_bool CPlayer::HasAnyBuff(_uint buffFlags) const
{
    return (m_ActiveBuffs & buffFlags) != 0;
}

// 다 가지고 있으면 True | And
_bool CPlayer::HasAllBuffs(_uint buffFlags) const
{
    return (m_ActiveBuffs & buffFlags) == buffFlags;
}


void CPlayer::Tick_BuffTimers(_float fTimeDelta)
{
    // 만료된 버프/디버프 들.
    std::vector<_uint> expiredBuffs;

    // 모든 타이머를 틱(감소)시킴
    for (auto& pair : m_BuffTimers)
    {
        uint32_t buffFlag = pair.first;
        _float& timer = pair.second;
        timer -= fTimeDelta;

        if (timer <= 0.0f)
            expiredBuffs.push_back(buffFlag);
    }

    // 만료된 버프들 제거
    for (uint32_t expiredBuff : expiredBuffs)
        RemoveBuff(expiredBuff, true);

    expiredBuffs.clear();

}


HRESULT CPlayer::Initialize_BuffDurations()
{
    m_BuffDefault_Durations[BUFF_HIT] = 1.f;        // 피격: 1초
    m_BuffDefault_Durations[BUFF_DOWN] = 5.f;       // 다운: 20초 => 두번 클릭했을 때 다운이 되는가.
    m_BuffDefault_Durations[BUFF_INVINCIBLE] = 1.f; // 무적 시간.
    return S_OK;
}



#pragma endregion


#pragma region 5. 플레이어 상태 스텟 관리.
void CPlayer::Increase_Damage(_float fDamage)
{
    m_pPlayerWeapon->Increase_Damage(fDamage);
}

void CPlayer::Decrease_Damage(_float fDamage)
{
    m_pPlayerWeapon->Decrease_Damage(fDamage);
}

#pragma endregion

#pragma region 6. TRAIL 관리

void CPlayer::SetTrail_Visible(_bool bTrail)
{
    m_pPlayerWeapon->Set_Trail(bTrail);
}


#pragma endregion

#pragma region MyRegion
void CPlayer::Create_Particle(CParticleSystem::PARTICLE_TYPE eType)
{
    CParticleSystem::PARTICLESYSTEM_ACTIVATE_DESC ActivateDesc = {};
    XMStoreFloat4((&ActivateDesc.vStartPos),m_pTransformCom->Get_State(STATE::POSITION));
    ActivateDesc.eAttachType = CParticleSystem::EAttachType::WORLD;
    ActivateDesc.pOwnerTransform = m_pTransformCom;

    m_pGameInstance->Move_Effect_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
        , TEXT("PARTICLE_SYSTEM"), TEXT("Layer_Effect")
        , 2, ENUM_CLASS(EFFECTTYPE::PARTICLE), &ActivateDesc);
}
#pragma endregion




#pragma region 충돌 관리
void CPlayer::On_Collision_Enter(CGameObject* pOther)
{
    // 몬스터 무기와 충돌했을 경우?
    CWeapon* pWeapon = dynamic_cast<CWeapon*>(pOther);

    // 이미 충돌 레이어를 몬스터, 몬스터 Weapon으로 한정했으므로 이건 Weapon 충돌.
    if (nullptr != pWeapon)
    {
        if (HasBuff(BUFF_INVINCIBLE))
        {
            OutputDebugWstring(TEXT("Buff Has Invincible"));
            return;
        }
            

        // 무기의 소유자(몬스터) 가져오기
        CMonster* pMonster = dynamic_cast<CMonster*>(pWeapon->Get_Owner());
        if (nullptr != pMonster)
        {
            // 몬스터 위치를 기반으로 피격 방향 계산 및 DamageState 전환
            Take_Damage(pWeapon->Get_AttackPower(), pMonster);
        }
    }

    CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
    if (nullptr != pMonster)
    {
        if (m_pFsmCom->Get_CurrentState() == CPlayer::SKILL_2)
        {
            m_pModelCom->Set_RootMotionTranslate(false);
            return;
        }

        if (m_pFsmCom->Get_CurrentState() == CPlayer::SKILL_1)
        {
            m_pModelCom->Set_RootMotionTranslate(false);
            return;
        }
    }
}

void CPlayer::On_Collision_Stay(CGameObject* pOther)
{
}

void CPlayer::On_Collision_Exit(CGameObject* pOther)
{
}


void CPlayer::Enable_Collider(COLLIDER_PARTS eColliderParts)
{
    switch (eColliderParts)
    {
    case PART_WEAPON:
        m_pPlayerWeapon->Activate_Collider();
        break;
    case PART_BODY:
        if (m_pColliderCom)
            m_pColliderCom->Set_Active(true);
        break;
    default:
        break;
    }
}

void CPlayer::Disable_Collider(COLLIDER_PARTS eColliderParts)
{
    switch (eColliderParts)
    {
    case PART_WEAPON:
        m_pPlayerWeapon->Deactivate_Collider();
        break;
    case PART_BODY:
        if (m_pColliderCom)
            m_pColliderCom->Set_Active(false);
        break;
    default:
        break;
    }
}

void CPlayer::Take_Damage(_float fHp)
{
    m_Stats.fHP -= fHp;
    // UI에 전달.
    HPCHANGE_DESC HpDesc{};
    HpDesc.bIncrease = false;
    HpDesc.fHp = fHp;
    HpDesc.fTime = 0.2f;
    m_pGameInstance->Publish(EventType::HP_CHANGE, &HpDesc);
}

ACTORDIR CPlayer::Calculate_Damage_Direction(CMonster* pAttacker)
{
    if (nullptr == pAttacker)
        return ACTORDIR::U; // 기본값: 앞쪽
        
    // 플레이어와 공격자의 위치
    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vAttackerPos = pAttacker->Get_Transform()->Get_State(STATE::POSITION);
    
    // 공격자로부터 플레이어로의 방향 벡터
    _vector vToPlayer = vPlayerPos - vAttackerPos;
    vToPlayer = XMVectorSetY(vToPlayer, 0.f); // Y축 제거 (수평면에서만 계산)
    vToPlayer = XMVector3Normalize(vToPlayer);
    
    // 플레이어의 전방 벡터
    _vector vPlayerForward = m_pTransformCom->Get_State(STATE::LOOK);
    vPlayerForward = XMVectorSetY(vPlayerForward, 0.f);
    vPlayerForward = XMVector3Normalize(vPlayerForward);
    
    // 플레이어의 오른쪽 벡터
    _vector vPlayerRight = m_pTransformCom->Get_State(STATE::RIGHT);
    vPlayerRight = XMVectorSetY(vPlayerRight, 0.f);
    vPlayerRight = XMVector3Normalize(vPlayerRight);
    
    // 내적을 통해 방향 계산
    _float fForwardDot = XMVectorGetX(XMVector3Dot(vToPlayer, vPlayerForward));
    _float fRightDot = XMVectorGetX(XMVector3Dot(vToPlayer, vPlayerRight));
    
    // 방향 결정 (가장 강한 방향으로 결정)
    if (abs(fForwardDot) > abs(fRightDot))
    {
        return (fForwardDot > 0) ? ACTORDIR::D : ACTORDIR::U;
    }
    else
    {
        return (fRightDot > 0) ? ACTORDIR::L : ACTORDIR::R;
    }
}

#pragma endregion




#pragma region 플레이어 상태 함수들
void CPlayer::HandleState(_float fTimeDelta)
{

    if(!m_IsInventoryDisplay)
        m_pFsmCom->Update(fTimeDelta);

    // Fsm 작동 이후에 상태 값에 대한 초기화를 담당하는 Tick_BuffTimer 실행.
    Tick_BuffTimers(fTimeDelta);

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

    if (m_pGameInstance->Get_KeyUp(DIK_I))
    {
        m_pGameInstance->Publish<CInventory>(EventType::INVENTORY_DISPLAY, nullptr);
        m_IsInventoryDisplay = !m_IsInventoryDisplay;
        m_pPlayerCamera->Set_InventroyMode(m_IsInventoryDisplay);
    }
        

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
    // 기본 데미지 처리
    Take_Damage(fDamage);

    if (fDamage <= 0.f)
        return;

    AddBuff(BUFF_INVINCIBLE);
    AddBuff(BUFF_HIT);
    
    // 공격자로부터 피격 방향 계산
    ACTORDIR eDamageDirection = Calculate_Damage_Direction(pMonster);
    
    // DamageState로 전환
    CPlayer_DamageState::DAMAGE_ENTER_DESC damageDesc{};
    damageDesc.eDamageDirection = eDamageDirection;
    
    m_pFsmCom->Change_State(PLAYER_STATE::DAMAGE, &damageDesc);
}


#pragma endregion






HRESULT CPlayer::Ready_Components(PLAYER_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CContainerObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Model_Player")
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_Colliders(PLAYER_DESC* pDesc)
{
    BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();
    m_fOffsetY = box.fHeight * 0.5f;

    CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
    SphereDesc.fRadius = max(max(box.vExtents.x, box.vExtents.y), box.vExtents.z);
    SphereDesc.vCenter = _float3(0.f, box.vExtents.y, 0.f); // 중점.
    SphereDesc.pOwner = this;
    SphereDesc.eCollisionType = CCollider::COLLISION_BODY;
    SphereDesc.eMyLayer = CCollider::PLAYER;
    SphereDesc.eTargetLayer = CCollider::MONSTER | CCollider::MONSTER_WEAPON
        | CCollider::MONSTER_SKILL | CCollider::STATIC_OBJECT;


    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Collider_SPHERE")
        , TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
    {
        CRASH("Failed Clone Collider SPHERE");
        return E_FAIL;
    }

    /* 생성과 동시에 등록 */
    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom, ENUM_CLASS(m_eCurLevel));

    return S_OK;
}

HRESULT CPlayer::Ready_Navigations()
{
    /* Navigation 추가 */
    CNavigation::NAVIGATION_DESC        NaviDesc{};
    NaviDesc.iCurrentCellIndex = 0;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
    {
        CRASH("Failed Clone Navigation");
        return E_FAIL;
    }
        
    _vector qInitRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.0f);
    m_pTransformCom->Set_Quaternion(qInitRot);

    // Navigation을 이용해서 내 초기화 위치와 가까운 실제 셀을 찾습니다. 
    // => 그리고 해당 Cell의 Center로 보내버립니다.

    _float3 vPos = {};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    _float3 vFinalPos = {};
    _int iNearCell = m_pNavigationCom->Find_NearCellIndex(vPos);
    
    if (iNearCell == -1)
    {
        CRASH("Failed Search Navigation Cell");
        return E_FAIL;
    }
    
    m_pNavigationCom->Set_CurrentCellIndex(iNearCell);
    XMStoreFloat3(&vFinalPos, m_pNavigationCom->Get_CellPos(iNearCell));
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&vFinalPos));

    return S_OK;
}

HRESULT CPlayer::Ready_Fsm()
{
    CFsm::FSM_DESC Desc{};
    Desc.pOwner = this;

    if (FAILED(CContainerObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
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
    m_pFsmCom->Add_State(CPlayer_DamageState::Create(PLAYER_STATE::DAMAGE, &PlayerDesc));

    // Z키
    m_pFsmCom->Add_State(CPlayer_FirstSkillState::Create(PLAYER_STATE::SKILL_1, &PlayerDesc));
    // X키
    m_pFsmCom->Add_State(CPlayer_SecondSkillState::Create(PLAYER_STATE::SKILL_2, &PlayerDesc));


    Register_CoolTime();

    CPlayer_IdleState::IDLE_ENTER_DESC enter{};
    enter.iAnimation_Idx = m_Action_AnimMap[TEXT("IDLE")];

    CPlayer_RunState::RUN_ENTER_DESC Run{};
    Run.iAnimation_Idx = PLAYER_ANIM_RUN_F_LOOP;
    m_pFsmCom->Change_State(PLAYER_STATE::RUN, &Run);
    m_pFsmCom->Change_State(PLAYER_STATE::IDLE, &enter);
    return S_OK;
}

// 애니메이션 쿨타임 보다. Ratio로 판단하는게 더맞을듯.
void CPlayer::Register_CoolTime()
{
    _float fTimeDelta = m_pGameInstance->Get_TimeDelta();
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::IDLE, 0.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::WALK, 0.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::RUN, 0.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::DODGE, 0.7f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::STRONG_ATTACK, 1.5f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::ATTACK, 1.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::GUARD, 0.5f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::SKILL_1, 10.f);
    m_pFsmCom->Register_StateCoolTime(PLAYER_STATE::SKILL_2, 10.f);


    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::IDLE, 0.f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::WALK, 0.f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::RUN, 0.f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::DODGE, 0.7f);

    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::SKILL_1, 4.f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::SKILL_2, 4.f);
    //m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::SKILL_2, 4.f);

    // 총 재생 시간.
    _float fCalcDuration = m_pModelCom->Get_AnimationDuration(m_Action_AnimMap[TEXT("STRONG_ATTACK1")]) /
        m_pModelCom->Get_AnimationTickPersecond(m_Action_AnimMap[TEXT("STRONG_ATTACK1")]);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::STRONG_ATTACK
        , fCalcDuration * 0.6f / m_pModelCom->Get_AnimSpeed(m_Action_AnimMap[TEXT("STRONG_ATTACK1")]));

    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::ATTACK, 0.8f);
    m_pFsmCom->Register_StateExitCoolTime(PLAYER_STATE::GUARD, 0.4f);
}

HRESULT CPlayer::Bind_ShaderReosurces()
{
      
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{   
    CPlayerWeapon::PLAYER_WEAPON_DESC Weapon{};
    Weapon.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Weapon.pSocketMatrix = m_pModelCom->Get_BoneMatrix("IKSocket_RightHandAttach");
    Weapon.pOwner = this;
    Weapon.eCurLevel = m_eCurLevel;
    Weapon.fAttackPower = m_Stats.fAttackPower;
    Weapon.eShaderPath = MESH_SHADERPATH::DEFAULT;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Com_Weapon"),
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Weapon")
        , reinterpret_cast<CPartObject**>(& m_pPlayerWeapon), &Weapon)))
    {
        CRASH("Failed Create Weapon");
        return E_FAIL;
    }

    //m_pPlayerWeapon->Set_AttackPower(m_Stats.fAttackPower);
    

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
    CContainerObject::Destroy();
}

void CPlayer::Free()
{
    CContainerObject::Free();
    Safe_Release(m_pFsmCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pPlayerWeapon);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);
        
}


#ifdef _DEBUG
void CPlayer::ImGui_Render()
{
    ImGuiIO& io = ImGui::GetIO();

    // 기존 Player Debug Window
    ImVec2 windowPos = ImVec2(10.f, io.DisplaySize.y - 350.f);
    ImVec2 windowSize = ImVec2(300.f, 300.f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("Player Debug", nullptr, ImGuiWindowFlags_NoCollapse);

    _float3 vPos = {};
    _vector vMine = m_pTransformCom->Get_State(STATE::POSITION);
    XMStoreFloat3(&vPos, vMine);
    ImGui::Text("Player Pos: (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);

    if (m_pLockOn_Target)
    {
        _float3 vLockOnPos = {};
        _vector vLockOn = m_pLockOn_Target->Get_Transform()->Get_State(STATE::POSITION);
        XMStoreFloat3(&vLockOnPos, vLockOn);
        ImGui::Text("LockOn Target Pos: (%.2f, %.2f, %.2f)", vLockOnPos.x, vLockOnPos.y, vLockOnPos.z);

        _float fDistance = XMVectorGetX(XMVector3Length(vLockOn - vMine));
        ImGui::Text("LockOn Distance : (%.2f)", fDistance);


        _float fHp = dynamic_cast<CMonster*>(m_pLockOn_Target)->Get_MonsterStat().fHP;
        ImGui::Text("LockOn HP : (%.2f)", fHp);
    }


    // ==== Player Anim Debug 추가 ====
    _uint iAnimationIdx = m_pModelCom->Get_CurrentAnimationIndex();
    ImGui::Separator();
    ImGui::Text("=== Player Animation Idx ====");

    ImGui::Text("Cur Anim Idx : %d", iAnimationIdx);
    ImGui::Text("Cur Anim Speed : (%.2f)", m_pModelCom->Get_AnimSpeed(iAnimationIdx));


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

    ImGui::Separator();
    ImGui::Text("=== Camera Pos ===");

    _float3 vCamPos = { };
    XMStoreFloat3(&vCamPos, XMLoadFloat4(m_pGameInstance->Get_CamPosition()));

    if (m_isLockOn)
    {
        ImGui::Separator();
        ImGui::Text("Lock On Camera Pos : (%.2f, %.2f, %.2f)", vCamPos.x, vCamPos.y, vCamPos.z);
    }
    else
    {
        ImGui::Separator();
        ImGui::Text("Normal Camera Pos : (%.2f, %.2f, %.2f)", vCamPos.x, vCamPos.y, vCamPos.z);
    }

    ImGui::End();

    // Navigation 렌더링
    if (m_pNavigationCom)
        m_pNavigationCom->Render();
}
#endif // _DEBUG