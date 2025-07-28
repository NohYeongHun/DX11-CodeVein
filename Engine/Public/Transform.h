#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{

public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype) = delete;
	virtual ~CTransform() = default;

public:
	void Set_State(STATE eState, _fvector vState);

	_vector Get_State(STATE eState)  {
		Update_WorldMatrix(); // 최신화 보장
		return XMLoadFloat4(reinterpret_cast<const _float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]));
	}

	_float3 Get_Scaled() {
		Update_WorldMatrix(); // 최신화 보장
		return _float3(
			XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
			XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
		);
	}

	_matrix Get_WorldMatrix_Inverse() {
		Update_WorldMatrix();  // 최신화를 보장한다.
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	_matrix Get_WorldMatrix() {
		Update_WorldMatrix();  // 최신화를 보장한다.
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	const _float4x4* Get_WorldMatrixPtr() {
		Update_WorldMatrix();  // 최신화를 보장한다.
		return &m_WorldMatrix;
	}

	_vector Get_LookDirection_NoPitch()
	{
		_vector vLook = XMVector3Normalize(Get_State(STATE::LOOK));
		vLook = XMVectorSetY(vLook, 0.f);
		return XMVector3Normalize(vLook);
	}

	_vector Get_RightDirection_NoPitch()
	{
			_vector vRight = XMVector3Normalize(Get_State(STATE::RIGHT));
		vRight = XMVectorSetY(vRight, 0.f);  // Pitch 제거
		return XMVector3Normalize(vRight);
	}

	void Set_Quaternion(_fvector qRot)
	{
		m_QuatRotation = XMQuaternionNormalize(qRot);
		m_bIsDirty = true; // 월드 행렬
	}

	_vector Get_Quaternion()
	{
		return m_QuatRotation;
	}

	_float  GetYawFromQuaternion() const;
	_float  GetPitchFromQuaternion() const;
	_float  GetRollFromQuaternion() const;
	

	

	void Move_Direction(_vector vDir, _float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);

public:
	HRESULT Bind_Shader_Resource(class CShader* pShader, const _char* pConstantName);
	void Update_WorldMatrix();

	/* 누적 회전 적용. */
public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);

public:
	void Set_Position(const _float3& vPos);
	void Set_Scale(const XMFLOAT3& vScale);
	void Scale(const XMFLOAT3& vScale);
	void Set_ScaleX(float fX) { m_vScale.x = fX; m_bIsDirty = true; }
	void Set_ScaleY(float fY) { m_vScale.y = fY; m_bIsDirty = true; }
	void Set_ScaleZ(float fZ) { m_vScale.z = fZ; m_bIsDirty = true; }
	_float3 Get_Scale();

	void Add_Rotation(_float fPitch, _float fYaw, _float fRoll);
	_float3 Get_EulerAngles() const;
	void LookAt(const _float3& vTargetPos, const _float3& vUp = { 0.f, 1.f, 0.f });
	void LookAt_YawOnly(_vector vTargetDir);

	void Set_ParentMatrix(const _float4x4* pParentWorldMatrix);

	void Turn(_fvector vAxis, _float fAngle);
	/*void Update_Transform();
	void Set_Parent(CTransform* pTarget);*/

private:
	const _float4x4*		m_pParentWorldMatrix = {};

	_float4x4	    m_CombineMatrix = {};
	_float4x4		m_WorldMatrix = {};
	_float3			m_vPosition = {};
	_float3			m_vScale = {};
	_vector			m_QuatRotation = {}; // 누적 쿼터니언 회전.

	_float			m_fSpeedPerSec = {};
	_float			m_fRotationPerSec = {};

	_float			m_fPitch = {}; // X축 회전
	_float			m_fYaw = {};   // Y축 회전
	_float			m_fRoll = {}; // Z축 회전.

private:
	_bool m_bIsDirty = { false }; // 갱신을 해주어야하는가? 체크


public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) { return nullptr; };
	virtual void Free() override;
};

NS_END