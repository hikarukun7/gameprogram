#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <vector>
#include "GMath.h"
#include "GameUtil.h"

#include <wrl.h>
using namespace Microsoft::WRL;


class Renderer
{
public:
	Renderer(class Game* game, XMFLOAT3 backColor = ZeroVec3d);
	~Renderer();

	bool initialize();

	void begin();
	void end();

	void setBackColor(float r, float g, float b);
	void setBackColor(XMFLOAT3 backColor) { m_backColor = backColor; }
	XMFLOAT3 getBackColor() const { return m_backColor; }

	//ŽÀŒ±—p
	void draw();

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

	//float m_backColor[3];
	XMFLOAT3 m_backColor;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	ComPtr<ID3D12RootSignature> m_simpleRootSig;
	ComPtr<ID3D12PipelineState> m_simplePSO;

	VertexUV                 m_vertices[4];
	ComPtr<ID3D12Resource>   m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	unsigned short          m_indices[6];
	ComPtr<ID3D12Resource>  m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;


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
	void setViewport(D3D12_VIEWPORT& viewport);
	void setScissorRect(D3D12_RECT& scissor);

	bool createResourceBuffer(ID3D12Resource** buffer, UINT64 bSize);
	bool uploadResourceBuffer(ID3D12Resource* buffer, void* src, size_t bSize,
		void** map = nullptr);
	bool createShaderResource(ID3D12Resource** buffer, UINT width, UINT height,
		DXGI_FORMAT format, UINT16 mipLevels = 1, UINT16 depthOrArraySize = 1,
		D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D);
	bool uploadShaderResource(ID3D12Resource* buffer, void* src,
		UINT lineSize, UINT allSize);
	void setVertexBufferView(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
		ID3D12Resource* buffer, UINT bSize, UINT stride);
	void setIndexBufferView(D3D12_INDEX_BUFFER_VIEW& indexBufferView,
		ID3D12Resource* buffer, UINT bSize);
	bool readShaderObject(const wchar_t* shaderPath, ID3DBlob** shaderObj);
	bool createRootSignature(ID3D12RootSignature** rootSig);
	bool createGPipelineState(ID3D12PipelineState** pso,
		ID3D12RootSignature* rootSig,
		const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath,
		D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum);

};
