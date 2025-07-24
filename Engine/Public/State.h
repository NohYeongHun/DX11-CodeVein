#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CState abstract : public CBase
{
public:
	typedef struct tagStateDesc
	{
		class CFsm* pFsm = { nullptr };
		class CGameObject* pOwner = { nullptr };
	}STATE_DESC;
;
protected:
	explicit CState();
	virtual ~CState() = default;

public:
	const _uint Get_StateNum() { return m_iStateNum;  }
	
public:
	// 시작 초기화
	virtual HRESULT Initialize(_uint iStateNum, void* pArg) PURE;

	// State 시작 시.
	virtual void Enter(void* pArg) PURE;

	// State 실행 로직
	virtual void Update(_float fTimeDelta) PURE;

	// State Exit
	virtual void Exit() PURE;

	// State 초기값으로 설정
	virtual void Reset() PURE;


protected:
	class CFsm* m_pFsm = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	_float m_fCurrentTime = { 0.f };
	_uint m_iStateNum = { 0 };
		
public:
	virtual void Free() override;
	

};
NS_END

