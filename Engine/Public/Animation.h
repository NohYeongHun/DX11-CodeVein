#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CAnimation : public CBase
{
private:
	explicit CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	void Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta);

private:
	/* 애니메이션 전체 재생 길이 */
	_float m_fDuration = {};

	/* 초당 이동해야할 재생 거리 */
	_float m_fTickPerSecond = {};

	_float m_fCurrentTrackPosition = {};

	/* 이 동작을 위한 뼈들의 상태 */
	/* CChannel == 뼈들을 위한 상태 행렬 저장*/
	_uint m_iNumChannels = {};
	vector<class CChannel*> m_Channels = {};


public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones);
	virtual void Free();
};

NS_END

