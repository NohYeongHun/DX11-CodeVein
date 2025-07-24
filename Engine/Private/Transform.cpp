#include "Transform.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
	
{

}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

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
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

// 매프레임 Update 되어야할 정보.
void CTransform::Update_Transform()
{

}



/* 크기 1기준으로 들어온 vScale 값을 곱해줍니다.
* => 누적이 아닙니다. 
*/
void CTransform::Scale(_float3 vScale)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * vScale.z);
}

/*
* 크기 기준으로 계속 곱해진다. 
* => 누적 곱.
*/
void CTransform::Scaling(_float3 vScale)
{
	Set_State(STATE::RIGHT, Get_State(STATE::RIGHT) * vScale.x);
	Set_State(STATE::UP, Get_State(STATE::UP) * vScale.y);
	Set_State(STATE::LOOK, Get_State(STATE::LOOK) * vScale.z);
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE::RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector4Transform(vLook, RotationMatrix));
	


}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vLook = Get_State(STATE::LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	Set_State(STATE::RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector4Transform(vLook, RotationMatrix));

}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(STATE::POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Set_Rotation(_float3 vAngle)
{
}


//void CTransform::Set_Rotation(_float3 vAngle)
//{
//	// Get current RotationMatrix from the WorldMatrix by decomposition.
//	_vector vScale, vRotationQuat, vTranslation;
//	XMMatrixDecompose(&vScale, &vRotationQuat, &vTranslation, Get_WorldMatrix());
//	_matrix RotationMatrix = XMMatrixRotationQuaternion(vRotationQuat);
//
//	// Multiply the WorldMatrix by the Inverse of current RotationMatrix (to get a WorldMatrix without any Rotation)
//	_matrix InverseRotationMatrix = XMMatrixInverse(nullptr, RotationMatrix);
//	_matrix WorldMatrixWithoutRotation = XMMatrixMultiply(Get_WorldMatrix(), InverseRotationMatrix);
//
//	// Make a NewRotationMatrix with new angle values
//	_matrix NewRotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(vAngle.x), XMConvertToRadians(vAngle.y), XMConvertToRadians(vAngle.z));
//	m_fPitch = vAngle.x;
//	m_fYaw = vAngle.y;
//	m_fRoll = vAngle.z;
//
//	// Set NewRotationMatrix to WorldMatrixWithoutRotation
//	Set_State(STATE::RIGHT, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[0], NewRotationMatrix));
//	Set_State(STATE::UP, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[1], NewRotationMatrix));
//	Set_State(STATE::LOOK, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[2], NewRotationMatrix));
//}

void CTransform::Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vMoveDir = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vMoveDir));

	if(fDistance >= fLimit)
		vPosition += XMVector3Normalize(vMoveDir) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

//// 회전축에 따른 축 뒤틀림 문제를 해결하기 위함.
//void CTransform::Update_Transform()
//{
//	// 매 프레임 마다 크 자 이 공 부 를 통해 회전축 업데이트
//	if (m_pParent != nullptr)
//		m_pParent->Update_Transform();
//}
//
//void CTransform::Set_Parent(CTransform* pTarget)
//{
//	if (pTarget == m_pParent)
//		return;
//
//	Update_Transform();
//
//}

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
