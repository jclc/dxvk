#include "dxbc_util.h"

namespace dxvk {
  
  uint32_t computeResourceSlotId(
          DxbcProgramType shaderStage,
          DxbcBindingType bindingType,
          uint32_t        bindingIndex) {
    if (shaderStage == DxbcProgramType::ComputeShader) {
      //   0 -  13: Constant buffers
      //  14 -  29: Samplers
      //  30 - 157: Shader resources
      // 158 - 221: Uniform access views
      switch (bindingType) {
        case DxbcBindingType::ConstantBuffer:     return bindingIndex +   0;
        case DxbcBindingType::ImageSampler:       return bindingIndex +  14;
        case DxbcBindingType::ShaderResource:     return bindingIndex +  30;
        case DxbcBindingType::UnorderedAccessView:return bindingIndex + 158;
        default: Logger::err("computeResourceSlotId: Invalid resource type");
      }
    } else {
      // Global resource slots
      //   0 -   7: Uniform access views
      //   8 -  11: Stream output buffers
      // Per-stage resource slots:
      //   0 -  13: Constant buffers
      //  14 -  29: Samplers
      //  30 - 157: Shader resources
      const uint32_t stageOffset = 12 + 158 * static_cast<uint32_t>(shaderStage);
      
      switch (bindingType) {
        case DxbcBindingType::UnorderedAccessView:return bindingIndex + 0;
        case DxbcBindingType::StreamOutputBuffer: return bindingIndex + 8;
        case DxbcBindingType::ConstantBuffer:     return bindingIndex + stageOffset +  0;
        case DxbcBindingType::ImageSampler:       return bindingIndex + stageOffset + 14;
        case DxbcBindingType::ShaderResource:     return bindingIndex + stageOffset + 30;
        default: Logger::err("computeResourceSlotId: Invalid resource type");
      }
    }
    
    return 0;
  }
  
  uint32_t primitiveVertexCount(DxbcPrimitive primitive) {
    static const std::array<uint32_t, 8> s_vertexCounts = {
       0, // Undefined
       1, // Point
       2, // Line
       3, // Triangle
       0, // Undefined
       0, // Undefined
       4, // Line with adjacency
       6, // Triangle with adjacency
    };
    
    if (primitive >= DxbcPrimitive::Patch1) {
      return static_cast<uint32_t>(primitive)
           - static_cast<uint32_t>(DxbcPrimitive::Patch1);
    } else {
      return s_vertexCounts.at(
        static_cast<uint32_t>(primitive));
    }
  }
  
}