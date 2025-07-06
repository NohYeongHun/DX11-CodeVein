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
		CBase* pOwner;		// 누가 등록했는지
		void* pData;		// payload
		FCallback   Fn;     // 실행 함수
	};


#pragma region Engine에 제공.
public:
	void Subscribe(EventType id, CBase* pOwner, void* data, FCallback&& fn);

	template<typename T>
	void Publish(EventType id, T* pMsg)
	{
		auto it = m_Table.find(id);
		if (it == m_Table.end()) return;

		// 콜백 순회
		for (auto& entry : it->second)
			if (entry.Fn) entry.Fn(static_cast<void*>(pMsg));
	}
	void UnSubscribe(EventType id, CBase* pOwner);
#pragma endregion

private:
	std::unordered_map<EventType, std::vector<FEntry>> m_Table;

public:
	static CEvent_Manager* Create();
	virtual void Free();
};
NS_END

