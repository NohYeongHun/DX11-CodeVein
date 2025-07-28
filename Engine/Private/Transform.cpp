#include "Transform.h"


CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]), vState);

	switch (eState)
	{
	case STATE::POSITION:
		XMStoreFloat3(&m_vPosition, vState);
		m_bIsDirty = true;
		break;

	case STATE::RIGHT:
	case STATE::UP:
	case STATE::LOOK:
	{
		// Right, Up, Look 중 하나라도 수정된 경우
		// 전체 회전 행렬을 추출해서 쿼터니언으로 재계산
		_vector vRight = XMLoadFloat4(reinterpret_cast<const _float4*>(&m_WorldMatrix.m[0]));
		_vector vUp = XMLoadFloat4(reinterpret_cast<const _float4*>(&m_WorldMatrix.m[1]));
		_vector vLook = XMLoadFloat4(reinterpret_cast<const _float4*>(&m_WorldMatrix.m[2]));

		XMMATRIX matRot = {
			XMVector3Normalize(vRight),
			XMVector3Normalize(vUp),
			XMVector3Normalize(vLook),
			XMVectorSet(0.f, 0.f, 0.f, 1.f)
		};

		m_QuatRotation = XMQuaternionRotationMatrix(matRot);
		m_bIsDirty = true; // ✅ 수정: 월드 행렬 갱신 필요
	}
	break;
	default:
		break;
	}

	// ✅ 수정: POSITION의 경우만 m_bIsDirty를 false로 하지 않음
	if (eState != STATE::POSITION)
	{
		m_bIsDirty = true; // 회전이나 다른 상태 변경 시 갱신 필요
	}
}

_float CTransform::GetYawFromQuaternion() const
{
	_float w = XMVectorGetW(m_QuatRotation);
	_float x = XMVectorGetX(m_QuatRotation);
	_float y = XMVectorGetY(m_QuatRotation);
	_float z = XMVectorGetZ(m_QuatRotation);

	return atan2f(2.0f * (w * y + x * z), 1.0f - 2.0f * (y * y + z * z));
}

_float CTransform::GetPitchFromQuaternion() const
{
	float w = XMVectorGetW(m_QuatRotation);
	_float x = XMVectorGetX(m_QuatRotation);
	_float y = XMVectorGetY(m_QuatRotation);
	_float z = XMVectorGetZ(m_QuatRotation);

	// Pitch (X축 회전) 계산
	_float sinp = 2.0f * (w * x - y * z);
	if (fabsf(sinp) >= 1.0f)
		return copysignf(XM_PIDIV2, sinp); // Gimbal lock 방지
	else
		return asinf(sinp);
}

_float CTransform::GetRollFromQuaternion() const
{
	_float w = XMVectorGetW(m_QuatRotation);
	_float x = XMVectorGetX(m_QuatRotation);
	_float y = XMVectorGetY(m_QuatRotation);
	_float z = XMVectorGetZ(m_QuatRotation);

	// Roll (Z축 회전) 계산
	return atan2f(2.0f * (w * z + x * y), 1.0f - 2.0f * (x * x + z * z));
}

void CTransform::Move_Direction(_vector vDir, _float fTimeDelta)
{
	_vector vMovement = vDir * (m_fSpeedPerSec * fTimeDelta);

	m_vPosition.x += XMVectorGetX(vMovement);
	m_vPosition.y += XMVectorGetY(vMovement);
	m_vPosition.z += XMVectorGetZ(vMovement);

	m_bIsDirty = true;
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_vPosition = { 0.f, 0.f, 0.f };
	m_vScale = { 1.f, 1.f, 1.f };
	m_QuatRotation = XMQuaternionIdentity();

	return S_OK;
}

HRESULT CTransform::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_Shader_Resource(CShader* pShader, const _char* pConstantName)
{
	// ✅ 수정: Shader 바인딩 전에 반드시 월드 행렬 갱신
	Update_WorldMatrix();
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

// 매프레임 업데이트
void CTransform::Update_WorldMatrix()
{
	_matrix matScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	_matrix matRotation = XMMatrixRotationQuaternion(m_QuatRotation);
	_matrix matTranslation = XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z);
	_matrix world = matScale * matRotation * matTranslation;

	XMStoreFloat4x4(&m_WorldMatrix, world);

	// ✅ 수정: 갱신 완료 후 플래그 해제
	m_bIsDirty = false;
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	Update_WorldMatrix();
	_vector vLook = XMVector3Normalize(Get_State(STATE::LOOK));
	vLook *= fTimeDelta * m_fSpeedPerSec;
	m_vPosition.x += XMVectorGetX(vLook);
	m_vPosition.y += XMVectorGetY(vLook);
	m_vPosition.z += XMVectorGetZ(vLook);
	m_bIsDirty = true;
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	Update_WorldMatrix();
	_vector vLook = XMVector3Normalize(Get_State(STATE::LOOK));
	vLook *= -fTimeDelta * m_fSpeedPerSec;
	m_vPosition.x += XMVectorGetX(vLook);
	m_vPosition.y += XMVectorGetY(vLook);
	m_vPosition.z += XMVectorGetZ(vLook);
	m_bIsDirty = true;
}

void CTransform::Go_Left(_float fTimeDelta)
{
	Update_WorldMatrix();
	_vector vRight = XMVector3Normalize(Get_State(STATE::RIGHT));
	vRight *= -fTimeDelta * m_fSpeedPerSec;
	m_vPosition.x += XMVectorGetX(vRight);
	m_vPosition.y += XMVectorGetY(vRight);
	m_vPosition.z += XMVectorGetZ(vRight);
	m_bIsDirty = true;
}

void CTransform::Go_Right(_float fTimeDelta)
{
	Update_WorldMatrix();
	_vector vRight = XMVector3Normalize(Get_State(STATE::RIGHT));
	vRight *= fTimeDelta * m_fSpeedPerSec;
	m_vPosition.x += XMVectorGetX(vRight);
	m_vPosition.y += XMVectorGetY(vRight);
	m_vPosition.z += XMVectorGetZ(vRight);
	m_bIsDirty = true;
}

_float3 CTransform::Get_EulerAngles() const
{
	_float3 angles;
	_vector quat = m_QuatRotation;
	_vector euler = XMQuaternionRotationMatrix(XMMatrixRotationQuaternion(quat));
	XMStoreFloat3(&angles, euler);

	return angles; // 라디안 단위
}

void CTransform::LookAt(const _float3& vTargetPos, const _float3& vUp)
{
	// 현재 위치와 타겟 위치 벡터
	_vector vPos = XMLoadFloat3(&m_vPosition);
	_vector vTarget = XMLoadFloat3(&vTargetPos);
	_vector vUpVec = XMLoadFloat3(&vUp);

	// Look 방향 (정규화된 방향)
	_vector vLook = XMVector3Normalize(vTarget - vPos);

	// Right = Up x Look
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUpVec, vLook));

	// Up = Look x Right
	_vector vTrueUp = XMVector3Cross(vLook, vRight);

	// 회전 행렬 구성 (열벡터가 아님, DirectX는 행우선이므로 행벡터 순서임)
	_matrix matRot = XMMatrixSet(
		XMVectorGetX(vRight), XMVectorGetY(vRight), XMVectorGetZ(vRight), 0.f,
		XMVectorGetX(vTrueUp), XMVectorGetY(vTrueUp), XMVectorGetZ(vTrueUp), 0.f,
		XMVectorGetX(vLook), XMVectorGetY(vLook), XMVectorGetZ(vLook), 0.f,
		0.f, 0.f, 0.f, 1.f
	);

	// 회전 행렬 → 쿼터니언 변환
	m_QuatRotation = XMQuaternionRotationMatrix(matRot);

	// Transform 상태 변경됨 → 월드행렬 재계산 필요
	m_bIsDirty = true;
}

void CTransform::LookAt_YawOnly(_vector vTargetDir)
{
	// 현재 위치 기준 방향 제거
	vTargetDir = XMVectorSetY(vTargetDir, 0.f);
	vTargetDir = XMVector3Normalize(vTargetDir);

	_vector vForward = XMVectorSet(0.f, 0.f, 1.f, 0.f); // 기본 전방

	// 회전 쿼터니언 생성 (Yaw만 반영)
	_vector qRot = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMVectorZero(), vTargetDir, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	m_QuatRotation = qRot;
	m_bIsDirty = true;
}

void CTransform::Set_ParentMatrix(const _float4x4* pParentWorldMatrix)
{
	m_pParentWorldMatrix = pParentWorldMatrix;
}

void CTransform::Set_Position(const _float3& vPos)
{
	m_vPosition = vPos;
	m_bIsDirty = true;
}

void CTransform::Set_Scale(const XMFLOAT3& vScale)
{
	m_vScale = vScale;
	m_bIsDirty = true;
}

void CTransform::Scale(const XMFLOAT3& vScale)
{
	m_vScale = vScale;
	m_bIsDirty = true;
}

void CTransform::Turn(_fvector vAxis, _float fAngle)
{
	_vector qRot = XMQuaternionRotationAxis(vAxis, fAngle); // 회전 쿼터니언 생성
	m_QuatRotation = XMQuaternionNormalize(XMQuaternionMultiply(qRot, m_QuatRotation)); // 누적
	m_bIsDirty = true;
}

_float3 CTransform::Get_Scale()
{
	return m_vScale;
}

/* Add Rotation */
void CTransform::Add_Rotation(_float fPitch, _float fYaw, _float fRoll)
{
	// 1. 회전 쿼터니언 생성 (Yaw-Pitch-Roll 순서)
	_vector deltaQuat = XMQuaternionRotationRollPitchYaw(fPitch, fYaw, fRoll);

	// 2. 기존 쿼터니언과 곱해 누적 (순서 주의: 새 회전을 뒤에 곱한다)
	m_QuatRotation = XMQuaternionNormalize(XMQuaternionMultiply(m_QuatRotation, deltaQuat));
	m_bIsDirty = true;
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}