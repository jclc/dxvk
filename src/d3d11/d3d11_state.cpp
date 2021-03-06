#include "d3d11_state.h"

namespace dxvk {
  
  size_t D3D11StateDescHash::operator () (
    const D3D11_BLEND_DESC& desc) const {
    DxvkHashState hash;
    hash.add(desc.AlphaToCoverageEnable);
    hash.add(desc.IndependentBlendEnable);
    
    // Render targets 1 to 7 are ignored and may contain
    // undefined data if independent blend is disabled
    const uint32_t usedRenderTargets = desc.IndependentBlendEnable ? 8 : 1;
    
    for (uint32_t i = 0; i < usedRenderTargets; i++)
      hash.add(this->operator () (desc.RenderTarget[i]));
    
    return hash;
  }
  
  
  size_t D3D11StateDescHash::operator () (
    const D3D11_DEPTH_STENCILOP_DESC& desc) const {
    DxvkHashState hash;
    hash.add(desc.StencilFunc);
    hash.add(desc.StencilDepthFailOp);
    hash.add(desc.StencilPassOp);
    hash.add(desc.StencilFailOp);
    return hash;
  }
  
  
  size_t D3D11StateDescHash::operator () (
    const D3D11_DEPTH_STENCIL_DESC& desc) const {
    DxvkHashState hash;
    hash.add(desc.DepthEnable);
    hash.add(desc.DepthWriteMask);
    hash.add(desc.DepthFunc);
    hash.add(desc.StencilEnable);
    hash.add(desc.StencilReadMask);
    hash.add(desc.StencilWriteMask);
    hash.add(this->operator () (desc.FrontFace));
    hash.add(this->operator () (desc.BackFace));
    return hash;
  }
  
  
  size_t D3D11StateDescHash::operator () (
    const D3D11_RASTERIZER_DESC& desc) const {
    DxvkHashState hash;
    hash.add(desc.FillMode);
    hash.add(desc.CullMode);
    hash.add(desc.FrontCounterClockwise);
    hash.add(desc.DepthBias);
    hash.add(desc.SlopeScaledDepthBias);
    hash.add(desc.DepthBiasClamp);
    hash.add(desc.DepthClipEnable);
    hash.add(desc.ScissorEnable);
    hash.add(desc.MultisampleEnable);
    hash.add(desc.AntialiasedLineEnable);
    return hash;
  }
  
  
  size_t D3D11StateDescHash::operator () (
    const D3D11_RENDER_TARGET_BLEND_DESC& desc) const {
    DxvkHashState hash;
    hash.add(desc.BlendEnable);
    hash.add(desc.SrcBlend);
    hash.add(desc.DestBlend);
    hash.add(desc.BlendOp);
    hash.add(desc.SrcBlendAlpha);
    hash.add(desc.DestBlendAlpha);
    hash.add(desc.BlendOpAlpha);
    hash.add(desc.RenderTargetWriteMask);
    return hash;
  }
  
  
  bool D3D11StateDescEqual::operator () (
    const D3D11_BLEND_DESC& a,
    const D3D11_BLEND_DESC& b) const {
    bool eq = a.AlphaToCoverageEnable  == b.AlphaToCoverageEnable
           && a.IndependentBlendEnable == b.IndependentBlendEnable;
    
    // Render targets 1 to 7 are ignored and may contain
    // undefined data if independent blend is disabled
    const uint32_t usedRenderTargets = a.IndependentBlendEnable ? 8 : 1;
    
    for (uint32_t i = 0; !eq && (i < usedRenderTargets); i++)
      eq &= this->operator () (a.RenderTarget[i], b.RenderTarget[i]);
    
    return eq;
  }
  
  
  bool D3D11StateDescEqual::operator () (
    const D3D11_DEPTH_STENCILOP_DESC& a,
    const D3D11_DEPTH_STENCILOP_DESC& b) const {
    return a.StencilFunc           == b.StencilFunc
        && a.StencilDepthFailOp    == b.StencilDepthFailOp
        && a.StencilPassOp         == b.StencilPassOp
        && a.StencilFailOp         == b.StencilFailOp;
  }
  
  
  bool D3D11StateDescEqual::operator () (
    const D3D11_DEPTH_STENCIL_DESC& a,
    const D3D11_DEPTH_STENCIL_DESC& b) const {
    return a.DepthEnable           == b.DepthEnable
        && a.DepthWriteMask        == b.DepthWriteMask
        && a.DepthFunc             == b.DepthFunc
        && a.StencilEnable         == b.StencilEnable
        && a.StencilReadMask       == b.StencilReadMask
        && a.StencilWriteMask      == b.StencilWriteMask
        && this->operator () (a.FrontFace, b.FrontFace)
        && this->operator () (a.BackFace, b.BackFace);
  }
  
  
  bool D3D11StateDescEqual::operator () (
    const D3D11_RASTERIZER_DESC& a,
    const D3D11_RASTERIZER_DESC& b) const {
    return a.FillMode              == b.FillMode
        && a.CullMode              == b.CullMode
        && a.FrontCounterClockwise == b.FrontCounterClockwise
        && a.DepthBias             == b.DepthBias
        && a.SlopeScaledDepthBias  == b.SlopeScaledDepthBias
        && a.DepthBiasClamp        == b.DepthBiasClamp
        && a.DepthClipEnable       == b.DepthClipEnable
        && a.ScissorEnable         == b.ScissorEnable
        && a.MultisampleEnable     == b.MultisampleEnable
        && a.AntialiasedLineEnable == b.AntialiasedLineEnable;
  }
  
  
  bool D3D11StateDescEqual::operator () (
    const D3D11_RENDER_TARGET_BLEND_DESC& a,
    const D3D11_RENDER_TARGET_BLEND_DESC& b) const {
    return a.BlendEnable           == b.BlendEnable
        && a.SrcBlend              == b.SrcBlend
        && a.DestBlend             == b.DestBlend
        && a.BlendOp               == b.BlendOp
        && a.SrcBlendAlpha         == b.SrcBlendAlpha
        && a.DestBlendAlpha        == b.DestBlendAlpha
        && a.BlendOpAlpha          == b.BlendOpAlpha
        && a.RenderTargetWriteMask == b.RenderTargetWriteMask;
  }
  
}