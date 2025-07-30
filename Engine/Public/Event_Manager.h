#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CEvent_Manager final : public CBase
{
private:
	explicit CEvent_Manager();
	virtual ~CEvent_Manager() = default;

public:

	struct FEntry
	{
		FCallback   Fn;     // 실행 함수
	};


#pragma region Engine에 제공.
public:
	void Subscribe(EventType id, _uint iID, FCallback&& fn);

	// 브로드캐스트(모든 오브젝트) 버전
	template<typename T>
	void Publish(EventType id, T* pMsg)   // ← iID 인자 생략
	{
		auto evIt = m_Table.find(id);
		if (evIt == m_Table.end()) return;

		vector<FCallback> callList;
		callList.reserve(evIt->second.size());

		for (auto& kv : evIt->second)
			if (kv.second.Fn) callList.emplace_back(kv.second.Fn);

		// 복사본 순회 ― 원본이 지워져도 OK
		for (auto& fn : callList)
			fn(static_cast<void*>(pMsg));
	}

	void UnSubscribe(EventType id, _uint iID);

#pragma endregion

private:
	unordered_map<EventType, unordered_map<_uint, FEntry>> m_Table;

public:
	static CEvent_Manager* Create();
	virtual void Free();
};
NS_END

