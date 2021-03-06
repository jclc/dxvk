#include "dxbc_compiler.h"

namespace dxvk {
  
  constexpr uint32_t PerVertex_Position  = 0;
  constexpr uint32_t PerVertex_PointSize = 1;
  constexpr uint32_t PerVertex_CullDist  = 2;
  constexpr uint32_t PerVertex_ClipDist  = 3;
  
  DxbcCompiler::DxbcCompiler(
    const DxbcProgramVersion& version,
    const Rc<DxbcIsgn>&       isgn,
    const Rc<DxbcIsgn>&       osgn)
  : m_version (version),
    m_isgn    (isgn),
    m_osgn    (osgn) {
    // Declare an entry point ID. We'll need it during the
    // initialization phase where the execution mode is set.
    m_entryPointId = m_module.allocateId();
    
    // Set the memory model. This is the same for all shaders.
    m_module.setMemoryModel(
      spv::AddressingModelLogical,
      spv::MemoryModelGLSL450);
    
    // Make sure our interface registers are clear
    for (uint32_t i = 0; i < DxbcMaxInterfaceRegs; i++) {
      m_ps.oTypes.at(i).ctype  = DxbcScalarType::Float32;
      m_ps.oTypes.at(i).ccount = 0;
      
      m_vRegs.at(i) = 0;
      m_oRegs.at(i) = 0;
    }
    
    // Initialize the shader module with capabilities
    // etc. Each shader type has its own peculiarities.
    switch (m_version.type()) {
      case DxbcProgramType::VertexShader:   this->emitVsInit(); break;
      case DxbcProgramType::GeometryShader: this->emitGsInit(); break;
      case DxbcProgramType::PixelShader:    this->emitPsInit(); break;
      default: throw DxvkError("DxbcCompiler: Unsupported program type");
    }
  }
  
  
  DxbcCompiler::~DxbcCompiler() {
    
  }
  
  
  void DxbcCompiler::processInstruction(const DxbcShaderInstruction& ins) {
    switch (ins.opClass) {
      case DxbcInstClass::Declaration:
        return this->emitDcl(ins);
        
      case DxbcInstClass::ControlFlow:
        return this->emitControlFlow(ins);
        
      case DxbcInstClass::GeometryEmit:
        return this->emitGeometryEmit(ins);
        
      case DxbcInstClass::TextureSample:
        return this->emitSample(ins);
        
      case DxbcInstClass::VectorAlu:
        return this->emitVectorAlu(ins);
        
      case DxbcInstClass::VectorCmov:
        return this->emitVectorCmov(ins);
        
      case DxbcInstClass::VectorCmp:
        return this->emitVectorCmp(ins);
        
      case DxbcInstClass::VectorDot:
        return this->emitVectorDot(ins);
        
      case DxbcInstClass::VectorIdiv:
        return this->emitVectorIdiv(ins);
        
      case DxbcInstClass::VectorImul:
        return this->emitVectorImul(ins);
        
      case DxbcInstClass::VectorSinCos:
        return this->emitVectorSinCos(ins);
        
      default:
        Logger::warn(
          str::format("DxbcCompiler: Unhandled opcode class: ",
          ins.op));
    }
  }
  
  
  Rc<DxvkShader> DxbcCompiler::finalize() {
    // Define the actual 'main' function of the shader
    m_module.functionBegin(
      m_module.defVoidType(),
      m_entryPointId,
      m_module.defFunctionType(
        m_module.defVoidType(), 0, nullptr),
      spv::FunctionControlMaskNone);
    m_module.opLabel(m_module.allocateId());
    
    // Depending on the shader type, this will prepare
    // input registers, call various shader functions
    // and write back the output registers.
    switch (m_version.type()) {
      case DxbcProgramType::VertexShader:   this->emitVsFinalize(); break;
      case DxbcProgramType::GeometryShader: this->emitGsFinalize(); break;
      case DxbcProgramType::PixelShader:    this->emitPsFinalize(); break;
      default: throw DxvkError("DxbcCompiler: Unsupported program type");
    }
    
    // End main function
    m_module.opReturn();
    m_module.functionEnd();
    
    // Declare the entry point, we now have all the
    // information we need, including the interfaces
    m_module.addEntryPoint(m_entryPointId,
      m_version.executionModel(), "main",
      m_entryPointInterfaces.size(),
      m_entryPointInterfaces.data());
    m_module.setDebugName(m_entryPointId, "main");
    
    // Create the shader module object
    return new DxvkShader(
      m_version.shaderStage(),
      m_resourceSlots.size(),
      m_resourceSlots.data(),
      m_module.compile());
  }
  
  
  void DxbcCompiler::emitDcl(const DxbcShaderInstruction& ins) {
    switch (ins.op) {
      case DxbcOpcode::DclGlobalFlags:
        return this->emitDclGlobalFlags(ins);
        
      case DxbcOpcode::DclTemps:
        return this->emitDclTemps(ins);
        
      case DxbcOpcode::DclInput:
      case DxbcOpcode::DclInputSgv:
      case DxbcOpcode::DclInputSiv:
      case DxbcOpcode::DclInputPs:
      case DxbcOpcode::DclInputPsSgv:
      case DxbcOpcode::DclInputPsSiv:
      case DxbcOpcode::DclOutput:
      case DxbcOpcode::DclOutputSgv:
      case DxbcOpcode::DclOutputSiv:
        return this->emitDclInterfaceReg(ins);
        
      case DxbcOpcode::DclConstantBuffer:
        return this->emitDclConstantBuffer(ins);
        
      case DxbcOpcode::DclSampler:
        return this->emitDclSampler(ins);
        
      case DxbcOpcode::DclResource:
        return this->emitDclResource(ins);
      
      case DxbcOpcode::DclGsInputPrimitive:
        return this->emitDclGsInputPrimitive(ins);
        
      case DxbcOpcode::DclGsOutputPrimitiveTopology:
        return this->emitDclGsOutputTopology(ins);
        
      case DxbcOpcode::DclMaxOutputVertexCount:
        return this->emitDclMaxOutputVertexCount(ins);
      
      default:
        Logger::warn(
          str::format("DxbcCompiler: Unhandled opcode: ",
          ins.op));
    }
  }
  
  
  void DxbcCompiler::emitDclGlobalFlags(const DxbcShaderInstruction& ins) {
    // TODO implement properly
  }
  
  
  void DxbcCompiler::emitDclTemps(const DxbcShaderInstruction& ins) {
    // dcl_temps has one operand:
    //    (imm0) Number of temp registers
    const uint32_t oldCount = m_rRegs.size();
    const uint32_t newCount = ins.imm[0].u32;
    
    if (newCount > oldCount) {
      m_rRegs.resize(newCount);
      
      DxbcRegisterInfo info;
      info.type.ctype   = DxbcScalarType::Float32;
      info.type.ccount  = 4;
      info.type.alength = 0;
      info.sclass       = spv::StorageClassPrivate;
      
      for (uint32_t i = oldCount; i < newCount; i++) {
        const uint32_t varId = this->emitNewVariable(info);
        m_module.setDebugName(varId, str::format("r", i).c_str());
        m_rRegs.at(i) = varId;
      }
    }
  }
  
  
  void DxbcCompiler::emitDclInterfaceReg(const DxbcShaderInstruction& ins) {
    // dcl_input and dcl_output instructions
    // have the following operands:
    //    (dst0) The register to declare
    //    (imm0) The system value (optional)
    uint32_t regDim = 0;
    uint32_t regIdx = 0;
    
    // In the vertex and fragment shader stage, the
    // operand indices will have the following format:
    //    (0) Register index
    // 
    // In other stages, the input and output registers
    // may be declared as arrays of a fixed size:
    //    (0) Array length
    //    (1) Register index
    if (ins.dst[0].idxDim == 2) {
      regDim = ins.dst[0].idx[0].offset;
      regIdx = ins.dst[0].idx[1].offset;
    } else if (ins.dst[0].idxDim == 1) {
      regIdx = ins.dst[0].idx[0].offset;
    } else {
      Logger::err(str::format(
        "DxbcCompiler: ", ins.op,
        ": Invalid index dimension"));
      return;
    }
    
    // This declaration may map an output register to a system
    // value. If that is the case, the system value type will
    // be stored in the second operand.
    const bool hasSv =
         ins.op == DxbcOpcode::DclInputSgv
      || ins.op == DxbcOpcode::DclInputSiv
      || ins.op == DxbcOpcode::DclInputPsSgv
      || ins.op == DxbcOpcode::DclInputPsSiv
      || ins.op == DxbcOpcode::DclOutputSgv
      || ins.op == DxbcOpcode::DclOutputSiv;
    
    DxbcSystemValue sv = DxbcSystemValue::None;
    
    if (hasSv)
      sv = static_cast<DxbcSystemValue>(ins.imm[0].u32);
    
    // In the pixel shader, inputs are declared with an
    // interpolation mode that is part of the op token.
    const bool hasInterpolationMode =
         ins.op == DxbcOpcode::DclInputPs
      || ins.op == DxbcOpcode::DclInputPsSiv;
    
    DxbcInterpolationMode im = DxbcInterpolationMode::Undefined;
    
    if (hasInterpolationMode)
      im = ins.controls.interpolation;
    
    // Declare the actual input/output variable
    switch (ins.op) {
      case DxbcOpcode::DclInput:
      case DxbcOpcode::DclInputSgv:
      case DxbcOpcode::DclInputSiv:
      case DxbcOpcode::DclInputPs:
      case DxbcOpcode::DclInputPsSgv:
      case DxbcOpcode::DclInputPsSiv:
        this->emitDclInput(regIdx, regDim, ins.dst[0].mask, sv, im);
        break;
      
      case DxbcOpcode::DclOutput:
      case DxbcOpcode::DclOutputSgv:
      case DxbcOpcode::DclOutputSiv:
        this->emitDclOutput(regIdx, regDim, ins.dst[0].mask, sv, im);
        break;
      
      default:
        Logger::err(str::format(
          "DxbcCompiler: Unexpected opcode: ",
          ins.op));
    }
  }
  
  
  void DxbcCompiler::emitDclInput(
          uint32_t                regIdx,
          uint32_t                regDim,
          DxbcRegMask             regMask,
          DxbcSystemValue         sv,
          DxbcInterpolationMode   im) {
    // Avoid declaring the same variable multiple times.
    // This may happen when multiple system values are
    // mapped to different parts of the same register.
    if (m_vRegs.at(regIdx) == 0) {
      DxbcRegisterInfo info;
      info.type.ctype   = DxbcScalarType::Float32;
      info.type.ccount  = 4;
      info.type.alength = regDim;
      info.sclass = spv::StorageClassInput;
      
      uint32_t varId = this->emitNewVariable(info);
      
      m_module.decorateLocation(varId, regIdx);
      m_module.setDebugName(varId, str::format("v", regIdx).c_str());
      m_entryPointInterfaces.push_back(varId);
      
      m_vRegs.at(regIdx) = varId;
      
      // Interpolation mode, used in pixel shaders
      if (im == DxbcInterpolationMode::Constant)
        m_module.decorate(varId, spv::DecorationFlat);
      
      if (im == DxbcInterpolationMode::LinearCentroid
       || im == DxbcInterpolationMode::LinearNoPerspectiveCentroid)
        m_module.decorate(varId, spv::DecorationCentroid);
      
      if (im == DxbcInterpolationMode::LinearNoPerspective
       || im == DxbcInterpolationMode::LinearNoPerspectiveCentroid
       || im == DxbcInterpolationMode::LinearNoPerspectiveSample)
        m_module.decorate(varId, spv::DecorationNoPerspective);
      
      if (im == DxbcInterpolationMode::LinearSample
       || im == DxbcInterpolationMode::LinearNoPerspectiveSample)
        m_module.decorate(varId, spv::DecorationSample);
    }
    
    // Add a new system value mapping if needed
    // TODO declare SV if necessary
    if (sv != DxbcSystemValue::None)
      m_vMappings.push_back({ regIdx, regMask, sv });
  }
  
  
  void DxbcCompiler::emitDclOutput(
          uint32_t                regIdx,
          uint32_t                regDim,
          DxbcRegMask             regMask,
          DxbcSystemValue         sv,
          DxbcInterpolationMode   im) {
    // Avoid declaring the same variable multiple times.
    // This may happen when multiple system values are
    // mapped to different parts of the same register.
    if (m_oRegs.at(regIdx) == 0) {
      DxbcRegisterInfo info;
      info.type.ctype   = DxbcScalarType::Float32;
      info.type.ccount  = 4;
      info.type.alength = regDim;
      info.sclass = spv::StorageClassOutput;
      
      const uint32_t varId = this->emitNewVariable(info);
      
      m_module.decorateLocation(varId, regIdx);
      m_module.setDebugName(varId, str::format("o", regIdx).c_str());
      m_entryPointInterfaces.push_back(varId);
      
      m_oRegs.at(regIdx) = varId;
    }
    
    
    // Add a new system value mapping if needed
    // TODO declare SV if necessary
    if (sv != DxbcSystemValue::None)
      m_oMappings.push_back({ regIdx, regMask, sv });
  }
  
  
  void DxbcCompiler::emitDclConstantBuffer(const DxbcShaderInstruction& ins) {
    // dcl_constant_buffer has one operand with two indices:
    //    (0) Constant buffer register ID (cb#)
    //    (1) Number of constants in the buffer
    const uint32_t bufferId     = ins.dst[0].idx[0].offset;
    const uint32_t elementCount = ins.dst[0].idx[1].offset;
    
    // Uniform buffer data is stored as a fixed-size array
    // of 4x32-bit vectors. SPIR-V requires explicit strides.
    const uint32_t arrayType = m_module.defArrayTypeUnique(
      getVectorTypeId({ DxbcScalarType::Float32, 4 }),
      m_module.constu32(elementCount));
    m_module.decorateArrayStride(arrayType, 16);
    
    // SPIR-V requires us to put that array into a
    // struct and decorate that struct as a block.
    const uint32_t structType = m_module.defStructTypeUnique(1, &arrayType);
    m_module.memberDecorateOffset(structType, 0, 0);
    m_module.decorateBlock(structType);
    
    // Variable that we'll use to access the buffer
    const uint32_t varId = m_module.newVar(
      m_module.defPointerType(structType, spv::StorageClassUniform),
      spv::StorageClassUniform);
    
    m_module.setDebugName(varId,
      str::format("cb", bufferId).c_str());
    
    m_constantBuffers.at(bufferId).varId = varId;
    m_constantBuffers.at(bufferId).size  = elementCount;
    
    // Compute the DXVK binding slot index for the buffer.
    // D3D11 needs to bind the actual buffers to this slot.
    const uint32_t bindingId = computeResourceSlotId(
      m_version.type(), DxbcBindingType::ConstantBuffer,
      bufferId);
    
    m_module.decorateDescriptorSet(varId, 0);
    m_module.decorateBinding(varId, bindingId);
    
    // Store descriptor info for the shader interface
    DxvkResourceSlot resource;
    resource.slot = bindingId;
    resource.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    m_resourceSlots.push_back(resource);
  }
  
  
  void DxbcCompiler::emitDclSampler(const DxbcShaderInstruction& ins) {
    // dclSampler takes one operand:
    //    (dst0) The sampler register to declare
    // TODO implement sampler mode (default / comparison / mono)
    const uint32_t samplerId = ins.dst[0].idx[0].offset;
    
    // The sampler type is opaque, but we still have to
    // define a pointer and a variable in oder to use it
    const uint32_t samplerType = m_module.defSamplerType();
    const uint32_t samplerPtrType = m_module.defPointerType(
      samplerType, spv::StorageClassUniformConstant);
    
    // Define the sampler variable
    const uint32_t varId = m_module.newVar(samplerPtrType,
      spv::StorageClassUniformConstant);
    m_module.setDebugName(varId,
      str::format("s", samplerId).c_str());
    
    m_samplers.at(samplerId).varId  = varId;
    m_samplers.at(samplerId).typeId = samplerType;
    
    // Compute binding slot index for the sampler
    const uint32_t bindingId = computeResourceSlotId(
      m_version.type(), DxbcBindingType::ImageSampler, samplerId);
    
    m_module.decorateDescriptorSet(varId, 0);
    m_module.decorateBinding(varId, bindingId);
    
    // Store descriptor info for the shader interface
    DxvkResourceSlot resource;
    resource.slot = bindingId;
    resource.type = VK_DESCRIPTOR_TYPE_SAMPLER;
    m_resourceSlots.push_back(resource);
  }
  
  
  void DxbcCompiler::emitDclResource(const DxbcShaderInstruction& ins) {
    // dclResource takes two operands:
    //  (dst0) The resource register ID
    //  (imm0) The resource return type
    const uint32_t registerId = ins.dst[0].idx[0].offset;
    
    // Defines the type of the resource (texture2D, ...)
    const DxbcResourceDim resourceType = ins.controls.resourceDim;
    
    // Defines the type of a read operation. DXBC has the ability
    // to define four different types whereas SPIR-V only allows
    // one, but in practice this should not be much of a problem.
    auto xType = static_cast<DxbcResourceReturnType>(
      bit::extract(ins.imm[0].u32, 0, 3));
    auto yType = static_cast<DxbcResourceReturnType>(
      bit::extract(ins.imm[0].u32, 4, 7));
    auto zType = static_cast<DxbcResourceReturnType>(
      bit::extract(ins.imm[0].u32, 8, 11));
    auto wType = static_cast<DxbcResourceReturnType>(
      bit::extract(ins.imm[0].u32, 12, 15));
    
    if ((xType != yType) || (xType != zType) || (xType != wType))
      Logger::warn("DxbcCompiler: dcl_resource: Ignoring resource return types");
    
    // Declare the actual sampled type
    uint32_t sampledTypeId = 0;
    
    switch (xType) {
      case DxbcResourceReturnType::Float: sampledTypeId = m_module.defFloatType(32);    break;
      case DxbcResourceReturnType::Sint:  sampledTypeId = m_module.defIntType  (32, 1); break;
      case DxbcResourceReturnType::Uint:  sampledTypeId = m_module.defIntType  (32, 0); break;
      default: throw DxvkError(str::format("DxbcCompiler: Invalid sampled type: ", xType));
    }
    
    // Declare the resource type
    uint32_t textureTypeId = 0;
    
    switch (resourceType) {
      case DxbcResourceDim::Texture1D:
        textureTypeId = m_module.defImageType(
          sampledTypeId, spv::Dim1D, 0, 0, 0, 1,
          spv::ImageFormatUnknown);
        break;
      
      case DxbcResourceDim::Texture1DArr:
        textureTypeId = m_module.defImageType(
          sampledTypeId, spv::Dim1D, 0, 1, 0, 1,
          spv::ImageFormatUnknown);
        break;
      
      case DxbcResourceDim::Texture2D:
        textureTypeId = m_module.defImageType(
          sampledTypeId, spv::Dim2D, 0, 0, 0, 1,
          spv::ImageFormatUnknown);
        break;
      
      case DxbcResourceDim::Texture2DArr:
        textureTypeId = m_module.defImageType(
          sampledTypeId, spv::Dim2D, 0, 1, 0, 1,
          spv::ImageFormatUnknown);
        break;
      
      case DxbcResourceDim::Texture3D:
        textureTypeId = m_module.defImageType(
          sampledTypeId, spv::Dim3D, 0, 0, 0, 1,
          spv::ImageFormatUnknown);
        break;
      
      case DxbcResourceDim::TextureCube:
        textureTypeId = m_module.defImageType(
          sampledTypeId, spv::DimCube, 0, 0, 0, 1,
          spv::ImageFormatUnknown);
        break;
      
      case DxbcResourceDim::TextureCubeArr:
        textureTypeId = m_module.defImageType(
          sampledTypeId, spv::DimCube, 0, 1, 0, 1,
          spv::ImageFormatUnknown);
        break;
        
      default:
        throw DxvkError(str::format("DxbcCompiler: Unsupported resource type: ", resourceType));
    }
    
    const uint32_t resourcePtrType = m_module.defPointerType(
      textureTypeId, spv::StorageClassUniformConstant);
    
    const uint32_t varId = m_module.newVar(resourcePtrType,
      spv::StorageClassUniformConstant);
    
    m_module.setDebugName(varId,
      str::format("t", registerId).c_str());
    
    m_textures.at(registerId).varId         = varId;
    m_textures.at(registerId).sampledTypeId = sampledTypeId;
    m_textures.at(registerId).textureTypeId = textureTypeId;
    
    // Compute the DXVK binding slot index for the resource.
    // D3D11 needs to bind the actual resource to this slot.
    const uint32_t bindingId = computeResourceSlotId(
      m_version.type(), DxbcBindingType::ShaderResource, registerId);
    
    m_module.decorateDescriptorSet(varId, 0);
    m_module.decorateBinding(varId, bindingId);
    
    // Store descriptor info for the shader interface
    DxvkResourceSlot resource;
    resource.slot = bindingId;
    resource.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    m_resourceSlots.push_back(resource);
  }
  
  
  void DxbcCompiler::emitDclGsInputPrimitive(const DxbcShaderInstruction& ins) {
    // The input primitive type is stored within in the
    // control bits of the opcode token. In SPIR-V, we
    // have to define an execution mode.
    const spv::ExecutionMode mode = [&] {
      switch (ins.controls.primitive) {
        case DxbcPrimitive::Point:       return spv::ExecutionModeInputPoints;
        case DxbcPrimitive::Line:        return spv::ExecutionModeInputLines;
        case DxbcPrimitive::Triangle:    return spv::ExecutionModeTriangles;
        case DxbcPrimitive::LineAdj:     return spv::ExecutionModeInputLinesAdjacency;
        case DxbcPrimitive::TriangleAdj: return spv::ExecutionModeInputTrianglesAdjacency;
        default: throw DxvkError("DxbcCompiler: Unsupported primitive type");
      }
    }();
    
    m_module.setExecutionMode(m_entryPointId, mode);
  }
  
  
  void DxbcCompiler::emitDclGsOutputTopology(const DxbcShaderInstruction& ins) {
    // The input primitive topology is stored within in the
    // control bits of the opcode token. In SPIR-V, we have
    // to define an execution mode.
    const spv::ExecutionMode mode = [&] {
      switch (ins.controls.primitiveTopology) {
        case DxbcPrimitiveTopology::PointList:     return spv::ExecutionModeOutputPoints;
        case DxbcPrimitiveTopology::LineStrip:     return spv::ExecutionModeOutputLineStrip;
        case DxbcPrimitiveTopology::TriangleStrip: return spv::ExecutionModeOutputTriangleStrip;
        default: throw DxvkError("DxbcCompiler: Unsupported primitive topology");
      }
    }();
    
    m_module.setExecutionMode(m_entryPointId, mode);
  }
  
  
  void DxbcCompiler::emitDclMaxOutputVertexCount(const DxbcShaderInstruction& ins) {
    // dcl_max_output_vertex_count has one operand:
    //    (imm0) The maximum number of vertices
    m_gs.outputVertexCount = ins.imm[0].u32;
    m_module.setOutputVertices(m_entryPointId, m_gs.outputVertexCount);
  }
  
  
  void DxbcCompiler::emitVectorAlu(const DxbcShaderInstruction& ins) {
    std::array<DxbcRegisterValue, DxbcMaxOperandCount> src;
    
    for (uint32_t i = 0; i < ins.srcCount; i++)
      src.at(i) = emitRegisterLoad(ins.src[i], ins.dst[0].mask);
    
    DxbcRegisterValue dst;
    dst.type.ctype  = ins.dst[0].dataType;
    dst.type.ccount = ins.dst[0].mask.setCount();
    
    const uint32_t typeId = getVectorTypeId(dst.type);
    
    switch (ins.op) {
      case DxbcOpcode::Add:
        dst.id = m_module.opFAdd(typeId,
          src.at(0).id, src.at(1).id);
        break;
        
      case DxbcOpcode::Div:
        dst.id = m_module.opFDiv(typeId,
          src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::Exp:
        dst.id = m_module.opExp2(
          typeId, src.at(0).id);
        break;
      
      case DxbcOpcode::Log:
        dst.id = m_module.opLog2(
          typeId, src.at(0).id);
        break;
      
      case DxbcOpcode::Mad:
        dst.id = m_module.opFFma(typeId,
          src.at(0).id, src.at(1).id, src.at(2).id);
        break;
      
      case DxbcOpcode::Max:
        dst.id = m_module.opFMax(typeId,
          src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::Min:
        dst.id = m_module.opFMin(typeId,
          src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::Mul:
        dst.id = m_module.opFMul(typeId,
          src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::Mov:
        dst.id = src.at(0).id;
        break;
      
      case DxbcOpcode::Sqrt:
        dst.id = m_module.opSqrt(
          typeId, src.at(0).id);
        break;
      
      case DxbcOpcode::Rsq:
        dst.id = m_module.opInverseSqrt(
          typeId, src.at(0).id);
        break;
      
      case DxbcOpcode::IAdd:
        dst.id = m_module.opIAdd(typeId,
          src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::IMad:
        dst.id = m_module.opIAdd(typeId,
          m_module.opIMul(typeId,
            src.at(0).id, src.at(1).id),
          src.at(2).id);
        break;
      
      case DxbcOpcode::IMax:
        dst.id = m_module.opSMax(typeId,
          src.at(0).id, src.at(1).id);
        break;
        
      case DxbcOpcode::IMin:
        dst.id = m_module.opSMin(typeId,
          src.at(0).id, src.at(1).id);
        break;
        
      case DxbcOpcode::INeg:
        dst.id = m_module.opSNegate(
          typeId, src.at(0).id);
        break;
        
      default:
        Logger::warn(str::format(
          "DxbcCompiler: Unhandled instruction: ",
          ins.op));
        return;
    }
    
    // Store computed value
    dst = emitDstOperandModifiers(dst, ins.modifiers);
    emitRegisterStore(ins.dst[0], dst);
  }
  
  
  void DxbcCompiler::emitVectorCmov(const DxbcShaderInstruction& ins) {
    // movc has four operands:
    //    (dst0) The destination register
    //    (src0) The condition vector
    //    (src0) Vector to select from if the condition is not 0
    //    (src0) Vector to select from if the condition is 0
    const DxbcRegisterValue condition   = emitRegisterLoad(ins.src[0], ins.dst[0].mask);
    const DxbcRegisterValue selectTrue  = emitRegisterLoad(ins.src[1], ins.dst[0].mask);
    const DxbcRegisterValue selectFalse = emitRegisterLoad(ins.src[2], ins.dst[0].mask);
    
    const uint32_t componentCount = ins.dst[0].mask.setCount();
    
    // We'll compare against a vector of zeroes to generate a
    // boolean vector, which in turn will be used by OpSelect
    uint32_t zeroType = m_module.defIntType(32, 0);
    uint32_t boolType = m_module.defBoolType();
    
    uint32_t zero = m_module.constu32(0);
    
    if (componentCount > 1) {
      zeroType = m_module.defVectorType(zeroType, componentCount);
      boolType = m_module.defVectorType(boolType, componentCount);
      
      const std::array<uint32_t, 4> zeroVec = { zero, zero, zero, zero };
      zero = m_module.constComposite(zeroType, componentCount, zeroVec.data());
    }
    
    
    // Use the component mask to select the vector components
    DxbcRegisterValue result;
    result.type.ctype  = ins.dst[0].dataType;
    result.type.ccount = componentCount;
    result.id = m_module.opSelect(
      getVectorTypeId(result.type),
      m_module.opINotEqual(boolType, condition.id, zero),
      selectTrue.id, selectFalse.id);
    
    // Apply result modifiers to floating-point results
    result = emitDstOperandModifiers(result, ins.modifiers);
    emitRegisterStore(ins.dst[0], result);
  }
  
  void DxbcCompiler::emitVectorCmp(const DxbcShaderInstruction& ins) {
    // Compare instructions have three operands:
    //    (dst0) The destination register
    //    (src0) The first vector to compare
    //    (src1) The second vector to compare
    const std::array<DxbcRegisterValue, 2> src = {
      emitRegisterLoad(ins.src[0], ins.dst[0].mask),
      emitRegisterLoad(ins.src[1], ins.dst[0].mask),
    };
    
    const uint32_t componentCount = ins.dst[0].mask.setCount();
    
    // Condition, which is a boolean vector used
    // to select between the ~0u and 0u vectors.
    uint32_t condition     = 0;
    uint32_t conditionType = m_module.defBoolType();
    
    if (componentCount > 1)
      conditionType = m_module.defVectorType(conditionType, componentCount);
    
    switch (ins.op) {
      case DxbcOpcode::Eq:
        condition = m_module.opFOrdEqual(
          conditionType, src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::Ge:
        condition = m_module.opFOrdGreaterThanEqual(
          conditionType, src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::Lt:
        condition = m_module.opFOrdLessThan(
          conditionType, src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::Ne:
        condition = m_module.opFOrdNotEqual(
          conditionType, src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::IEq:
        condition = m_module.opIEqual(
          conditionType, src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::IGe:
        condition = m_module.opSGreaterThanEqual(
          conditionType, src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::ILt:
        condition = m_module.opSLessThan(
          conditionType, src.at(0).id, src.at(1).id);
        break;
      
      case DxbcOpcode::INe:
        condition = m_module.opINotEqual(
          conditionType, src.at(0).id, src.at(1).id);
        break;
      
      default:
        Logger::warn(str::format(
          "DxbcCompiler: Unhandled instruction: ",
          ins.op));
        return;
    }
    
    // Generate constant vectors for selection
    uint32_t sFalse = m_module.constu32( 0u);
    uint32_t sTrue  = m_module.constu32(~0u);
    
    DxbcRegisterValue result;
    result.type.ctype  = DxbcScalarType::Uint32;
    result.type.ccount = componentCount;
    
    const uint32_t typeId = getVectorTypeId(result.type);
    
    if (componentCount > 1) {
      const std::array<uint32_t, 4> vFalse = { sFalse, sFalse, sFalse, sFalse };
      const std::array<uint32_t, 4> vTrue  = { sTrue,  sTrue,  sTrue,  sTrue  };
      
      sFalse = m_module.constComposite(typeId, componentCount, vFalse.data());
      sTrue  = m_module.constComposite(typeId, componentCount, vTrue .data());
    }
    
    // Perform component-wise mask selection
    // based on the condition evaluated above.
    result.id = m_module.opSelect(
      typeId, condition, sTrue, sFalse);
    
    emitRegisterStore(ins.dst[0], result);
  }
  
  
  void DxbcCompiler::emitVectorDot(const DxbcShaderInstruction& ins) {
    const DxbcRegMask srcMask(true,
      ins.op >= DxbcOpcode::Dp2,
      ins.op >= DxbcOpcode::Dp3,
      ins.op >= DxbcOpcode::Dp4);
    
    const std::array<DxbcRegisterValue, 2> src = {
      emitRegisterLoad(ins.src[0], srcMask),
      emitRegisterLoad(ins.src[1], srcMask),
    };
    
    DxbcRegisterValue dst;
    dst.type.ctype  = ins.dst[0].dataType;
    dst.type.ccount = 1;
    
    dst.id = m_module.opDot(
      getVectorTypeId(dst.type),
      src.at(0).id,
      src.at(1).id);
    
    dst = emitDstOperandModifiers(dst, ins.modifiers);
    emitRegisterStore(ins.dst[0], dst);
  }
  
  
  void DxbcCompiler::emitVectorIdiv(const DxbcShaderInstruction& ins) {
    // udiv has four operands:
    //    (dst0) Quotient destination register
    //    (dst1) Remainder destination register
    //    (src0) The first vector to compare
    //    (src1) The second vector to compare
    if (ins.dst[0].type == DxbcOperandType::Null
     && ins.dst[1].type == DxbcOperandType::Null)
      return;
    
    // FIXME support this if applications require it
    if (ins.dst[0].type != DxbcOperandType::Null
     && ins.dst[1].type != DxbcOperandType::Null
     && ins.dst[0].mask != ins.dst[1].mask) {
      Logger::warn("DxbcCompiler: Umul with different destination masks not supported");
      return;
    }
    
    // Load source operands as integers with the
    // mask of one non-NULL destination operand
    const DxbcRegMask srcMask =
      ins.dst[0].type != DxbcOperandType::Null
        ? ins.dst[0].mask
        : ins.dst[1].mask;
    
    const std::array<DxbcRegisterValue, 2> src = {
      emitRegisterLoad(ins.src[0], srcMask),
      emitRegisterLoad(ins.src[1], srcMask),
    };
    
    // Compute results only if the destination
    // operands are not NULL.
    if (ins.dst[0].type != DxbcOperandType::Null) {
      DxbcRegisterValue quotient;
      quotient.type.ctype  = ins.dst[0].dataType;
      quotient.type.ccount = ins.dst[0].mask.setCount();
      
      quotient.id = m_module.opUDiv(
        getVectorTypeId(quotient.type),
        src.at(0).id, src.at(1).id);
      
      quotient = emitDstOperandModifiers(quotient, ins.modifiers);
      emitRegisterStore(ins.dst[0], quotient);
    }
    
    if (ins.dst[1].type != DxbcOperandType::Null) {
      DxbcRegisterValue remainder;
      remainder.type.ctype  = ins.dst[1].dataType;
      remainder.type.ccount = ins.dst[1].mask.setCount();
      
      remainder.id = m_module.opUMod(
        getVectorTypeId(remainder.type),
        src.at(0).id, src.at(1).id);
      
      remainder = emitDstOperandModifiers(remainder, ins.modifiers);
      emitRegisterStore(ins.dst[1], remainder);
    }
  }
  
  
  void DxbcCompiler::emitVectorImul(const DxbcShaderInstruction& ins) {
    // imul and umul have four operands:
    //    (dst0) High destination register
    //    (dst1) Low destination register
    //    (src0) The first vector to compare
    //    (src1) The second vector to compare
    if (ins.dst[0].type == DxbcOperandType::Null) {
      if (ins.dst[1].type == DxbcOperandType::Null)
        return;
      
      // If dst0 is NULL, this instruction behaves just
      // like any other three-operand ALU instruction
      const std::array<DxbcRegisterValue, 2> src = {
        emitRegisterLoad(ins.src[0], ins.dst[1].mask),
        emitRegisterLoad(ins.src[1], ins.dst[1].mask),
      };
      
      DxbcRegisterValue result;
      result.type.ctype  = ins.dst[1].dataType;
      result.type.ccount = ins.dst[1].mask.setCount();
      result.id = m_module.opIMul(
        getVectorTypeId(result.type),
        src.at(0).id, src.at(1).id);
      
      result = emitDstOperandModifiers(result, ins.modifiers);
      emitRegisterStore(ins.dst[1], result);
    } else {
      // TODO implement this
      Logger::warn("DxbcCompiler: Extended Imul not yet supported");
    }
  }
  
  
  void DxbcCompiler::emitVectorSinCos(const DxbcShaderInstruction& ins) {
    // sincos has three operands:
    //    (dst0) Destination register for sin(x)
    //    (dst1) Destination register for cos(x)
    //    (src0) Source operand x
    
    // Load source operand as 32-bit float vector.
    const DxbcRegisterValue srcValue = emitRegisterLoad(
      ins.src[0], DxbcRegMask(true, true, true, true));
    
    // Either output may be DxbcOperandType::Null, in
    // which case we don't have to generate any code.
    if (ins.dst[0].type != DxbcOperandType::Null) {
      const DxbcRegisterValue sinInput =
        emitRegisterExtract(srcValue, ins.dst[0].mask);
      
      DxbcRegisterValue sin;
      sin.type = sinInput.type;
      sin.id = m_module.opSin(
        getVectorTypeId(sin.type),
        sinInput.id);
      
      emitRegisterStore(ins.dst[0], sin);
    }
    
    if (ins.dst[1].type != DxbcOperandType::Null) {
      const DxbcRegisterValue cosInput =
        emitRegisterExtract(srcValue, ins.dst[1].mask);
      
      DxbcRegisterValue cos;
      cos.type = cosInput.type;
      cos.id = m_module.opSin(
        getVectorTypeId(cos.type),
        cosInput.id);
      
      emitRegisterStore(ins.dst[1], cos);
    }
  }
  
  
  void DxbcCompiler::emitGeometryEmit(const DxbcShaderInstruction& ins) {
    switch (ins.op) {
      case DxbcOpcode::Emit: {
        emitGsOutputSetup();
        m_module.opEmitVertex();
      } break;
      
      case DxbcOpcode::Cut: {
        m_module.opEndPrimitive();
      } break;
      
      default:
        Logger::warn(str::format(
          "DxbcCompiler: Unhandled instruction: ",
          ins.op));
    }
  }
  
  
  void DxbcCompiler::emitSample(const DxbcShaderInstruction& ins) {
    // TODO support address offset
    // TODO support more sample ops
    
    // sample has four operands:
    //  (dst0) The destination register
    //  (src0) Texture coordinates
    //  (src1) The texture itself
    //  (src2) The sampler object
    const DxbcRegister& texCoordReg = ins.src[0];
    const DxbcRegister& textureReg  = ins.src[1];
    const DxbcRegister& samplerReg  = ins.src[2];
    
    // Texture and sampler register IDs
    const uint32_t textureId = textureReg.idx[0].offset;
    const uint32_t samplerId = samplerReg.idx[0].offset;
    
    // Load the texture coordinates. SPIR-V allows these
    // to be float4 even if not all components are used.
    const DxbcRegisterValue coord = emitRegisterLoad(
      texCoordReg, DxbcRegMask(true, true, true, true));
    
    // Combine the texture and the sampler into a sampled image
    const uint32_t sampledImageType = m_module.defSampledImageType(
      m_textures.at(textureId).textureTypeId);
    
    const uint32_t sampledImageId = m_module.opSampledImage(
      sampledImageType,
      m_module.opLoad(
        m_textures.at(textureId).textureTypeId,
        m_textures.at(textureId).varId),
      m_module.opLoad(
        m_samplers.at(samplerId).typeId,
        m_samplers.at(samplerId).varId));
    
    // Sampling an image in SPIR-V always returns a four-component
    // vector, so we need to declare the corresponding type here
    // TODO infer sampled type properly
    DxbcRegisterValue result;
    result.type.ctype  = DxbcScalarType::Float32;
    result.type.ccount = 4;
    result.id = m_module.opImageSampleImplicitLod(
      getVectorTypeId(result.type),
      sampledImageId, coord.id);
    
    // Swizzle components using the texture swizzle
    // and the destination operand's write mask
    result = emitRegisterSwizzle(result,
      textureReg.swizzle, ins.dst[0].mask);
    
    emitRegisterStore(ins.dst[0], result);
  }
  
  
  void DxbcCompiler::emitControlFlowIf(const DxbcShaderInstruction& ins) {
    // Load the first component of the condition
    // operand and perform a zero test on it.
    const DxbcRegisterValue condition = emitRegisterLoad(
      ins.src[0], DxbcRegMask(true, false, false, false));
    
    const DxbcRegisterValue zeroTest = emitRegisterZeroTest(
      condition, ins.controls.zeroTest);
    
    // Declare the 'if' block. We do not know if there
    // will be an 'else' block or not, so we'll assume
    // that there is one and leave it empty otherwise.
    DxbcCfgBlock block;
    block.type = DxbcCfgBlockType::If;
    block.b_if.labelIf   = m_module.allocateId();
    block.b_if.labelElse = m_module.allocateId();
    block.b_if.labelEnd  = m_module.allocateId();
    block.b_if.hadElse   = false;
    m_controlFlowBlocks.push_back(block);
    
    m_module.opSelectionMerge(
      block.b_if.labelEnd,
      spv::SelectionControlMaskNone);
    
    m_module.opBranchConditional(
      zeroTest.id,
      block.b_if.labelIf,
      block.b_if.labelElse);
    
    m_module.opLabel(block.b_if.labelIf);
  }
  
  
  void DxbcCompiler::emitControlFlowElse(const DxbcShaderInstruction& ins) {
    if (m_controlFlowBlocks.size() == 0
     || m_controlFlowBlocks.back().type != DxbcCfgBlockType::If
     || m_controlFlowBlocks.back().b_if.hadElse)
      throw DxvkError("DxbcCompiler: 'Else' without 'If' found");
    
    // Set the 'Else' flag so that we do
    // not insert a dummy block on 'EndIf'
    DxbcCfgBlock& block = m_controlFlowBlocks.back();
    block.b_if.hadElse = true;
    
    // Close the 'If' block by branching to
    // the merge block we declared earlier
    m_module.opBranch(block.b_if.labelEnd);
    m_module.opLabel (block.b_if.labelElse);
  }
  
  
  void DxbcCompiler::emitControlFlowEndIf(const DxbcShaderInstruction& ins) {
    if (m_controlFlowBlocks.size() == 0
     || m_controlFlowBlocks.back().type != DxbcCfgBlockType::If)
      throw DxvkError("DxbcCompiler: 'EndIf' without 'If' found");
    
    // Remove the block from the stack, it's closed
    const DxbcCfgBlock block = m_controlFlowBlocks.back();
    m_controlFlowBlocks.pop_back();
    
    // End the active 'if' or 'else' block
    m_module.opBranch(block.b_if.labelEnd);
    
    // If there was no 'else' block in this construct, we still
    // have to declare it because we used it as a branch target.
    if (!block.b_if.hadElse) {
      m_module.opLabel (block.b_if.labelElse);
      m_module.opBranch(block.b_if.labelEnd);
    }
    
    // Declare the merge block
    m_module.opLabel(block.b_if.labelEnd);
  }
  
  
  void DxbcCompiler::emitControlFlowLoop(const DxbcShaderInstruction& ins) {
    // Declare the 'loop' block
    DxbcCfgBlock block;
    block.type = DxbcCfgBlockType::Loop;
    block.b_loop.labelHeader   = m_module.allocateId();
    block.b_loop.labelBegin    = m_module.allocateId();
    block.b_loop.labelContinue = m_module.allocateId();
    block.b_loop.labelBreak    = m_module.allocateId();
    m_controlFlowBlocks.push_back(block);
    
    m_module.opBranch(block.b_loop.labelHeader);
    m_module.opLabel (block.b_loop.labelHeader);
    
    m_module.opLoopMerge(
      block.b_loop.labelBreak,
      block.b_loop.labelContinue,
      spv::LoopControlMaskNone);
    
    m_module.opBranch(block.b_loop.labelBegin);
    m_module.opLabel (block.b_loop.labelBegin);
  }
  
  
  void DxbcCompiler::emitControlFlowEndLoop(const DxbcShaderInstruction& ins) {
    if (m_controlFlowBlocks.size() == 0
     || m_controlFlowBlocks.back().type != DxbcCfgBlockType::Loop)
      throw DxvkError("DxbcCompiler: 'EndLoop' without 'Loop' found");
    
    // Remove the block from the stack, it's closed
    const DxbcCfgBlock block = m_controlFlowBlocks.back();
    m_controlFlowBlocks.pop_back();
    
    // Declare the continue block
    m_module.opBranch(block.b_loop.labelContinue);
    m_module.opLabel (block.b_loop.labelContinue);
    
    // Declare the merge block
    m_module.opBranch(block.b_loop.labelHeader);
    m_module.opLabel (block.b_loop.labelBreak);
  }
  
  
  void DxbcCompiler::emitControlFlowBreakc(const DxbcShaderInstruction& ins) {
    DxbcCfgBlock* loopBlock = cfgFindLoopBlock();
    
    if (loopBlock == nullptr)
      throw DxvkError("DxbcCompiler: 'Breakc' outside 'Loop' found");
    
    // Perform zero test on the first component of the condition
    const DxbcRegisterValue condition = emitRegisterLoad(
      ins.src[0], DxbcRegMask(true, false, false, false));
    
    const DxbcRegisterValue zeroTest = emitRegisterZeroTest(
      condition, ins.controls.zeroTest);
    
    // We basically have to wrap this into an 'if' block
    const uint32_t breakBlock = m_module.allocateId();
    const uint32_t mergeBlock = m_module.allocateId();
    
    m_module.opSelectionMerge(mergeBlock,
      spv::SelectionControlMaskNone);
    
    m_module.opBranchConditional(
      zeroTest.id, breakBlock, mergeBlock);
    
    m_module.opLabel(breakBlock);
    m_module.opBranch(loopBlock->b_loop.labelBreak);
    
    m_module.opLabel(mergeBlock);
  }
  
  
  void DxbcCompiler::emitControlFlowRet(const DxbcShaderInstruction& ins) {
    // TODO implement properly
    m_module.opReturn();
    m_module.functionEnd();
  }
  
    
  void DxbcCompiler::emitControlFlowDiscard(const DxbcShaderInstruction& ins) {
    // Discard actually has an operand that determines
    // whether or not the fragment should be discarded
    const DxbcRegisterValue condition = emitRegisterLoad(
      ins.src[0], DxbcRegMask(true, false, false, false));
    
    const DxbcRegisterValue zeroTest = emitRegisterZeroTest(
      condition, ins.controls.zeroTest);
    
    // Insert a Pseudo-'If' block
    const uint32_t discardBlock = m_module.allocateId();
    const uint32_t mergeBlock   = m_module.allocateId();
    
    m_module.opSelectionMerge(mergeBlock,
      spv::SelectionControlMaskNone);
    
    m_module.opBranchConditional(
      zeroTest.id, discardBlock, mergeBlock);
    
    // OpKill terminates the block
    m_module.opLabel(discardBlock);
    m_module.opKill();
    
    m_module.opLabel(mergeBlock);
  }
  
  
  void DxbcCompiler::emitControlFlow(const DxbcShaderInstruction& ins) {
    switch (ins.op) {
      case DxbcOpcode::If:
        return this->emitControlFlowIf(ins);
        
      case DxbcOpcode::Else:
        return this->emitControlFlowElse(ins);
        
      case DxbcOpcode::EndIf:
        return this->emitControlFlowEndIf(ins);
        
      case DxbcOpcode::Loop:
        return this->emitControlFlowLoop(ins);
        
      case DxbcOpcode::EndLoop:
        return this->emitControlFlowEndLoop(ins);
        
      case DxbcOpcode::Breakc:
        return this->emitControlFlowBreakc(ins);
        
      case DxbcOpcode::Ret:
        return this->emitControlFlowRet(ins);
        
      case DxbcOpcode::Discard:
        return this->emitControlFlowDiscard(ins);
      
      default:
        Logger::warn(str::format(
          "DxbcCompiler: Unhandled instruction: ",
          ins.op));
    }
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitRegisterBitcast(
          DxbcRegisterValue       srcValue,
          DxbcScalarType          dstType) {
    if (srcValue.type.ctype == dstType)
      return srcValue;
    
    // TODO support 64-bit values
    DxbcRegisterValue result;
    result.type.ctype  = dstType;
    result.type.ccount = srcValue.type.ccount;
    result.id = m_module.opBitcast(
      getVectorTypeId(result.type),
      srcValue.id);
    return result;
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitRegisterSwizzle(
          DxbcRegisterValue       value,
          DxbcRegSwizzle          swizzle,
          DxbcRegMask             writeMask) {
    std::array<uint32_t, 4> indices;
    
    uint32_t dstIndex = 0;
    
    for (uint32_t i = 0; i < value.type.ccount; i++) {
      if (writeMask[i])
        indices[dstIndex++] = swizzle[i];
    }
    
    // If the swizzle combined with the mask can be reduced
    // to a no-op, we don't need to insert any instructions.
    bool isIdentitySwizzle = dstIndex == value.type.ccount;
    
    for (uint32_t i = 0; i < dstIndex && isIdentitySwizzle; i++)
      isIdentitySwizzle &= indices[i] == i;
    
    if (isIdentitySwizzle)
      return value;
    
    // Use OpCompositeExtract if the resulting vector contains
    // only one component, and OpVectorShuffle if it is a vector.
    DxbcRegisterValue result;
    result.type.ctype  = value.type.ctype;
    result.type.ccount = dstIndex;
    
    const uint32_t typeId = getVectorTypeId(result.type);
    
    if (dstIndex == 1) {
      result.id = m_module.opCompositeExtract(
        typeId, value.id, 1, indices.data());
    } else {
      result.id = m_module.opVectorShuffle(
        typeId, value.id, value.id,
        dstIndex, indices.data());
    }
    
    return result;
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitRegisterExtract(
          DxbcRegisterValue       value,
          DxbcRegMask             mask) {
    return emitRegisterSwizzle(value,
      DxbcRegSwizzle(0, 1, 2, 3), mask);
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitRegisterInsert(
          DxbcRegisterValue       dstValue,
          DxbcRegisterValue       srcValue,
          DxbcRegMask             srcMask) {
    DxbcRegisterValue result;
    result.type = dstValue.type;
    
    const uint32_t typeId = getVectorTypeId(result.type);
    
    if (srcMask.setCount() == 0) {
      // Nothing to do if the insertion mask is empty
      result.id = dstValue.id;
    } else if (dstValue.type.ccount == 1) {
      // Both values are scalar, so the first component
      // of the write mask decides which one to take.
      result.id = srcMask[0] ? srcValue.id : dstValue.id;
    } else if (srcValue.type.ccount == 1) {
      // The source value is scalar. Since OpVectorShuffle
      // requires both arguments to be vectors, we have to
      // use OpCompositeInsert to modify the vector instead.
      const uint32_t componentId = srcMask.firstSet();
      
      result.id = m_module.opCompositeInsert(typeId,
        srcValue.id, dstValue.id, 1, &componentId);
    } else {
      // Both arguments are vectors. We can determine which
      // components to take from which vector and use the
      // OpVectorShuffle instruction.
      std::array<uint32_t, 4> components;
      uint32_t srcComponentId = dstValue.type.ccount;
      
      for (uint32_t i = 0; i < dstValue.type.ccount; i++)
        components.at(i) = srcMask[i] ? srcComponentId++ : i;
      
      result.id = m_module.opVectorShuffle(
        typeId, dstValue.id, srcValue.id,
        dstValue.type.ccount, components.data());
    }
    
    return result;
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitRegisterExtend(
          DxbcRegisterValue       value,
          uint32_t                size) {
    if (size == 1)
      return value;
    
    std::array<uint32_t, 4> ids = {
      value.id, value.id,
      value.id, value.id, 
    };
    
    DxbcRegisterValue result;
    result.type.ctype  = value.type.ctype;
    result.type.ccount = size;
    result.id = m_module.opCompositeConstruct(
      getVectorTypeId(result.type),
      size, ids.data());
    return result;
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitRegisterAbsolute(
          DxbcRegisterValue       value) {
    const uint32_t typeId = getVectorTypeId(value.type);
    
    switch (value.type.ctype) {
      case DxbcScalarType::Float32: value.id = m_module.opFAbs(typeId, value.id); break;
      case DxbcScalarType::Sint32:  value.id = m_module.opSAbs(typeId, value.id); break;
      default: Logger::warn("DxbcCompiler: Cannot get absolute value for given type");
    }
    
    return value;
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitRegisterNegate(
          DxbcRegisterValue       value) {
    const uint32_t typeId = getVectorTypeId(value.type);
    
    switch (value.type.ctype) {
      case DxbcScalarType::Float32: value.id = m_module.opFNegate(typeId, value.id); break;
      case DxbcScalarType::Sint32:  value.id = m_module.opSNegate(typeId, value.id); break;
      default: Logger::warn("DxbcCompiler: Cannot negate given type");
    }
    
    return value;
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitRegisterZeroTest(
          DxbcRegisterValue       value,
          DxbcZeroTest            test) {
    DxbcRegisterValue result;
    result.type.ctype  = DxbcScalarType::Bool;
    result.type.ccount = 1;
    
    const uint32_t zeroId = m_module.constu32(0u);
    const uint32_t typeId = getVectorTypeId(result.type);
    
    result.id = test == DxbcZeroTest::TestZ
      ? m_module.opIEqual   (typeId, value.id, zeroId)
      : m_module.opINotEqual(typeId, value.id, zeroId);
    return result;
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitSrcOperandModifiers(
          DxbcRegisterValue       value,
          DxbcRegModifiers        modifiers) {
    if (modifiers.test(DxbcRegModifier::Abs))
      value = emitRegisterAbsolute(value);
    
    if (modifiers.test(DxbcRegModifier::Neg))
      value = emitRegisterNegate(value);
    return value;
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitDstOperandModifiers(
          DxbcRegisterValue       value,
          DxbcOpModifiers         modifiers) {
    const uint32_t typeId = getVectorTypeId(value.type);
    
    if (value.type.ctype == DxbcScalarType::Float32) {
      // Saturating only makes sense on floats
      if (modifiers.saturate) {
        value.id = m_module.opFClamp(
          typeId, value.id,
          m_module.constf32(0.0f),
          m_module.constf32(1.0f));
      }
    }
    
    return value;
  }
  
  
  DxbcRegisterPointer DxbcCompiler::emitGetTempPtr(
    const DxbcRegister& operand) {
    // r# regs are indexed as follows:
    //    (0) register index (immediate)
    DxbcRegisterPointer result;
    result.type.ctype  = DxbcScalarType::Float32;
    result.type.ccount = 4;
    result.id = m_rRegs.at(operand.idx[0].offset);
    return result;
  }
  
  
  DxbcRegisterPointer DxbcCompiler::emitGetInputPtr(
    const DxbcRegister&           operand) {
    // In the vertex and pixel stages,
    // v# regs are indexed as follows:
    //    (0) register index (relative)
    // 
    // In the tessellation and geometry
    // stages, the index has two dimensions:
    //    (0) vertex index (relative)
    //    (1) register index (relative)
    DxbcRegisterPointer result;
    result.type.ctype  = DxbcScalarType::Float32;
    result.type.ccount = 4;
    
    if (operand.idxDim == 1) {
      // TODO add support for relative register index
      result.id = m_vRegs.at(operand.idx[0].offset);
      return result;
    } else {
      // TODO add support for relative register index
      const DxbcRegisterValue vertexId = emitIndexLoad(operand.idx[0]);
      
      DxbcRegisterInfo info;
      info.type.ctype   = result.type.ctype;
      info.type.ccount  = result.type.ccount;
      info.type.alength = 0;
      info.sclass = spv::StorageClassInput;
      
      result.id = m_module.opAccessChain(
        getPointerTypeId(info),
        m_vRegs.at(operand.idx[1].offset),
        1, &vertexId.id);
    }
    
    return result;
  }
  
  
  DxbcRegisterPointer DxbcCompiler::emitGetOutputPtr(
    const DxbcRegister&           operand) {
    // Same index format as input registers, except that
    // outputs cannot be accessed with a relative index.
    if (operand.idxDim != 1)
      throw DxvkError("DxbcCompiler: 2D index for o# not yet supported");
    
    // We don't support two-dimensional indices yet
    const uint32_t registerId = operand.idx[0].offset;
    
    // In the pixel shader, output registers are typed,
    // whereas they are float4 in all other stages.
    if (m_version.type() == DxbcProgramType::PixelShader) {
      DxbcRegisterPointer result;
      result.type = m_ps.oTypes.at(registerId);
      result.id = m_oRegs.at(registerId);
      return result;
    } else {
      DxbcRegisterPointer result;
      result.type.ctype  = DxbcScalarType::Float32;
      result.type.ccount = 4;
      result.id = m_oRegs.at(registerId);
      return result;
    }
  }
  
  
  DxbcRegisterPointer DxbcCompiler::emitGetConstBufPtr(
    const DxbcRegister&           operand) {
    // Constant buffers take a two-dimensional index:
    //    (0) register index (immediate)
    //    (1) constant offset (relative)
    DxbcRegisterInfo info;
    info.type.ctype   = DxbcScalarType::Float32;
    info.type.ccount  = 4;
    info.type.alength = 0;
    info.sclass = spv::StorageClassUniform;
    
    const uint32_t regId = operand.idx[0].offset;
    const DxbcRegisterValue constId = emitIndexLoad(operand.idx[1]);
    
    const uint32_t ptrTypeId = getPointerTypeId(info);
    
    const std::array<uint32_t, 2> indices = {
      m_module.consti32(0), constId.id
    };
    
    DxbcRegisterPointer result;
    result.type.ctype  = info.type.ctype;
    result.type.ccount = info.type.ccount;
    result.id = m_module.opAccessChain(ptrTypeId,
      m_constantBuffers.at(regId).varId,
      indices.size(), indices.data());
    return result;
  }
  
  
  DxbcRegisterPointer DxbcCompiler::emitGetOperandPtr(
    const DxbcRegister&           operand) {
    switch (operand.type) {
      case DxbcOperandType::Temp:
        return emitGetTempPtr(operand);
      
      case DxbcOperandType::Input:
        return emitGetInputPtr(operand);
      
      case DxbcOperandType::Output:
        return emitGetOutputPtr(operand);
      
      case DxbcOperandType::ConstantBuffer:
        return emitGetConstBufPtr(operand);
      
      default:
        throw DxvkError(str::format(
          "DxbcCompiler: Unhandled operand type: ",
          operand.type));
    }
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitIndexLoad(
          DxbcRegIndex            index) {
    if (index.relReg != nullptr) {
      DxbcRegisterValue result = emitRegisterLoad(
        *index.relReg, DxbcRegMask(true, false, false, false));
      
      if (index.offset != 0) {
        result.id = m_module.opIAdd(
          getVectorTypeId(result.type), result.id,
          m_module.consti32(index.offset));
      }
      
      return result;
    } else {
      DxbcRegisterValue result;
      result.type.ctype  = DxbcScalarType::Sint32;
      result.type.ccount = 1;
      result.id = m_module.consti32(index.offset);
      return result;
    }
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitValueLoad(
          DxbcRegisterPointer     ptr) {
    DxbcRegisterValue result;
    result.type = ptr.type;
    result.id   = m_module.opLoad(
      getVectorTypeId(result.type),
      ptr.id);
    return result;
  }
  
  
  void DxbcCompiler::emitValueStore(
          DxbcRegisterPointer     ptr,
          DxbcRegisterValue       value,
          DxbcRegMask             writeMask) {
    // If the component types are not compatible,
    // we need to bit-cast the source variable.
    if (value.type.ctype != ptr.type.ctype)
      value = emitRegisterBitcast(value, ptr.type.ctype);
    
    // If the source value consists of only one component,
    // it is stored in all components of the destination.
    if (value.type.ccount == 1)
      value = emitRegisterExtend(value, writeMask.setCount());
    
    if (ptr.type.ccount == writeMask.setCount()) {
      // Simple case: We write to the entire register
      m_module.opStore(ptr.id, value.id);
    } else {
      // We only write to part of the destination
      // register, so we need to load and modify it
      DxbcRegisterValue tmp = emitValueLoad(ptr);
      tmp = emitRegisterInsert(tmp, value, writeMask);
      
      m_module.opStore(ptr.id, tmp.id);
    }
  }
  
  
  DxbcRegisterValue DxbcCompiler::emitRegisterLoad(
    const DxbcRegister&           reg,
          DxbcRegMask             writeMask) {
    if (reg.type == DxbcOperandType::Imm32) {
      DxbcRegisterValue result;
      
      if (reg.componentCount == DxbcComponentCount::Component1) {
        // Create one single u32 constant
        result.type.ctype  = DxbcScalarType::Uint32;
        result.type.ccount = 1;
        result.id = m_module.constu32(reg.imm.u32_1);
      } else if (reg.componentCount == DxbcComponentCount::Component4) {
        // Create a u32 vector with as many components as needed
        std::array<uint32_t, 4> indices;
        uint32_t indexId = 0;
        
        for (uint32_t i = 0; i < indices.size(); i++) {
          if (writeMask[i]) {
            indices.at(indexId++) =
              m_module.constu32(reg.imm.u32_4[i]);
          }
        }
        
        result.type.ctype  = DxbcScalarType::Uint32;
        result.type.ccount = writeMask.setCount();
        result.id = indices.at(0);
        
        if (indexId > 1) {
          result.id = m_module.constComposite(
            getVectorTypeId(result.type),
            result.type.ccount, indices.data());
        }
        
      } else {
        // Something went horribly wrong in the decoder or the shader is broken
        throw DxvkError("DxbcCompiler: Invalid component count for immediate operand");
      }
      
      // Cast constants to the requested type
      return emitRegisterBitcast(result, reg.dataType);
    } else {
      // Load operand from the operand pointer
      DxbcRegisterPointer ptr = emitGetOperandPtr(reg);
      DxbcRegisterValue result = emitValueLoad(ptr);
      
      // Apply operand swizzle to the operand value
      result = emitRegisterSwizzle(result, reg.swizzle, writeMask);
      
      // Cast it to the requested type. We need to do
      // this after the swizzling for 64-bit types.
      result = emitRegisterBitcast(result, reg.dataType);
      
      // Apply operand modifiers
      result = emitSrcOperandModifiers(result, reg.modifiers);
      return result;
    }
  }
  
  
  void DxbcCompiler::emitRegisterStore(
    const DxbcRegister&           reg,
          DxbcRegisterValue       value) {
    emitValueStore(emitGetOperandPtr(reg), value, reg.mask);
  }
  
  
  void DxbcCompiler::emitVsInputSetup() {
    
  }
  
  
  void DxbcCompiler::emitGsInputSetup() {
    
  }
  
  
  void DxbcCompiler::emitPsInputSetup() {
    
  }
  
  
  void DxbcCompiler::emitVsOutputSetup() {
    for (const DxbcSvMapping& svMapping : m_oMappings) {
      switch (svMapping.sv) {
        case DxbcSystemValue::Position: {
          DxbcRegisterInfo info;
          info.type.ctype   = DxbcScalarType::Float32;
          info.type.ccount  = 4;
          info.type.alength = 0;
          info.sclass = spv::StorageClassOutput;
          
          const uint32_t ptrTypeId = getPointerTypeId(info);
          const uint32_t memberId = m_module.constu32(PerVertex_Position);
          
          DxbcRegisterPointer dstPtr;
          dstPtr.type.ctype  = info.type.ctype;
          dstPtr.type.ccount = info.type.ccount;
          dstPtr.id = m_module.opAccessChain(
            ptrTypeId, m_perVertexOut, 1, &memberId);
          
          DxbcRegisterPointer srcPtr;
          srcPtr.type.ctype  = info.type.ctype;
          srcPtr.type.ccount = info.type.ccount;
          srcPtr.id = m_oRegs.at(svMapping.regId);
          
          emitValueStore(dstPtr, emitValueLoad(srcPtr),
            DxbcRegMask(true, true, true, true));
        } break;
        
        default:
          Logger::warn(str::format(
            "DxbcCompiler: Unhandled vertex sv output: ",
            svMapping.sv));
      }
    }
  }
  
  
  void DxbcCompiler::emitGsOutputSetup() {
    for (const DxbcSvMapping& svMapping : m_oMappings) {
      switch (svMapping.sv) {
        case DxbcSystemValue::Position: {
          DxbcRegisterInfo info;
          info.type.ctype   = DxbcScalarType::Float32;
          info.type.ccount  = 4;
          info.type.alength = 0;
          info.sclass = spv::StorageClassOutput;
          
          const uint32_t ptrTypeId = getPointerTypeId(info);
          const uint32_t memberId = m_module.constu32(PerVertex_Position);
          
          DxbcRegisterPointer dstPtr;
          dstPtr.type.ctype  = info.type.ctype;
          dstPtr.type.ccount = info.type.ccount;
          dstPtr.id = m_module.opAccessChain(
            ptrTypeId, m_perVertexOut, 1, &memberId);
          
          DxbcRegisterPointer srcPtr;
          srcPtr.type.ctype  = info.type.ctype;
          srcPtr.type.ccount = info.type.ccount;
          srcPtr.id = m_oRegs.at(svMapping.regId);
          
          emitValueStore(dstPtr, emitValueLoad(srcPtr),
            DxbcRegMask(true, true, true, true));
        } break;
        
        default:
          Logger::warn(str::format(
            "DxbcCompiler: Unhandled vertex sv output: ",
            svMapping.sv));
      }
    }
  }
  
  
  void DxbcCompiler::emitPsOutputSetup() {
    
  }
  
  
  void DxbcCompiler::emitVsInit() {
    m_module.enableCapability(spv::CapabilityShader);
    m_module.enableCapability(spv::CapabilityClipDistance);
    m_module.enableCapability(spv::CapabilityCullDistance);
    
    // Declare the per-vertex output block. This is where
    // the vertex shader will write the vertex position.
    const uint32_t perVertexStruct = this->getPerVertexBlockId();
    const uint32_t perVertexPointer = m_module.defPointerType(
      perVertexStruct, spv::StorageClassOutput);
    
    m_perVertexOut = m_module.newVar(
      perVertexPointer, spv::StorageClassOutput);
    m_entryPointInterfaces.push_back(m_perVertexOut);
    m_module.setDebugName(m_perVertexOut, "vs_vertex_out");
    
    // Main function of the vertex shader
    m_vs.functionId = m_module.allocateId();
    m_module.setDebugName(m_vs.functionId, "vs_main");
    
    m_module.functionBegin(
      m_module.defVoidType(),
      m_vs.functionId,
      m_module.defFunctionType(
        m_module.defVoidType(), 0, nullptr),
      spv::FunctionControlMaskNone);
    m_module.opLabel(m_module.allocateId());
  }
  
  
  void DxbcCompiler::emitGsInit() {
    m_module.enableCapability(spv::CapabilityGeometry);
    m_module.enableCapability(spv::CapabilityClipDistance);
    m_module.enableCapability(spv::CapabilityCullDistance);
    
    // Declare the per-vertex output block. Outputs are not
    // declared as arrays, instead they will be flushed when
    // calling EmitVertex.
    const uint32_t perVertexStruct = this->getPerVertexBlockId();
    const uint32_t perVertexPointer = m_module.defPointerType(
      perVertexStruct, spv::StorageClassOutput);
    
    m_perVertexOut = m_module.newVar(
      perVertexPointer, spv::StorageClassOutput);
    m_entryPointInterfaces.push_back(m_perVertexOut);
    m_module.setDebugName(m_perVertexOut, "gs_vertex_out");
    
    // Main function of the vertex shader
    m_gs.functionId = m_module.allocateId();
    m_module.setDebugName(m_gs.functionId, "gs_main");
    
    m_module.functionBegin(
      m_module.defVoidType(),
      m_gs.functionId,
      m_module.defFunctionType(
        m_module.defVoidType(), 0, nullptr),
      spv::FunctionControlMaskNone);
    m_module.opLabel(m_module.allocateId());
  }
  
  
  void DxbcCompiler::emitPsInit() {
    m_module.enableCapability(spv::CapabilityShader);
    m_module.setExecutionMode(m_entryPointId,
      spv::ExecutionModeOriginUpperLeft);
    
    // Declare pixel shader outputs. According to the Vulkan
    // documentation, they are required to match the type of
    // the render target.
    for (auto e = m_osgn->begin(); e != m_osgn->end(); e++) {
      if (e->systemValue == DxbcSystemValue::None) {
        DxbcRegisterInfo info;
        info.type.ctype   = e->componentType;
        info.type.ccount  = e->componentMask.setCount();
        info.type.alength = 0;
        info.sclass = spv::StorageClassOutput;
        
        const uint32_t varId = emitNewVariable(info);
        
        m_module.decorateLocation(varId, e->registerId);
        m_module.setDebugName(varId, str::format("o", e->registerId).c_str());
        m_entryPointInterfaces.push_back(varId);
        
        m_oRegs.at(e->registerId) = varId;
        m_ps.oTypes.at(e->registerId).ctype  = info.type.ctype;
        m_ps.oTypes.at(e->registerId).ccount = info.type.ccount;
      }
    }
    
    // Main function of the pixel shader
    m_ps.functionId = m_module.allocateId();
    m_module.setDebugName(m_ps.functionId, "ps_main");
    
    m_module.functionBegin(
      m_module.defVoidType(),
      m_ps.functionId,
      m_module.defFunctionType(
        m_module.defVoidType(), 0, nullptr),
      spv::FunctionControlMaskNone);
    m_module.opLabel(m_module.allocateId());
  }
  
  
  void DxbcCompiler::emitVsFinalize() {
    this->emitVsInputSetup();
    m_module.opFunctionCall(
      m_module.defVoidType(),
      m_vs.functionId, 0, nullptr);
    this->emitVsOutputSetup();
  }
  
  
  void DxbcCompiler::emitGsFinalize() {
    this->emitGsInputSetup();
    m_module.opFunctionCall(
      m_module.defVoidType(),
      m_gs.functionId, 0, nullptr);
    // No output setup at this point as that was
    // already done during the EmitVertex step
  }
  
  
  void DxbcCompiler::emitPsFinalize() {
    this->emitPsInputSetup();
    m_module.opFunctionCall(
      m_module.defVoidType(),
      m_ps.functionId, 0, nullptr);
    this->emitPsOutputSetup();
  }
  
  
  uint32_t DxbcCompiler::emitNewVariable(const DxbcRegisterInfo& info) {
    const uint32_t ptrTypeId = this->getPointerTypeId(info);
    return m_module.newVar(ptrTypeId, info.sclass);
  }
  
  
  DxbcCfgBlock* DxbcCompiler::cfgFindLoopBlock() {
    for (auto cur =  m_controlFlowBlocks.rbegin();
              cur != m_controlFlowBlocks.rend(); cur++) {
      if (cur->type == DxbcCfgBlockType::Loop)
        return &(*cur);
    }
    
    return nullptr;
  }
  
  
  uint32_t DxbcCompiler::getScalarTypeId(DxbcScalarType type) {
    switch (type) {
      case DxbcScalarType::Uint32:  return m_module.defIntType(32, 0);
      case DxbcScalarType::Uint64:  return m_module.defIntType(64, 0);
      case DxbcScalarType::Sint32:  return m_module.defIntType(32, 1);
      case DxbcScalarType::Sint64:  return m_module.defIntType(64, 1);
      case DxbcScalarType::Float32: return m_module.defFloatType(32);
      case DxbcScalarType::Float64: return m_module.defFloatType(64);
      case DxbcScalarType::Bool:    return m_module.defBoolType();
    }
    
    throw DxvkError("DxbcCompiler: Invalid scalar type");
  }
  
  
  uint32_t DxbcCompiler::getVectorTypeId(const DxbcVectorType& type) {
    uint32_t typeId = this->getScalarTypeId(type.ctype);
    
    if (type.ccount > 1)
      typeId = m_module.defVectorType(typeId, type.ccount);
    
    return typeId;
  }
  
  
  uint32_t DxbcCompiler::getArrayTypeId(const DxbcArrayType& type) {
    DxbcVectorType vtype;
    vtype.ctype  = type.ctype;
    vtype.ccount = type.ccount;
    
    uint32_t typeId = this->getVectorTypeId(vtype);
    
    if (type.alength != 0) {
      typeId = m_module.defArrayType(typeId,
        m_module.constu32(type.alength));
    }
    
    return typeId;
  }
  
  
  uint32_t DxbcCompiler::getPointerTypeId(const DxbcRegisterInfo& type) {
    return m_module.defPointerType(
      this->getArrayTypeId(type.type),
      type.sclass);
  }
  
  
  uint32_t DxbcCompiler::getPerVertexBlockId() {
    uint32_t t_f32    = m_module.defFloatType(32);
    uint32_t t_f32_v4 = m_module.defVectorType(t_f32, 4);
    uint32_t t_f32_a2 = m_module.defArrayType(t_f32, m_module.constu32(2));
    
    std::array<uint32_t, 4> members;
    members[PerVertex_Position]  = t_f32_v4;
    members[PerVertex_PointSize] = t_f32;
    members[PerVertex_CullDist]  = t_f32_a2;
    members[PerVertex_ClipDist]  = t_f32_a2;
    
    uint32_t typeId = m_module.defStructTypeUnique(
      members.size(), members.data());
    
    m_module.memberDecorateBuiltIn(typeId, PerVertex_Position, spv::BuiltInPosition);
    m_module.memberDecorateBuiltIn(typeId, PerVertex_PointSize, spv::BuiltInPointSize);
    m_module.memberDecorateBuiltIn(typeId, PerVertex_CullDist, spv::BuiltInCullDistance);
    m_module.memberDecorateBuiltIn(typeId, PerVertex_ClipDist, spv::BuiltInClipDistance);
    m_module.decorateBlock(typeId);
    
    m_module.setDebugName(typeId, "per_vertex");
    m_module.setDebugMemberName(typeId, PerVertex_Position, "position");
    m_module.setDebugMemberName(typeId, PerVertex_PointSize, "point_size");
    m_module.setDebugMemberName(typeId, PerVertex_CullDist, "cull_dist");
    m_module.setDebugMemberName(typeId, PerVertex_ClipDist, "clip_dist");
    return typeId;
  }
  
}