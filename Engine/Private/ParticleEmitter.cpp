#pragma region 0. 기본 함수들
CParticleEmitter::CParticleEmitter()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CParticleEmitter::Initialize(const EMITTER_DESC* pDesc)
{
	// 1. Emitter의 고유 설정을 멤버 변수에 복사
	m_tDesc = *pDesc;

	// 2. 연속 생성(Continuous) 타입일 경우, 초당 생성률을 시간 간격으로 미리 계산
	if (m_tDesc.eEmissionType == EMITTER_CONTINUOUS && m_tDesc.iEmissionRate > 0)
	{
		// 매번 나누기 연산을 하는 것을 피하기 위해 미리 계산해 둡니다.
		m_fEmitInterval = 1.f / m_tDesc.iEmissionRate;
	}

	return S_OK;
}



#pragma endregion

#pragma region 1. 고유 함수들
void CParticleEmitter::Reset(const CParticleSystem::PARTICLESYSTEM_CLONE_DESC* pOwnerDesc, CTransform* pOwnerTransform)
{
	// 소유주(CParticleSystem)의 정보와 연결합니다.
	m_pOwnerDesc = pOwnerDesc;
	m_pOwnerTransform = pOwnerTransform;

	// Emitter의 내부 상태를 초기화합니다.
	m_fEmitTimer = 0.f;

	// BURST 타입일 경우, 아직 터지지 않았다는 상태로 리셋
	if (m_tDesc.eEmissionType == EMITTER_BURST)
	{
		// (이를 위해 멤버 변수에 _bool m_bHasBursted = { false }; 추가 필요)
		m_bHasBursted = false;
	}
	
}
void CParticleEmitter::Emit(_float fTimeDelta, vector<PARTICLE_DATA>& out_Particles)
{
	// 유효한 소유주 정보가 없거나 Stop()이 호출된 상태면 아무것도 하지 않음
	if (nullptr == m_pOwnerDesc || nullptr == m_pOwnerTransform || m_fEmitTimer < 0.f)
		return;

	switch (m_tDesc.eEmissionType)
	{
	case EMITTER_CONTINUOUS:
	{
		m_fEmitTimer += fTimeDelta;
		// 타이머가 생성 간격을 넘어섰다면 파티클 생성
		// if 대신 while을 쓰는 이유: 프레임이 심하게 끊겨도 그 시간만큼의 파티클을 모두 생성해주기 위함
		while (m_fEmitTimer >= m_fEmitInterval)
		{
			Create_One_Particle(out_Particles);
			m_fEmitTimer -= m_fEmitInterval; // 타이머에서 생성 간격만큼 차감
		}
	}
	break;

	case EMITTER_BURST:
	{
		 //아직 BURST가 일어나지 않았다면 한번에 모두 생성
		 //(멤버 변수에 _bool m_bHasBursted 추가 필요)
		 if (!m_bHasBursted)
		 {
		     for (_uint i = 0; i < m_tDesc.iBurstCount; ++i)
		     {
		         Create_One_Particle(out_Particles);
		     }
		     m_bHasBursted = true; // 터졌다고 표시
		 }
	}
	break;
	}
}
void CParticleEmitter::Stop()
{
	// Emitter를 비활성화하는 로직. 
   // 간단하게는 타이머를 매우 큰 값으로 설정하여 Emit이 동작하지 않게 할 수 있습니다.
	m_fEmitTimer = -1.f; // 예시: 음수 값으로 만들어 Emit 조건이 절대 참이 되지 않도록 함
}
#pragma endregion


#pragma region 0. 기본 함수들


#pragma region 헬퍼 함수들
void CParticleEmitter::Create_One_Particle(vector<PARTICLE_DATA>& out_Particles)
{
	PARTICLE_DATA newParticle = {};

	// 1. Emitter 모양에 따라 초기 위치와 방향 결정
	Generate_Particle_Attribute(newParticle.vPosition, newParticle.vDir);

	// 2. 소유주(CParticleSystem)의 DESC를 참조하여 속성 무작위 설정
	newParticle.vLife.y = m_pGameInstance->Rand(m_pOwnerDesc->vLifeTime.x, m_pOwnerDesc->vLifeTime.y);
	newParticle.fSpeed = m_pGameInstance->Rand(m_pOwnerDesc->vSpeed.x, m_pOwnerDesc->vSpeed.y);
	newParticle.fStartSize = m_pGameInstance->Rand(m_pOwnerDesc->vSizeStart.x, m_pOwnerDesc->vSizeStart.y);
	newParticle.fEndSize = m_pGameInstance->Rand(m_pOwnerDesc->vSizeEnd.x, m_pOwnerDesc->vSizeEnd.y);

	// 3. 초기값 설정
	newParticle.vLife.x = 0.f; // 현재 수명은 0부터 시작
	newParticle.fCurrentSize = newParticle.fStartSize; // 현재 크기는 시작 크기부터

	// 4. 완성된 파티클을 결과 벡터에 추가
	out_Particles.push_back(newParticle);
}

_float3 CParticleEmitter::Create_Random_Direction()
{
	_float3 vDir;
	// DirectX::XMVector3Normalize 등 사용
	// 예시: 구면 좌표계를 이용한 무작위 벡터 생성
	float z = m_pGameInstance->Rand(-1.f, 1.f);
	float t = m_pGameInstance->Rand(0.f, XM_2PI);
	float r = sqrt(1.f - z * z);

	vDir.x = r * cos(t);
	vDir.y = r * sin(t);
	vDir.z = z;

	return vDir;
}
void CParticleEmitter::Generate_Particle_Attribute(_float3& out_vPosition, _float3& out_vDirection)
{
	// Emitter의 월드 위치를 가져옴
	_float3 vEmitterPosition = {};
	XMStoreFloat3(&vEmitterPosition, m_pOwnerTransform->Get_State(STATE::POSITION));

	switch (m_tDesc.eEmitterShape)
	{
	case EMITTER_POINT:
		// 위치는 항상 Emitter의 위치
		out_vPosition = vEmitterPosition;
		// 방향은 360도 무작위
		out_vDirection = Create_Random_Direction();
		break;

	case EMITTER_SPHERE:
	{
		// 구 표면의 한 점을 무작위로 선택
		_float3 vRandomDir = Create_Random_Direction();
		_float fRadius = m_tDesc.vShapeSize.x; // x를 반지름으로 사용
		XMStoreFloat3(&out_vPosition, 
			(XMLoadFloat3(&vEmitterPosition) + XMLoadFloat3(&vRandomDir) * fRadius));
		// 방향은 구 중심에서 바깥쪽으로
		out_vDirection = vRandomDir;
	}
	break;

	case EMITTER_BOX:
	{
		// 박스 범위 내에서 무작위 위치 선택
		_float fHalfX = m_tDesc.vShapeSize.x * 0.5f;
		_float fHalfY = m_tDesc.vShapeSize.y * 0.5f;
		_float fHalfZ = m_tDesc.vShapeSize.z * 0.5f;

		out_vPosition.x = vEmitterPosition.x + m_pGameInstance->Rand(-fHalfX, fHalfX);
		out_vPosition.y = vEmitterPosition.y + m_pGameInstance->Rand(-fHalfY, fHalfY);
		out_vPosition.z = vEmitterPosition.z + m_pGameInstance->Rand(-fHalfZ, fHalfZ);

		// 방향은 Emitter의 위쪽 방향 (예시)
		XMStoreFloat3(&out_vDirection, m_pOwnerTransform->Get_State(STATE::LOOK));
	}
	break;

	// SHAPE_CONE은 삼각함수가 필요하여 조금 더 복잡합니다.
	case EMITTER_CONE:
		// ...
		break;
	}
}
#pragma endregion



CParticleEmitter* CParticleEmitter::Create(const EMITTER_DESC* pDesc)
{
	CParticleEmitter* pInstance = new CParticleEmitter();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CParticleEmitter"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CParticleEmitter::Free()
{
	CBase::Free();
	Safe_Release(m_pGameInstance);
	m_pOwnerTransform = nullptr;
	m_pOwnerDesc = nullptr;
}

#pragma endregion

