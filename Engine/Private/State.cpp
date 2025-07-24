#include "State.h"



CState::CState()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

void CState::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);

}
