#include "Event_Manager.h"

CEvent_Manager::CEvent_Manager()
{
}

void CEvent_Manager::Subscribe(EventType id, _uint iID, FCallback&& fn)
{
    auto& objectMap = m_Table[id];      // 1단: EventType 키가 없으면 알아서 생성
    objectMap[iID] = { std::move(fn) };
}


void CEvent_Manager::UnSubscribe(EventType id, _uint iID)
{
    auto evIt = m_Table.find(id);
    if (evIt == m_Table.end()) return;

    evIt->second.erase(iID);       // 오브젝트 날림

    if (evIt->second.empty())           // 이벤트에 남은 게 없으면 이벤트도 정리
        m_Table.erase(evIt);
}

CEvent_Manager* CEvent_Manager::Create()
{
	return new CEvent_Manager;
}

void CEvent_Manager::Free()
{
    __super::Free();
    for (auto& pair : m_Table)
        pair.second.clear();
    m_Table.clear();

}