#include "Renderer.h"
#include "Game.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

Renderer::Renderer(Game* game, XMFLOAT3 backColor)
	: m_game(game)
	, m_featureLevel()
	, m_bufferIndex(0)
	, m_rtvIncSize(0)
	, m_csuIncSize(0)
	, m_backColor(backColor)
	, m_fenceValues{ 0 }
	, m_vertexBufferView{}
	, m_indexBufferView{}
{
	m_vertices[0] = { { -0.525f, -0.7f, 0.0f }, {0.0f, 1.0f } };
	m_vertices[1] = { { -0.525f,  0.7f, 0.0f }, {0.0f, 0.0f } };
	m_vertices[2] = { {  0.525f, -0.7f, 0.0f }, {1.0f, 1.0f } };
	m_vertices[3] = { {  0.525f,  0.7f, 0.0f }, {1.0f, 0.0f } };
	//m_vertices[4] = {  1.0f, -1.0f, 0.0f };
	//m_vertices[5] = { -1.0f,  1.0f, 0.0f };

	m_indices[0] = 0;  m_indices[1] = 1;    m_indices[2] = 2;
	m_indices[3] = 3;  m_indices[4] = 2;    m_indices[5] = 1;
}

Renderer::~Renderer()
{
	waitForGPU();
}

bool Renderer::initialize()
{
#ifdef _DEBUG
	enableDebugLayer();
#endif  

	if (!createFactory())return false;
	if (!createDevice(L"NVIDIA"))   return false;
	if (!createCommandQueue())      return false;
	if (!createSwapchain())         return false;
	if (!createCommandAllocators()) return false;
	if (!createCommandList())       return false;
	if (!createRenderTargetView())  return false;
	if (!createFence())             return false;
	setViewport(m_viewport);
	setScissorRect(m_scissorRect);

	m_cmdList->Close();

	//入力レイアウト、ルートシグネチャ、PSO
	D3D12_INPUT_ELEMENT_DESC inputLayouts[2];
	inputLayouts[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayouts[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	if (!createRootSignature(m_simpleRootSig.GetAddressOf())) return false;
	if (!createGPipelineState(m_simplePSO.GetAddressOf(), m_simpleRootSig.Get(),
		L"shader\\simpleVS.cso", L"shader\\simplePS.cso",
		inputLayouts, 2)) return false;


	//頂点バッファ生成
	if (!createResourceBuffer(m_vertexBuffer.GetAddressOf(),
		4 * sizeof(VertexUV))) return false;
	if (!uploadResourceBuffer(m_vertexBuffer.Get(),
		(void*)m_vertices, 4 * sizeof(VertexUV))) return false;
	setVertexBufferView(m_vertexBufferView, m_vertexBuffer.Get(),
		4 * sizeof(VertexUV), sizeof(VertexUV));

	//インデックスバッファの生成
	if (!createResourceBuffer(m_indexBuffer.GetAddressOf(),
		6 * sizeof(unsigned short))) return false;
	if (!uploadResourceBuffer(m_indexBuffer.Get(),
		(void*)m_indices, 6 * sizeof(unsigned short))) return false;
	setIndexBufferView(m_indexBufferView, m_indexBuffer.Get(),
		6 * sizeof(unsigned short));

	//シェーダーリソース用ディスクリプタヒープの生成
	if (!createDescHeap(m_scDescHeap.GetAddressOf(), 1)) return false;

	//実験用のランダムノイズテクスチャの生成とビューのセット
	std::vector<ColorRGBA> texture(512 * 512);
	createRandomNoiseImage(texture, m_game);
	if (!createShaderResource(m_textureBuffer.GetAddressOf()
		512, 512, DXGI_FORMAT_R8G8B8A8_UNORM)) return false;
	if (!uploadShaderResource(m_textureBuffer.Get(),
		texture.data(), (UINT)sizeof(ColorRGBA) * 512,
		(UINT)(sizeof(ColorRGBA) * texture.size()))) return false;
	setShaderResourceView(m_textureBuffer.Get(), DXGI_FORMAT_R8G8B8A8_UNORM,
		m_scDescHeap.Get(), 0);
	return true;
}

void Renderer::begin()
{
	m_cmdAllocators[m_bufferIndex]->Reset();
	m_cmdList->Reset(m_cmdAllocators[m_bufferIndex].Get(), nullptr);

	setResourceBarrier(D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_bufferIndex * m_rtvIncSize;
	m_cmdList->OMSetRenderTargets(1, &rtvHandle, true, nullptr);

	float clearColor[4] = { m_backColor.x, m_backColor.y, m_backColor.z, 1.0f };
	m_cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	m_cmdList->RSSetViewports(1, &m_viewport);
	m_cmdList->RSSetScissorRects(1, &m_scissorRect);
}

void Renderer::end()
{
	setResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	m_cmdList->Close();
	ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);
	m_swapchain->Present(1, 0);
	m_bufferIndex = m_swapchain->GetCurrentBackBufferIndex();
	moveToNextFrame();
}

bool Renderer::createFactory()
{
	UINT debugFlag = 0;
#ifdef _DEBUG
	debugFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

	HRESULT hr = CreateDXGIFactory2(debugFlag, IID_PPV_ARGS(m_dxgiFactory.GetAddressOf()));

	return SUCCEEDED(hr);
}



bool Renderer::createDevice(const wchar_t* adapterName)
{
	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* sa = nullptr;
	int i = 0;
	while (m_dxgiFactory->EnumAdapters(i, &sa) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(sa);
		++i;
	}
	sa = nullptr;
	for (auto adp : adapters)
	{
		DXGI_ADAPTER_DESC adpDesc = {};
		adp->GetDesc(&adpDesc);
		std::wstring str = adpDesc.Description;
		if (str.find(adapterName) != std::wstring::npos)
		{
			sa = adp;
			break;
		}
	}
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	for (auto level : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(sa, level, IID_PPV_ARGS(m_device.GetAddressOf()))))
		{
			m_featureLevel = level;
			break;
		}
	}
	for (auto adp : adapters)
	{
		adp->Release();
	}
	

	if (m_device.Get() == nullptr) return false;

	m_rtvIncSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_csuIncSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return true;
}

void Renderer::enableDebugLayer()
{
	ComPtr<ID3D12Debug> debugLayer;
	D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.GetAddressOf()));
	debugLayer->EnableDebugLayer();
}

bool Renderer::createCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(m_cmdQueue.GetAddressOf()));
	return SUCCEEDED(hr);
}

bool Renderer::createSwapchain()
{
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width              = m_game->getWidth();
	scDesc.Height             = m_game->getHeight();
	scDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Stereo             = FALSE;
	scDesc.BufferUsage        = DXGI_USAGE_BACK_BUFFER;
	scDesc.BufferCount        = FrameNum;
	scDesc.SampleDesc.Count   = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Scaling            = DXGI_SCALING_NONE;
	scDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
	fsDesc.Windowed = TRUE;
	HWND hwnd = m_game->getHwnd();
	ComPtr<IDXGISwapChain1> swapchain;
	HRESULT hr = m_dxgiFactory->CreateSwapChainForHwnd(
		m_cmdQueue.Get(), hwnd, &scDesc, &fsDesc,
		nullptr, swapchain.GetAddressOf());
	if (FAILED(hr)) return false;

	hr = swapchain.As(&m_swapchain);
	if (FAILED(hr)) return false;
	m_bufferIndex = m_swapchain->GetCurrentBackBufferIndex();
	hr = m_dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

	return SUCCEEDED(hr);
}

bool Renderer::createCommandAllocators()
{
	for (UINT i = 0; i < FrameNum; ++i)
	{
		HRESULT hr = m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_cmdAllocators[i].GetAddressOf()));
		if (FAILED(hr)) return false;
	}
	return true;
}

bool Renderer::createCommandList()
{
	HRESULT hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_cmdAllocators[m_bufferIndex].Get(), nullptr,
		IID_PPV_ARGS(m_cmdList.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Renderer::createRenderTargetView()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = FrameNum;
	desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask       = 0;

	HRESULT hr = m_device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf()));
	if (FAILED(hr)) return false;

	auto handle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < FrameNum; ++i)
	{
		hr = m_swapchain->GetBuffer(i, IID_PPV_ARGS(m_backBuffers[i].GetAddressOf()));
		if (FAILED(hr)) return false;

		D3D12_RENDER_TARGET_VIEW_DESC rDesc = {};
		rDesc.Format               = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rDesc.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
		rDesc.Texture2D.MipSlice   = 0;
		rDesc.Texture2D.PlaneSlice = 0;

		m_device->CreateRenderTargetView(m_backBuffers[i].Get(), &rDesc, handle);
		handle.ptr += m_rtvIncSize;
	}
	return true;
 }

void Renderer::setBackColor(float r, float g, float b)
{
	//m_backColor[0] = r;
	//m_backColor[1] = g;
	//m_backColor[2] = b;
	m_backColor = XMFLOAT3(r, g, b);
}

bool Renderer::createFence()
{
	for (UINT i = 0; i < FrameNum; ++i)
	{
		m_fenceValues[i] = 0;
	}

	HRESULT hr = m_device->CreateFence(
		m_fenceValues[m_bufferIndex],
		D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf()));
	if (FAILED(hr)) return false;

	m_fenceValues[m_bufferIndex]++;
	return true;
}

void Renderer::moveToNextFrame()
{
	auto currentValue = m_fenceValues[m_bufferIndex];
	m_cmdQueue->Signal(m_fence.Get(), currentValue);
	m_bufferIndex = m_swapchain->GetCurrentBackBufferIndex();
	if (m_fence->GetCompletedValue() < m_fenceValues[m_bufferIndex])
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		if (event)
		{
			m_fence->SetEventOnCompletion(m_fenceValues[m_bufferIndex], event);
			WaitForSingleObjectEx(event, INFINITE, FALSE);
			CloseHandle(event);
		}
	}
	m_fenceValues[m_bufferIndex] = currentValue + 1;
}

void Renderer::waitForGPU()
{
	m_cmdQueue->Signal(m_fence.Get(), m_fenceValues[m_bufferIndex]);
	HANDLE event = CreateEvent(nullptr, false, false, nullptr);
	if (event)
	{
		m_fence->SetEventOnCompletion(m_fenceValues[m_bufferIndex], event);
		WaitForSingleObjectEx(event, INFINITE, FALSE);
		CloseHandle(event);
	}
	m_fenceValues[m_bufferIndex]++;
	m_bufferIndex = m_swapchain->GetCurrentBackBufferIndex();
}

void Renderer::setResourceBarrier(D3D12_RESOURCE_STATES stateBefore,
	D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER desc = {};
	desc.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.Transition.pResource   = m_backBuffers[m_bufferIndex].Get();
	desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	desc.Transition.StateBefore = stateBefore;
	desc.Transition.StateAfter  = stateAfter;

	m_cmdList->ResourceBarrier(1, &desc);
}

bool Renderer::createResourceBuffer(ID3D12Resource** buffer, UINT64 bSize)
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width              = bSize;
	desc.Height             = 1;
	desc.DepthOrArraySize   = 1;
	desc.MipLevels          = 1;
	desc.Format             = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags              = D3D12_RESOURCE_FLAG_NONE;
	desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = m_device->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(buffer));

	return SUCCEEDED(hr);
}

bool Renderer::uploadResourceBuffer(ID3D12Resource* buffer, void* src, size_t bSize,
	void** map)
{
	if (map != nullptr)
	{
		*map = nullptr;
		HRESULT hr = buffer->Map(0, nullptr, map);
		if (FAILED(hr) || *map == nullptr) return false;
		memcpy(*map, src, bSize);
	}
	else
	{
		void* pmap = nullptr;
		HRESULT hr = buffer->Map(0, nullptr, &pmap);
		if (FAILED(hr) || pmap == nullptr) return false;
		memcpy(pmap, src, bSize);
		buffer->Unmap(0, nullptr);
	}
	return true;
}

void Renderer::setVertexBufferView(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
	ID3D12Resource* buffer, UINT bSize, UINT stride)
{
	vertexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes    = bSize;
	vertexBufferView.StrideInBytes  = stride;
}

bool Renderer::readShaderObject(const wchar_t* shaderPath, ID3DBlob** shaderObj)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath, shaderObj);

	return SUCCEEDED(hr);
}

bool Renderer::createRootSignature(ID3D12RootSignature** rootSig)
{
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> blob;
	HRESULT hr = D3D12SerializeRootSignature(
		&desc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		blob.GetAddressOf(), nullptr);
	if (FAILED(hr)) return false;

	hr = m_device->CreateRootSignature(
		0, blob->GetBufferPointer(), blob->GetBufferSize(),
		IID_PPV_ARGS(rootSig));

	return SUCCEEDED(hr);
}

bool Renderer::createGPipelineState(ID3D12PipelineState** pso,
	ID3D12RootSignature* rootSig,
	const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath,
	D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pDesc = {};

	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	if (!readShaderObject(vertexShaderPath, vsBlob.GetAddressOf())) return false;
	if (!readShaderObject(pixelShaderPath, psBlob.GetAddressOf())) return false;

	pDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pDesc.PS.BytecodeLength = psBlob->GetBufferSize();
	pDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	D3D12_RENDER_TARGET_BLEND_DESC rtDesc = {};
	rtDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	rtDesc.LogicOpEnable = FALSE;
	rtDesc.BlendEnable = FALSE;

	pDesc.BlendState.AlphaToCoverageEnable = FALSE;
	pDesc.BlendState.IndependentBlendEnable = FALSE;
	pDesc.BlendState.RenderTarget[0] = rtDesc;

	pDesc.RasterizerState.MultisampleEnable = FALSE;
	pDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pDesc.RasterizerState.DepthClipEnable = TRUE;
	pDesc.RasterizerState.FrontCounterClockwise = FALSE;
	pDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	pDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	pDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	pDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	pDesc.RasterizerState.ForcedSampleCount = 0;
	pDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	pDesc.DepthStencilState.DepthEnable = FALSE;
	pDesc.DepthStencilState.StencilEnable = FALSE;

	pDesc.InputLayout.pInputElementDescs = inputLayouts;
	pDesc.InputLayout.NumElements = layoutNum;

	pDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	pDesc.NumRenderTargets = 1;
	pDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	pDesc.SampleDesc.Count = 1;
	pDesc.SampleDesc.Quality = 0;

	pDesc.pRootSignature = rootSig;

	HRESULT hr = m_device->CreateGraphicsPipelineState(&pDesc,
		IID_PPV_ARGS(pso));

	return SUCCEEDED(hr);
}

void Renderer::setViewport(D3D12_VIEWPORT& viewport)
{
	viewport.Width = (float)m_game->getWidth();
	viewport.Height = (float)m_game->getHeight();
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void Renderer::setScissorRect(D3D12_RECT& scissor)
{
	scissor.left = 0;
	scissor.top = 0;
	scissor.right = (LONG)m_game->getWidth();
	scissor.bottom = (LONG)m_game->getHeight();
}

void Renderer::draw()
{
	m_cmdList->SetPipelineState(m_simplePSO.Get());
	m_cmdList->SetGraphicsRootSignature(m_simpleRootSig.Get());

	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_cmdList->IASetIndexBuffer(&m_indexBufferView);

	m_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Renderer::setIndexBufferView(D3D12_INDEX_BUFFER_VIEW& indexBufferView,
	ID3D12Resource* buffer, UINT bSize)
{
	indexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = bSize;
	indexBufferView.Format = DXGI_FORMAT_R16_UINT;
}

bool Renderer::createShaderResource(ID3D12Resource** buffer, UINT width, UINT height,
	DXGI_FORMAT format, UINT16 mipLevels, UINT16 depthOrArraySize,
	D3D12_RESOURCE_DIMENSION dimension)
{
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type                 = D3D12_HEAP_TYPE_CUSTOM;
	prop.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension          = dimension;
	desc.Width              = width;
	desc.Height             = height;
	desc.DepthOrArraySize   = depthOrArraySize;
	desc.MipLevels          = mipLevels;
	desc.Format             = format;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags              = D3D12_RESOURCE_FLAG_NONE;
	desc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	HRESULT hr = m_device->CreateCommittedResource(
		&prop, D3D12_HEAP_FLAG_NONE,
		&desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr, IID_PPV_ARGS(buffer));

	return SUCCEEDED(hr);
}

bool Renderer::uploadShaderResource(ID3D12Resource* buffer, void* src,
	UINT lineSize, UINT allSize)
{
	HRESULT hr = buffer->WriteToSubresource(0, nullptr,
		src, lineSize, allSize);

	return SUCCEEDED(hr);
}

bool Renderer::createDescHeap(ID3D12DescriptorHeap** dHeap, UINT dNum)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask       = 0;
	desc.NumDescriptors = dNum;
	desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT hr = m_device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(dHeap));

	return SUCCEEDED(hr);
}

void Renderer::setShaderResourceView(ID3D12Resource* buffer, DXGI_FORMAT format,
	ID3D12DescriptorHeap* dHeap, UINT index)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format                  = format;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels     = 1;

	auto handle = dHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_csuIncSize * index;
	m_device->CreateShaderResourceView(buffer, &desc, handle);
}

//k6_25