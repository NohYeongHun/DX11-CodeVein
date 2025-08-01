#pragma once
#include "GameObject.h"

/* Map Tool 상에서 배치하고 생성하고 수정하는 Class */
/* Client용 Class가 하나 더 필요함. */
NS_BEGIN(Tool)
class CMap_Part final : public CGameObject
{
public:

private:
	CMap_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_Part(const CMap_Part& Prototype);
	virtual ~CMap_Part() = default;
	
/* 충돌된 Map Part는 Imgui에서 조작할 수 있는 Transform 주소를 반환합니다. */
public:
	//const MAP_PART_INFO& Save_NonAminModel(_fmatrix PreTransformMatrix);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual HRESULT Initialize_Load(void* pArg);

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

#pragma region 충돌 함수 정의
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);

	/* Ray에 맞았는지 확인. */
	//virtual const _bool Is_Ray_LocalHit(_float* pOutDist) override;
#pragma endregion


private:
	class CTool_Model* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	const _tchar* m_pModelTag = { nullptr };
	_wstring m_PartName = {  };
	int m_iPartID = {};


private:
	HRESULT Ready_Components(MODEL_CREATE_DESC* pDesc);
	HRESULT Ready_Transform(MODEL_CREATE_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CMap_Part* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;

};
NS_END

