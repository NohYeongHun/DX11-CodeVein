#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CLoad_Animation final : public CBase
{
private:
	explicit CLoad_Animation();
	explicit CLoad_Animation(const CLoad_Animation& Prototype);

	virtual ~CLoad_Animation() = default;


public:
	HRESULT Initialize(std::ifstream& ifs);
	void Update_TransformationMatrices(const vector<class CLoad_Bone*>& Bones, _bool isLoop, _bool* pFinished, _bool* pTrackEnd, _float fTimeDelta);
	const _float Get_CurrentTrackPosition() { return m_fCurrentTrackPosition; }
	_matrix Get_BoneMatrixAtTime(_uint iBoneIndex, _float fCurrentTrackPosition);
	void Reset();


private:
	/* 채널 이름 */
	_char m_szName[MAX_PATH] = {};
	/* 애니메이션 전체 재생 길이 */
	_float m_fDuration = {};

	/* 초당 이동해야할 재생 거리 */
	_float m_fTickPerSecond = {};
	_float m_fCurrentTrackPosition = {};

	/* 이 동작을 위한 뼈들의 상태 */
	/* CChannel == 뼈들을 위한 상태 행렬 저장*/
	_uint m_iNumChannels = {};
	vector<class CLoad_Channel*> m_Channels = {};
	// 최근에 재생한 키프레임의 인덱스들.
	vector<_uint> m_CurrentKeyFrameIndices;

private:
	void ApplyRootMotion(_float fTimeDelta);

private:
	HRESULT Load_Channels(std::ifstream& ifs);
	

public:
	static CLoad_Animation* Create(std::ifstream& ifs);
	CLoad_Animation* Clone();
	virtual void Free();


};
NS_END

