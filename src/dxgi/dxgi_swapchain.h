#pragma once

#include <memory>
#include <mutex>

#include <dxvk_surface.h>
#include <dxvk_swapchain.h>

#include "dxgi_interfaces.h"
#include "dxgi_object.h"
#include "dxgi_presenter.h"

#include "../d3d11/d3d11_interfaces.h"

#include "../spirv/spirv_module.h"

namespace dxvk {
  
  class DxgiFactory;
  
  class DxgiSwapChain : public DxgiObject<IDXGISwapChain> {
    
  public:
    
    DxgiSwapChain(
          DxgiFactory*          factory,
          IUnknown*             pDevice,
          DXGI_SWAP_CHAIN_DESC* pDesc);
    ~DxgiSwapChain();
    
    HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID  riid,
            void**  ppvObject) final;
            
    HRESULT STDMETHODCALLTYPE GetParent(
            REFIID  riid,
            void**  ppParent) final;
    
    HRESULT STDMETHODCALLTYPE GetDevice(
            REFIID  riid,
            void**  ppDevice) final;
    
    HRESULT STDMETHODCALLTYPE GetBuffer(
            UINT    Buffer,
            REFIID  riid,
            void**  ppSurface) final;
    
    HRESULT STDMETHODCALLTYPE GetContainingOutput(
            IDXGIOutput **ppOutput) final;
    
    HRESULT STDMETHODCALLTYPE GetDesc(
            DXGI_SWAP_CHAIN_DESC *pDesc) final;
    
    HRESULT STDMETHODCALLTYPE GetFrameStatistics(
            DXGI_FRAME_STATISTICS *pStats) final;
    
    HRESULT STDMETHODCALLTYPE GetFullscreenState(
            BOOL        *pFullscreen,
            IDXGIOutput **ppTarget) final;
    
    HRESULT STDMETHODCALLTYPE GetLastPresentCount(
            UINT *pLastPresentCount) final;
    
    HRESULT STDMETHODCALLTYPE Present(
            UINT SyncInterval,
            UINT Flags) final;
    
    HRESULT STDMETHODCALLTYPE ResizeBuffers(
            UINT        BufferCount,
            UINT        Width,
            UINT        Height,
            DXGI_FORMAT NewFormat,
            UINT        SwapChainFlags) final;
    
    HRESULT STDMETHODCALLTYPE ResizeTarget(
      const DXGI_MODE_DESC *pNewTargetParameters) final;
    
    HRESULT STDMETHODCALLTYPE SetFullscreenState(
            BOOL        Fullscreen,
            IDXGIOutput *pTarget) final;

  private:
    
    std::mutex m_mutex;
    
    Com<DxgiFactory>                m_factory;
    Com<IDXGIAdapterPrivate>        m_adapter;
    Com<IDXGIDevicePrivate>         m_device;
    Com<IDXGIPresentDevicePrivate>  m_presentDevice;
    
    DXGI_SWAP_CHAIN_DESC  m_desc;
    DXGI_FRAME_STATISTICS m_stats;
    
    SDL_Window*         m_window = nullptr;
    
    Rc<DxgiPresenter>   m_presenter;
    Com<IUnknown>       m_backBufferIface;
    
    void createPresenter();
    void createBackBuffer();
    
    void createContext();
    
    VkExtent2D getWindowSize() const;
    
    HRESULT GetSampleCount(
            UINT                    Count,
            VkSampleCountFlagBits*  pCount) const;
        
  };
  
}
