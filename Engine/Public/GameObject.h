#pragma once

#include "Transform.h"

/* 게임오브젝트들의 부모가 되는 클래스. */

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{

public:
	typedef struct tagGameObject : public CTransform::TRANSFORM_DESC
	{

	}GAMEOBJECT_DESC;
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual _bool Picking(_float3* PickingPoint) { return false; }

public:
	class CComponent* Get_Component(const _wstring& strComponentTag);
	HRESULT Change_Component(const _wstring& strComponentTag, CComponent** ppOut, CComponent* pChangeComponent);

	class CTransform* Get_Transform() { return m_pTransformCom; }

	virtual void RootMotion_Translate(_fvector vTranslate);
	

public:
	virtual void Compute_CamDistance(_fvector vWorldPos);
	_float Get_CamDistance() const { return m_fCamDistance; }

private:
	_float m_fCamDistance = {};

public:
	const _wstring& Get_ObjectTag();

#pragma region 생존 여부 확인
public:
	const _bool Is_Destroy() { return m_IsDestroy; }
	void Set_Destroy(_bool IsDestroy) { m_IsDestroy = IsDestroy; }
	
	//const _bool Is_Active() { return m_IsActive; }
	//const _bool Is_Dead() { return m_IsDead; }
	//void Set_Active(_bool IsActive) { m_IsActive = IsActive; }
	//void Set_Dead(_bool IsDead) { m_IsDead = IsDead; }

	//const _bool Is_Active() { return m_IsActive; }
	//const _bool Is_Dead() { return m_IsDead; }
	//void Set_Active(_bool IsActive) { m_IsActive = IsActive; }
	//void Set_Dead(_bool IsDead) { m_IsDead = IsDead; }

#pragma endregion


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Finalize_PreUpdate(_float fTimeDelta) {}; // Update의 마지막 부근에 사용되는 함수

	virtual void Update(_float fTimeDelta);
	virtual void Finalize_Update(_float fTimeDelta) {} ; // Update의 마지막 부근에 사용되는 함수

	virtual void Late_Update(_float fTimeDelta);
	virtual void Finalize_LateUpdate(_float fTimeDelta) {}; // Update의 마지막 부근에 사용되는 함수
	virtual HRESULT Render();



#pragma region 충돌 함수 정의
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);

	virtual const _bool Is_Ray_LocalHit(_float3* pOutLocalPos, _float3* pOutLocalNormal, _float* pOutDist);
	virtual const _bool Is_Ray_LocalHit(MODEL_PICKING_INFO* pPickingInfo, _float* pOutDist);
#pragma endregion



protected:
	// 기본적으로 있어야 할 것들.
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CTransform*			m_pTransformCom = { nullptr };

	map<const _wstring, class CComponent*>		m_Components;

	//_bool m_IsActive = { true };
	_bool m_IsDestroy = { false };


protected:
	// 부가된 것들.
	_wstring m_strObjTag = {};


#ifdef _DEBUG
protected:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
	_float4 m_vColor = { 0.f, 1.f, 0.f, 1.f }; // 기본 녹색

public:
	virtual HRESULT  Initialize_Debug();

public:
	virtual HRESULT  BoundingBoxRender(const BOUNDING_BOX& box, _matrix WorldMatrix);
	virtual void    Render_AABB(const BOUNDING_BOX& box);
#endif // _DEBUG

protected:
	/*원형컴포넌트를 찾아서 복제한다. */
	/*map컨테이너에 보관한다.  */
	/*자식의 멤버변수에도 저장한다. */
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, 
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);




public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Destroy(); 
	virtual void Free() override;

};

NS_END