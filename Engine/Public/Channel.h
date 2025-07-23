#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CChannel final : public CBase
{
private:
	explicit CChannel();
	virtual ~CChannel() = default;


public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_float fCurrentTrackPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex);

private:
	_char m_szName[MAX_PATH] = {};
	_uint m_iBoneIndex = {};

	/* 시간에 따른 뼈의 상태를 보관합니다. */
	_uint m_iNumKeyFrames = {};
	vector<KEYFRAME> m_KeyFrames = {};

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free();
	
};
NS_END

