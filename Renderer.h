#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <vector>
#include "GMath.h"

#include <wrl.h>
using namespace Microsoft::WRL;


class Renderer
{
public:
	Renderer(class Game* game, float r = 0.0f, float g = 0.0f, float b = 0.0f);
	~Renderer();

	bool initialize();

	void begin();
	void end();

	void setBackColor(float r, float g, float b);
	const float* getBackColor() const { return m_backColor; }

private:
	static const UINT FrameNum = 2;
	class Game* m_game;

	ComPtr<IDXGIFactory4> m_dxgiFactory;
	D3D_FEATURE_LEVEL m_featureLevel;
	ComPtr<ID3D12Device>     m_device;
	ComPtr<ID3D12CommandQueue> m_cmdQueue;
	ComPtr<IDXGISwapChain3>    m_swapchain;
	UINT                       m_bufferIndex;
	ComPtr<ID3D12CommandAllocator> m_cmdAllocators[FrameNum];
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	UINT                         m_rtvIncSize;
	ComPtr<ID3D12Resource>       m_backBuffers[FrameNum];
	ComPtr<ID3D12Fence> m_fence;
	UINT64              m_fenceValues[FrameNum];

	float m_backColor[3];
	ComPtr<ID3D12RootSignature> m_simpleRootSig;
	ComPtr<ID3D12PipelineState> m_simplePSO;

	XMFLOAT3                 m_vertices[3];
	ComPtr<ID3D12Resource>   m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;


	bool createFactory();
	bool createDevice(const wchar_t* adapterName);
	bool createCommandQueue();
	bool createSwapchain();
	bool createCommandAllocators();
	bool createCommandList();
	bool createRenderTargetView();
	bool createFence();
	void moveToNextFrame();
	void waitForGPU();
	void setResourceBarrier(D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter);
	void enableDebugLayer();

	bool createResourceBuffer(ID3D12Resource** buffer, UINT64 bSize);
	bool uploadResourceBuffer(ID3D12Resource* buffer, void* src, size_t bSize,
		void** map = nullptr);
	void setVertexBufferView(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
		ID3D12Resource* buffer, UINT bSize, UINT stride);
	bool readShaderObject(const wchar_t* shaderPath, ID3DBlob** shaderObj);
	bool createRootSignature(ID3D12RootSignature** rootSig);
	bool createGPipelineState(ID3D12PipelineState** pso,
		ID3D12RootSignature* rootSig,
		const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath,
		D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum);

};

