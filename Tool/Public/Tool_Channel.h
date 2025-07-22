#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CTool_Channel final : public CBase
{
private:
	explicit CTool_Channel();
	virtual ~CTool_Channel() = default;


public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CTool_Bone*>& Bones);
	void Update_TransformationMatrix(const vector<class CTool_Bone*>& Bones, _float fCurrentTrackPosition);

#pragma region 저장 로직.
public:
	void Save_Channel(CHANNEL_INFO& channelInfo);
#pragma endregion


private:
	_char m_szName[MAX_PATH] = {};
	_uint m_iBoneIndex = {};

	/* 시간에 따른 뼈의 상태를 보관합니다. */
	_uint m_iNumKeyFrames = {};
	vector<KEYFRAME> m_KeyFrames = {};

public:
	static CTool_Channel* Create(const aiNodeAnim* pAIChannel, const vector<class CTool_Bone*>& Bones);
	virtual void Free();
	
};
NS_END

