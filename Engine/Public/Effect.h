#pragma once
/*
* 모든 이펙트 객체가 상속하는 객체들.
*/
NS_BEGIN(Engine)
class ENGINE_DLL CEffect abstract : public CGameObject
{
public:
	enum class EAttachType : _uint { WORLD, PARENT };

public:
	typedef struct tagEffectCloneDesc : public CGameObject::GAMEOBJECT_DESC
	{

	}EFFECTCLONE_DESC;

	typedef struct tagEffectActivateDesc 
	{
		_float4 vStartPos;
		// 생성 시 부모의 TransformComponent를 받아옵니다.
		class CTransform* pOwnerTransform = { nullptr };
		EAttachType eAttachType = { EAttachType::WORLD };

	}EFFECTACITVATE_DESC;
protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;





#pragma region 기본 함수들
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
#pragma endregion

#pragma region Polling 객체마다 다르게 정의해야하는 것들.
public:
	virtual void OnMoved_ToObjectManager(); // 객체마다 다르게 정의해야함.
public:
	virtual void OnActivate(void* pArg = nullptr); // 오브젝트가 활성화되어 사용 준비되었을 때 호출.
	virtual void OnDeActivate(); // 오브젝트가 비활성화되어 풀로 돌아갈 때 호출.

#pragma endregion


#pragma region Effect 객체가 소유해야할 것들.
protected:
	class CTransform* m_pOwnerTransform = { nullptr };
	EAttachType m_eAttachType = {};
	_float4x4 m_CombinedWorldMatrix = {};
#pragma endregion

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Destroy();
	virtual void Free();
};
NS_END

