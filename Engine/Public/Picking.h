#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CPicking final : public CBase
{
private:
	explicit CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd);
	void Update();
	_bool isPicked(const _float3& boxMin, const _float3& boxMax, _uint& face);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	HWND				 m_hWnd = {};
public:
	const _float3& Get_RayOrigin() const { return m_Ray_Origin; }
	const _float3& Get_RayDir() const { return m_Ray_Dir; }

private:
	POINT m_PtMouse{};
	D3D11_VIEWPORT m_ViewPort{};
	_float4x4 m_ProjMatrix{}, m_ViewMatrix{}, m_WorldMatrix{};
	_float4 m_Far_Point {}, m_Near_Point{};
	_float3 m_Ray_Origin {}, m_Ray_Dir{};
	_matrix m_Identity_Matrix {};

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
	virtual void Free() override;
};
NS_END

