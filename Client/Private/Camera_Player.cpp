#include "Camera_Player.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& Prototype)
	: CCamera(Prototype)
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Player::Initialize_Clone(void* pArg)
{
	CAMERA_PLAYER_DESC* pDesc = static_cast<CAMERA_PLAYER_DESC*>(pArg);

	m_fMouseSensor = pDesc->fMouseSensor;
	m_pTarget = pDesc->pTarget;

	// 타겟 기준 뒤에서 바라보는 오프셋 (플레이어 뒤쪽 5미터, 위쪽 3미터)
	XMStoreFloat4(&m_vTargetOffset, XMVectorSet(0.f, 3.f, -5.f, 0.f));

	// 초기 Yaw 각도 (0도 = 플레이어 정면을 바라봄)
	m_fYaw = 0.f;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCamera_Player::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (nullptr == m_pTarget)
		return;

	// 1. 타겟(플레이어) 위치 가져오기
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	// 2. 마우스 입력 처리 (클라이언트 영역 내에서만)
	POINT ptMouse = m_pGameInstance->Get_Mouse_Cursor(g_hWnd);
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient);

	if (PtInRect(&rcClient, ptMouse))
	{
		// 마우스 X축 이동으로 Y축 중심 회전 (Transform의 Turn 함수 사용)
		if (_long MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
		{
			_float fAngle = (_float)MouseMove * m_fMouseSensor * fTimeDelta;
			m_fYaw += fAngle;
		}
	}

	// 3. Y축 회전 행렬로 오프셋 회전
	_matrix matRotY = XMMatrixRotationY(m_fYaw);
	_vector vRotatedOffset = XMVector3TransformNormal(XMLoadFloat4(&m_vTargetOffset), matRotY);

	// 4. 카메라 위치 = 플레이어 위치 + 회전된 오프셋
	_vector vCameraPos = vTargetPos + vRotatedOffset;
	m_pTransformCom->Set_State(STATE::POSITION, vCameraPos);

	// 5. Transform의 LookAt 함수를 사용해서 플레이어를 바라보도록 설정
	_float3 vTargetPosFloat3;
	XMStoreFloat3(&vTargetPosFloat3, vTargetPos);
	m_pTransformCom->LookAt(vTargetPosFloat3);

	// 6. 파이프라인 업데이트
	__super::Update_PipeLines();
}

void CCamera_Player::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CCamera_Player::Render()
{
	return S_OK;
}

// Getter 함수들 (필요시 추가)
_float CCamera_Player::Get_Yaw() const
{
	return m_fYaw;
}

void CCamera_Player::Set_Yaw(_float fYaw)
{
	m_fYaw = fYaw;
}

void CCamera_Player::Add_Yaw(_float fYawDelta)
{
	m_fYaw += fYawDelta;
}

void CCamera_Player::Set_TargetOffset(_float4 vOffset)
{
	m_vTargetOffset = vOffset;
}

CCamera_Player* CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Player* pInstance = new CCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Player::Clone(void* pArg)
{
	CCamera_Player* pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	__super::Free();
}