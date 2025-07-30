#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CAnimation : public CBase
{
private:
	explicit CAnimation();
	explicit CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	_bool Update_TransformationMatrices(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);

private:
	_char					m_szName[MAX_PATH] = {};
	_float m_fDuration = {};
	_float m_fTickPerSecond = {};
	_float m_fCurrentTrackPosition = {};
	_uint m_iNumChannels = {};

	/* 이 동작을 위한 뼈들의 상태 */
	/* CChannel == 뼈들을 위한 상태 행렬 저장*/
	
	vector<class CChannel*> m_Channels = {};
	vector<_uint>			m_ChannelCurrentKeyFrameIndices;

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free();
};

NS_END

