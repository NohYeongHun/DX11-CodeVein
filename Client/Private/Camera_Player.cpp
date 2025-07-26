#include "Camera_Free.h"

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

	XMStoreFloat4(&m_vTargetOffset, (XMVectorSet(0.f, 3.f, -5.f, 0.f))); // 타겟 기준 뒤에서 바라봄

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;


	return S_OK;
}

// 카메라에 대한 이동 전환은 Priority_Update에서 완료 후 Pipe Line에 행렬이 전달됨.
void CCamera_Player::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	
}

void CCamera_Player::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	// 1. 타겟 위치
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	POINT ptMouse = m_pGameInstance->Get_Mouse_Cursor(g_hWnd); // ← 여기서 가져옴
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient); // 클라이언트 영역 (0,0) ~ (width,height)

	if (PtInRect(&rcClient, ptMouse)) // 마우스가 클라이언트 내부일 때만 회전 허용
	{
		// 2. 마우스 입력으로 Yaw 회전 누적
		if (_long MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
		{
			m_fYaw += (_float)MouseMove * m_fMouseSensor * fTimeDelta;
		}
	}

	

	// 3. 회전 행렬 (Yaw)
	_matrix matRotY = XMMatrixRotationY(m_fYaw);
	_vector vRotatedOffset = XMVector3TransformNormal(XMLoadFloat4(&m_vTargetOffset), matRotY);

	// 4. 카메라 위치 = 타겟 위치 + 회전된 오프셋
	_vector vCamPos = vTargetPos + vRotatedOffset;
	m_pTransformCom->Set_State(STATE::POSITION, vCamPos);

	// 5. 카메라가 타겟을 바라보는 방향 계산
	_vector vLook = XMVector3Normalize(vTargetPos - vCamPos);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Cross(vLook, vRight);

	m_pTransformCom->Set_State(STATE::LOOK, vLook);
	m_pTransformCom->Set_State(STATE::RIGHT, vRight);
	m_pTransformCom->Set_State(STATE::UP, vUp);


	__super::Update_PipeLines();
}

void CCamera_Player::Late_Update(_float fTimeDelta)
{
	// 여기서 마우스 이전 프레임 위치 저장.
	
	__super::Late_Update(fTimeDelta);
}

HRESULT CCamera_Player::Render()
{
	return S_OK;
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

