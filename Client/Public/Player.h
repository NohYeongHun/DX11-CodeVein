#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CPlayer final : public CGameObject
{
public:
	typedef struct tagPlayerDesc : public CGameObject::GAMEOBJECT_DESC
	{
		const _char* pModelFilePath;
	}PLAYER_DESC;

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

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
#pragma endregion


private:
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };


private:
	HRESULT Ready_Components(PLAYER_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;

};
NS_END

