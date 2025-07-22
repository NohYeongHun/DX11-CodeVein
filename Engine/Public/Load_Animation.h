#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CLoad_Animation final : public CBase
{
private:
	explicit CLoad_Animation();
	virtual ~CLoad_Animation() = default;


public:
	HRESULT Initialize(std::ifstream& ifs);
	void Update_TransformationMatrices(const vector<class CLoad_Bone*>& Bones, _float fTimeDelta);


private:
	/* 애니메이션 전체 재생 길이 */
	_float m_fDuration = {};

	/* 초당 이동해야할 재생 거리 */
	_float m_fTickPerSecond = {};

	_float m_fCurrentTrackPosition = {};

	/* 이 동작을 위한 뼈들의 상태 */
	/* CChannel == 뼈들을 위한 상태 행렬 저장*/
	_uint m_iNumChannels = {};
	vector<class CLoad_Channel*> m_Channels = {};


private:
	HRESULT Load_Channels(std::ifstream& ifs);

public:
	static CLoad_Animation* Create(std::ifstream& ifs);
	virtual void Free();


};
NS_END

