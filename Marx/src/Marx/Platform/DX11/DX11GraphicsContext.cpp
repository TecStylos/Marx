#include "mxpch.h"
#include "DX11GraphicsContext.h"

#include "Marx/Exceptions/ExceptionMacros.h"
#include "Marx/Platform/DX11/DX11InfoException.h"
#include "Marx/Platform/DX11/DX11ExceptionMacros.h"
#include "Marx/Platform/DX11/DX11InfoManager.h"

namespace Marx
{
	DX11GraphicsContext::DX11GraphicsContext(HWND wndHandle)
		: m_wndHandle(wndHandle)
	{
		RECT rect;
		GetClientRect(wndHandle, &rect);
		m_width = rect.right - rect.left;
		m_height = rect.bottom - rect.top;
	}

	DX11GraphicsContext::~DX11GraphicsContext()
	{}

	void DX11GraphicsContext::init()
	{
		MX_CORE_ASSERT(!m_initialized, "DX11GraphicsContext is already initialized!");
		DX11Manager::init();

		createSwapChain();
		createRenderTargetView();
		createDepthStencil();
		createDepthStencilView();
		setViewport(0, 0, m_width, m_height);
		setRenderTarget();

		m_initialized = true;
	}

	void DX11GraphicsContext::shutdown()
	{
		MX_CORE_ASSERT(m_initialized, "DX11GraphicsContext is not initialized!");
		DX11Manager::shutdown();
		m_initialized = false;
	}

	void DX11GraphicsContext::clear(float r, float g, float b)
	{
		float rgba[] = { r, g, b, 1.0f };
		DX11Manager::getContext().ClearRenderTargetView(
			m_pRenderTargetView.Get(),
			rgba
		);
		DX11Manager::getContext().ClearDepthStencilView(
			m_pDepthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f, 0
		);
	}

	void DX11GraphicsContext::swapBuffers()
	{
		present(true);
	}

	void DX11GraphicsContext::onResize(unsigned int width, unsigned int height)
	{
		m_width = width;
		m_height = height;

		MX_DEBUG_HR_DECL;

		unsetRenderTarget();
		m_pRenderTargetView->Release();
		m_pDepthStencil->Release();
		m_pDepthStencilView->Release();
		DX11Manager::flushContext();

		MX_VERIFY_THROW_HR(
			m_pSwapChain->ResizeBuffers(
				0, 0, 0,
				DXGI_FORMAT_UNKNOWN,
				0
			)
		);

		createRenderTargetView();
		createDepthStencil();
		createDepthStencilView();

		setViewport();
		setRenderTarget();
	}

	void DX11GraphicsContext::present(bool vSyncEnabled)
	{
		MX_DEBUG_HR_DECL;

		MX_VERIFY_THROW_HR(
			m_pSwapChain->Present(
				vSyncEnabled, 0
			)
		);
		// Present with SWAP_EFFECT_FLIP_DISCARD unbinds the render target
		DX11Manager::getContext().OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
	}

	void DX11GraphicsContext::createSwapChain()
	{
		MX_DEBUG_HR_DECL;

		DXGI_SWAP_CHAIN_DESC scDesc;
		scDesc.BufferDesc.Width = m_width;
		scDesc.BufferDesc.Height = m_height;
		scDesc.BufferDesc.RefreshRate.Numerator = 144;
		scDesc.BufferDesc.RefreshRate.Denominator = 1;
		scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scDesc.SampleDesc.Count = 1;
		scDesc.SampleDesc.Quality = 0;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.BufferCount = 2;
		scDesc.OutputWindow = m_wndHandle;
		scDesc.Windowed = TRUE;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.Flags = 0;

		MX_VERIFY_THROW_HR_INFO(
			DX11Manager::getFactory().CreateSwapChain(
				&DX11Manager::getDevice(),
				&scDesc,
				m_pSwapChain.GetAddressOf()
			)
		);
	}

	void DX11GraphicsContext::createRenderTargetView()
	{
		MX_DEBUG_HR_DECL;

		ID3D11Texture2D* pBuffer;
		MX_VERIFY_THROW_HR(
			m_pSwapChain->GetBuffer(
				0, __uuidof(ID3D11Texture2D),
				(void**)&pBuffer
			)
		);

		D3D11_RENDER_TARGET_VIEW_DESC targetDesc;
		targetDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		targetDesc.Texture2D.MipSlice = 0;

		MX_VERIFY_THROW_HR(
			DX11Manager::getDevice().CreateRenderTargetView(
				pBuffer,
				&targetDesc,
				m_pRenderTargetView.GetAddressOf()
			)
		);
		pBuffer->Release();
	}

	void DX11GraphicsContext::createDepthStencil()
	{
		MX_DEBUG_HR_DECL;

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = m_width;
		depthStencilDesc.Height = m_height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		MX_VERIFY_THROW_HR(
			DX11Manager::getDevice().CreateTexture2D(
				&depthStencilDesc,
				NULL,
				m_pDepthStencil.GetAddressOf()
			)
		);
	}

	void DX11GraphicsContext::createDepthStencilView()
	{
		MX_DEBUG_HR_DECL;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = 0;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		MX_VERIFY_THROW_HR(
			DX11Manager::getDevice().CreateDepthStencilView(
				m_pDepthStencil.Get(),
				&depthStencilViewDesc,
				m_pDepthStencilView.GetAddressOf()
			)
		);
	}

	void DX11GraphicsContext::setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		m_viewport.Width = (float)width;
		m_viewport.Height = (float)height;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1;
		m_viewport.TopLeftX = (float)x;
		m_viewport.TopLeftY = (float)y;

		setViewport();
	}

	void DX11GraphicsContext::DX11Manager::init()
	{
		if (s_initialized > 0)
		{
			++s_initialized;
			return;
		}

		MX_CORE_WARN("DX11Manager is not initialized yet. Initializing it now");

		MX_DEBUG_HR_DECL;

		D3D_FEATURE_LEVEL fLvls[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		#ifdef MX_DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		MX_VERIFY_THROW_HR(
			D3D11CreateDevice(
				NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				flags,
				fLvls,
				ARRAYSIZE(fLvls),
				D3D11_SDK_VERSION,
				&s_pDevice,
				NULL,
				&s_pContext
			)
		);

		++s_initialized;
	}

	void DX11GraphicsContext::DX11Manager::shutdown()
	{
		MX_CORE_ASSERT(s_initialized > 0, "Cannot shutdown non-initialized DX11Manager!");
		if (s_initialized > 1)
		{
			--s_initialized;
			return;
		}

		MX_CORE_INFO("Shutting down DX11Manager");

		s_pDevice.Reset();
		s_pContext.Reset();

		--s_initialized;
	}

	IDXGIFactory& DX11GraphicsContext::DX11Manager::getFactory()
	{
		MX_DEBUG_HR_DECL;

		IDXGIDevice* pDXGIDevice = nullptr;
		MX_VERIFY_THROW_HR(
			s_pDevice->QueryInterface(
				__uuidof(IDXGIDevice),
				(void**)&pDXGIDevice
			)
		);

		IDXGIAdapter* pDXGIAdapter = nullptr;
		MX_VERIFY_THROW_HR(
			pDXGIDevice->GetAdapter(
				&pDXGIAdapter
			)
		);

		IDXGIFactory* pIDXGIFactory = nullptr;
		MX_VERIFY_THROW_HR(
			pDXGIAdapter->GetParent(
				__uuidof(IDXGIFactory),
				(void**)&pIDXGIFactory
			)
		);

		return *pIDXGIFactory;
	}
}