#pragma once

NS_BEGIN(Tool)

typedef struct ModelDesc
{
	const _tchar* prototypeName;
	const _char* modelPath;
	const _char* texturePath;
}MODEL_DESC;


/* 사용할 Prototype Name들을 배열에 미리 저장 */
//extern const _tchar* Model_PrototypeNames[MAX_PATH];
extern const MODEL_DESC Model_Prototypes[];
extern unsigned int Model_PrototypeSize;
NS_END