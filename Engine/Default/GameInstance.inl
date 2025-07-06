#pragma once
#include "Event_Manager.h"   // 여기서 완전 정의 포함

template<typename T>
inline void CGameInstance::Publish(EventType id, T* msg)
{
    m_pEvent_Manager->Publish<T>(id, msg);
}