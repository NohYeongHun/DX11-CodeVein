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
	class CComponent* Get_Component(const _wstring& strComponentTag);
	HRESULT Change_Component(const _wstring& strComponentTag, CComponent** ppOut, CComponent* pChangeComponent);

public:
	const _wstring& Get_ObjectTag();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

#pragma region 충돌 함수 정의
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);

	virtual const _bool Is_Ray_LocalHit(_float* pOutDist);
#pragma endregion



protected:
	// 기본적으로 있어야 할 것들.
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CTransform*			m_pTransformCom = { nullptr };

	map<const _wstring, class CComponent*>		m_Components;


protected:
	// 부가된 것들.
	_wstring m_strObjTag = {};

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