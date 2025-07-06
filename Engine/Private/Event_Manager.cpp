#include "Event_Manager.h"

CEvent_Manager::CEvent_Manager()
{
}


//void CEvent_Manager::Subscribe(EventType id, CBase* pOwner, FCallback&& fn)
//{
//    if (nullptr == pOwner)
//        return;
//
//    Safe_AddRef(pOwner);
//    // 이동 함수를 통해 값 자체를 이관해옵니다.
//    m_Table[id].push_back({ pOwner, std::move(fn) });
//
//}

void CEvent_Manager::Subscribe(EventType id, FCallback&& fn)
{
    // 이동 함수를 통해 값 자체를 이관해옵니다.
    m_Table[id].push_back({ std::move(fn) });
}


//void CEvent_Manager::UnSubscribe(EventType id, CBase* pOwner)
//{
//    auto& vec = m_Table[id];
//
//    vec.erase(std::remove_if(vec.begin(), vec.end(),
//        [&](const FEntry& e) { 
//            if (e.pOwner == pOwner)
//            {
//                Safe_Release(pOwner);
//                return true;
//            }
//            return false;
//        }),
//        vec.end());
//}

void CEvent_Manager::UnSubscribe(EventType id)
{
    auto iter = m_Table.find(id);
    if (iter == m_Table.end())
        return;

    m_Table[id].clear();
    m_Table.erase(id);
}


CEvent_Manager* CEvent_Manager::Create()
{
	return new CEvent_Manager;
}

//void CEvent_Manager::Free()
//{
//    __super::Free();
//    for (auto& pair : m_Table)
//    {
//        for (auto& pValue : pair.second)
//            Safe_Release(pValue.pOwner);
//
//        // pair.second = vector
//        pair.second.clear();
//    }
//}

void CEvent_Manager::Free()
{
    __super::Free();
    for (auto& pair : m_Table)
        pair.second.clear();
    m_Table.clear();

}