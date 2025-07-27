#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CPlayer final : public CGameObject
{

#pragma region PLAYER STATE 정의 STATE != ANIMATION
public:
	// Add_State 순서대로 넣습니다.
	enum PLAYER_STATE : _int
	{
		IDLE = 0, WALK, RUN, DODGE,
		SWORD_STRONG_ATTACK,
		SWORD_IDLE,
		STATE_END
	};
#pragma endregion

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
	void Move_By_Camera_Direction_8Way(DIR eDir, _float fTimeDelta, _float fSpeed);
	
	void UpdatePlayerRotationSmooth(_vector vMoveDir, _float fTimeDelta);
	void Debug_CameraVectors();

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



#pragma region PLAYER 함수 정의.
public:
	void HandleState(_float fTimeDelta);
	void Change_Animation(_uint iAnimationIndex, _bool isLoop);
	_vector  Calculate_Move_Direction(DIR eDir);
	
	const _bool IsLockOn() { return m_isLockOn; }

public:
	// 키 입력 상태 확인 함수들
	uint16_t Get_KeyInput() { return m_KeyInput;  }

	_bool Is_KeyPressed(PLAYER_KEY ePlayerKey) const { 
		return (m_KeyInput & static_cast<uint16_t>(ePlayerKey)) != 0;
	}

	// 움직임 키 확인
	_bool Is_MovementKeyPressed() const {
		return Is_KeyPressed(PLAYER_KEY::MOVE_FORWARD) ||
			Is_KeyPressed(PLAYER_KEY::MOVE_BACKWARD) ||
			Is_KeyPressed(PLAYER_KEY::MOVE_LEFT) ||
			Is_KeyPressed(PLAYER_KEY::MOVE_RIGHT);
	}
	DIR Get_Direction() { return m_eCurrentDirection; }

private:
	DIR Calculate_Direction();
	void Update_KeyInput();
#pragma endregion


private:
	// Load Model;

	//class CModel* m_pModelCom = { nullptr };
	class CLoad_Model* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CFsm* m_pFsmCom = { nullptr };
	
private:
	/* 상태 정의 */
	_bool m_isLockOn = { false };


private:
	/* 키 코드 상태 16개 까지 관리 */
	uint16_t m_KeyInput = {};
	// 키 매핑 테이블 (PLAYER_KEY -> DirectInput 키코드)
	static const _uint m_KeyboardMappingsCount;
	static const pair<PLAYER_KEY, _ubyte> m_KeyboardMappings[];
	DIR m_eCurrentDirection = {};

private:
	HRESULT Ready_Components(PLAYER_DESC* pDesc);
	HRESULT Ready_Fsm();
	HRESULT Ready_Render_Resources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;

};
NS_END

