#pragma once

#include "Base.h"

/* 사본 게임오브젝트를 생성하여 보관한다. */
/* 보관 : 레벨별로, 사용자의 정의에 따라 그룹지어서 */


NS_BEGIN(Engine)

using LayerTable = map<const _wstring, class CLayer*>;

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
#pragma region ENGINE제공
	class CComponent* Get_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CLayer* Get_Layer(_uint iLayerIndex, const _wstring& strLayerTag);
	class CGameObject* Get_GameObject(_uint iLayerIndex, const _wstring& strLayerTag, _uint iIndex);

	/* 맵 툴에 현재 레벨의 레이어 정보를 담아서 전달합니다. 읽기 전용 */
	const LayerTable& Export_EditLayer(_uint iLayerLevelIndex);
	void Request_EditObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, uint32_t objID, const EditPayload& payload);
	void Request_DeleteObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, uint32_t objID);

	/* 특정 Layer Picking 결과 반환.*/
	RAYHIT_DESC Get_PickingLocalObject(_uint iLayerLevelIndex, const _wstring strLayerTag, _float* pOutDist);
	MODEL_PICKING_INFO Get_PickingLocalObject(_uint iLayerLevelIndex, const _wstring strLayerTag);
#pragma endregion

	

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_uint				 m_iNumLevels = {};
	LayerTable*		m_pLayers = {};
	
#pragma region 삭제 처리할 객체들을 Layer에서 빼서 모아둡니다. => IsDestroy 객체들.
private:
	list<class CGameObject*> m_DestroyObjects = {};
#pragma endregion




private:
	class CLayer* Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);

	void Edit_Transform(_uint iLayerLevelIndex, const _wstring& strLayerTag, uint32_t objID, const EditPayload& payload);
	

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END