#include "Box.h"
#include "BindableBase.h"
#include "Sphere.h"
#include "Cone.h"
#include "Cube.h"
#include "Plane.h"
#include "Prism.h"

Box::Box(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	DirectX::XMFLOAT3 material) :
	TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	if (!IsStaticBindsInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		IndexedTriangleList<Vertex> model = Prism::MakeTesselatedIndependentCapNormals<Vertex>(7);
		//model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pVertexBytecode = pVertexShader->GetBytecode();
		AddStaticBind(std::move(pVertexShader));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pVertexBytecode));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	struct PSConstantBuffer
	{
		alignas(16) DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[2];
	} colorConst;
	colorConst.color = material;
	AddBind(std::make_unique<PixelConstantBuffer<PSConstantBuffer>>(gfx, colorConst, 1u));
}
