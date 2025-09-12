#pragma once

NS_BEGIN(Client)
namespace ParticlePresets
{
    // enum 타입을 받아서, 미리 정의된 DESC를 반환하는 '레시피' 함수
    static CParticleSystem::PARTICLESYSTEM_CLONE_DESC Get(CParticleSystem::PARTICLE_TYPE eType)
    {
        CParticleSystem::PARTICLESYSTEM_CLONE_DESC ParticleSystemDesc = {};

        switch (eType)
        {
        case CParticleSystem::PARTICLE_TYPE_BOSS_EXPLOSION:
			// Pool Tag
			ParticleSystemDesc.strPoolTag = TEXT("PARTICLE_SYSTEM");
			// Transform
			ParticleSystemDesc.fSpeedPerSec = 10.f;
			ParticleSystemDesc.fRotationPerSec = XMConvertToRadians(90.f);

			// Texture
			ParticleSystemDesc.strTexturePrototypeTag[TEXTURE_DIFFUSE] = ClientEffect_TexturePrototypes[TEXTURE_DIFFUSE].prototypeName;

			ParticleSystemDesc.iComponentPrototypeLevel = ENUM_CLASS(LEVEL::STATIC);

			/* VIBuffer 추가*/
			ParticleSystemDesc.strVIBufferPrototypeTag = TEXT("Prototype_Component_VIBuffer_Particle_Instance");

			// 사용 여부 텍스쳐
			ParticleSystemDesc.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true;
			ParticleSystemDesc.useTextureCheckArray[TEXTURE::TEXTURE_MASK] = false;
			ParticleSystemDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT] = false;
			ParticleSystemDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT_ALPHA] = false;
			ParticleSystemDesc.useTextureCheckArray[TEXTURE::TEXTURE_NOISE] = false;

			ParticleSystemDesc.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 0;

			// Shader
			ParticleSystemDesc.iShaderPath = 0; // Default shader path
			ParticleSystemDesc.strShaderPrototypeTag = TEXT("Prototype_Component_Shader_ParticleSystem");

            ParticleSystemDesc.iEmitCount = 200;
            ParticleSystemDesc.vLifeTime = { 1.f, 3.f };
            ParticleSystemDesc.vSpeed = { 10.f, 25.f };
			ParticleSystemDesc.vSizeStart = { 0.1f, 0.2f };
			ParticleSystemDesc.vSizeEnd = { 0.05f, 0.1f };
            ParticleSystemDesc.vColorStart = { 1.f, 0.5f, 0.1f, 1.f }; // 주황색
            ParticleSystemDesc.vColorEnd = { 0.3f, 0.3f, 0.3f, 0.f };   // 어두운 회색
			ParticleSystemDesc.isLoop = false;

			ParticleSystemDesc.eEmissionType = EMITTER_BURST;
			ParticleSystemDesc.eEmitterShape = EMITTER_SPHERE;
			ParticleSystemDesc.iEmissionRate = 100;
			ParticleSystemDesc.iBurstCount = 200; // 한번 생성시 터져나올 개수
			ParticleSystemDesc.vShapeSize = { 2.f, 2.f, 2.f };
			ParticleSystemDesc.fConeAngle = 45.f;
            break;

        case CParticleSystem::PARTICLE_TYPE_QUEEN_WARP:

#pragma region CLONE하는데 필요한 공통. 정보
			// Pool Tag
			ParticleSystemDesc.strPoolTag = TEXT("PARTICLE_SYSTEM");
			// Transform
			ParticleSystemDesc.fSpeedPerSec = 10.f;
			ParticleSystemDesc.fRotationPerSec = XMConvertToRadians(90.f);

			// Texture
			ParticleSystemDesc.strTexturePrototypeTag[TEXTURE_DIFFUSE] = ClientEffect_TexturePrototypes[TEXTURE_DIFFUSE].prototypeName;
			ParticleSystemDesc.strTexturePrototypeTag[TEXTURE_MASK] = ClientEffect_TexturePrototypes[TEXTURE_MASK].prototypeName;

			ParticleSystemDesc.iComponentPrototypeLevel = ENUM_CLASS(LEVEL::STATIC);

			/* VIBuffer 추가*/
			ParticleSystemDesc.strVIBufferPrototypeTag = TEXT("Prototype_Component_VIBuffer_Particle_Instance");

			// 사용 여부 텍스쳐
			ParticleSystemDesc.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true;
			ParticleSystemDesc.useTextureCheckArray[TEXTURE::TEXTURE_MASK] = true;
			ParticleSystemDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT] = false;
			ParticleSystemDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT_ALPHA] = false;
			ParticleSystemDesc.useTextureCheckArray[TEXTURE::TEXTURE_NOISE] = false;

			ParticleSystemDesc.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 1;
			ParticleSystemDesc.useTextureIndexArray[TEXTURE::TEXTURE_MASK] = 0;

			// Shader
			ParticleSystemDesc.iShaderPath = ENUM_CLASS(POINTDIRPARTICLE_SHADERPATH::QUEENKNIGHTWARP);
			ParticleSystemDesc.strShaderPrototypeTag = TEXT("Prototype_Component_Shader_ParticleSystem");
#pragma endregion
			ParticleSystemDesc.vLifeTime = { 1.f, 6.f };
			ParticleSystemDesc.vSpeed = { 3.f , 6.f };
			ParticleSystemDesc.vSizeStart = { 0.05f, 0.1f };
			ParticleSystemDesc.vSizeEnd = { 0.12f, 0.15f };
			ParticleSystemDesc.vColorStart = { 1.f, 0.f, 1.f, 1.f };
			ParticleSystemDesc.vColorEnd = { 1.f, 1.f, 0.f, 1.f };
			ParticleSystemDesc.iEmitCount = 300;
			ParticleSystemDesc.isLoop = false;


			ParticleSystemDesc.eEmissionType = EMITTER_BURST;
			ParticleSystemDesc.eEmitterShape = EMITTER_SPHERE;
			ParticleSystemDesc.iEmissionRate = 100;
			ParticleSystemDesc.iBurstCount = 50; // 한번 생성시 터져나올 개수
			ParticleSystemDesc.vShapeSize = { 1.f, 1.f ,1.f };
			ParticleSystemDesc.fConeAngle = 45.f;
        }
        return ParticleSystemDesc;
    }
}
NS_END


