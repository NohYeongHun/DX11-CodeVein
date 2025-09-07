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
            ParticleSystemDesc.iEmitCount = 200;
            ParticleSystemDesc.vLifeTime = { 1.f, 3.f };
            ParticleSystemDesc.vSpeed = { 10.f, 25.f };
            ParticleSystemDesc.vColorStart = { 1.f, 0.5f, 0.1f, 1.f }; // 주황색
            ParticleSystemDesc.vColorEnd = { 0.3f, 0.3f, 0.3f, 0.f };   // 어두운 회색
            //tDesc.strPrototypeTag = L"Prototype_Component_Texture_BossExplosion";
            break;

        case CParticleSystem::PARTICLE_TYPE_QUEEN_WARP:
			// Pool Tag
			ParticleSystemDesc.strPoolTag = TEXT("PARTICLE_SYSTEM");
			// Transform
			ParticleSystemDesc.fSpeedPerSec = 10.f;
			ParticleSystemDesc.fRotationPerSec = XMConvertToRadians(90.f);

			// Texture
			ParticleSystemDesc.strTexturePrototypeTag[TEXTURE_DIFFUSE] = Effect_TexturePrototypes[TEXTURE_DIFFUSE].prototypeName;
			ParticleSystemDesc.strTexturePrototypeTag[TEXTURE_MASK] = Effect_TexturePrototypes[TEXTURE_MASK].prototypeName;
			ParticleSystemDesc.iComponentPrototypeLevel = ENUM_CLASS(LEVEL::STATIC);

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

			


			ParticleSystemDesc.vLifeTime = { 1.f, 6.f };
			ParticleSystemDesc.vSpeed = { 3.f , 6.f };
			ParticleSystemDesc.vSizeStart = { 0.05f, 0.1f };
			ParticleSystemDesc.vSizeEnd = { 0.12f, 0.15f };
			ParticleSystemDesc.vColorStart = { 1.f, 0.f, 1.f, 1.f };
			ParticleSystemDesc.vColorEnd = { 1.f, 1.f, 0.f, 1.f };
			ParticleSystemDesc.iEmitCount = 300;
			ParticleSystemDesc.isLoop = false;

			ParticleSystemDesc.eEmissionType = EMITTER_CONTINUOUS;
			ParticleSystemDesc.eEmitterShape = EMITTER_SPHERE;
			ParticleSystemDesc.iEmissionRate = 100;
			ParticleSystemDesc.iBurstCount = 50; // 한번 생성시 터져나올 개수
			ParticleSystemDesc.vShapeSize = { 1.f, 1.f ,1.f };
			ParticleSystemDesc.fConeAngle = 45.f;
            break;
        }
        return ParticleSystemDesc;
    }
}
NS_END


