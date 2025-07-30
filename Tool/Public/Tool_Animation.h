#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CTool_Animation : public CBase
{
private:
	explicit CTool_Animation();
	explicit CTool_Animation(const CTool_Animation& Prototype);
	virtual ~CTool_Animation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CTool_Bone*>& Bones);
	void Update_TransformationMatrices(const vector<class CTool_Bone*>& Bones, _bool isLoop, _bool* pFinished, _float fTimeDelta);

#pragma region 저장 로직.
public:
	void Save_Animation(ANIMATION_INFO& animInfo);
#pragma endregion

private:
	_char m_szName[MAX_PATH] = {};
	/* 애니메이션 전체 재생 길이 */
	_float m_fDuration = {};

	/* 초당 이동해야할 재생 거리 */
	_float m_fTickPerSecond = {};
	_float m_fCurrentTrackPosition = {};

	/* 이 동작을 위한 뼈들의 상태 */
	/* CTool_Channel == 뼈들을 위한 상태 행렬 저장*/
	_uint m_iNumChannels = {};
	vector<class CTool_Channel*> m_Channels = {};
	vector<_uint> m_ChannelCurrentKeyFrameIndices = {};


public:
	static CTool_Animation* Create(const aiAnimation* pAIAnimation, const vector<CTool_Bone*>& Bones);
	virtual void Free();
};

NS_END

