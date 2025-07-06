#pragma once
#include "Event_Manager.h"   // ���⼭ ���� ���� ����

template<typename T>
inline void CGameInstance::Publish(EventType id, T* msg)
{
    m_pEvent_Manager->Publish<T>(id, msg);
}