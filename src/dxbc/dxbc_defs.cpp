#include "dxbc_defs.h"

namespace dxvk {
  
  const std::array<DxbcInstFormat, 207> g_instructionFormats = {{
    /* Add                                  */
    { 3, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* And                                  */
    { },
    /* Break                                */
    { 0, DxbcInstClass::ControlFlow },
    /* Breakc                               */
    { 1, DxbcInstClass::ControlFlow, {
      { DxbcOperandKind::SrcReg, DxbcScalarType::Uint32 },
    } },
    /* Call                                 */
    { 1, DxbcInstClass::ControlFlow, {
      { DxbcOperandKind::SrcReg, DxbcScalarType::Uint32 },
    } },
    /* Callc                                */
    { 2, DxbcInstClass::ControlFlow, {
      { DxbcOperandKind::SrcReg, DxbcScalarType::Uint32 },
    } },
    /* Case                                 */
    { },
    /* Continue                             */
    { 0, DxbcInstClass::ControlFlow },
    /* Continuec                            */
    { 1, DxbcInstClass::ControlFlow, {
      { DxbcOperandKind::SrcReg, DxbcScalarType::Uint32 },
    } },
    /* Cut                                  */
    { 0, DxbcInstClass::GeometryEmit },
    /* Default                              */
    { },
    /* DerivRtx                             */
    { },
    /* DerivRty                             */
    { },
    /* Discard                              */
    { 1, DxbcInstClass::ControlFlow, {
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Div                                  */
    { 3, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Dp2                                  */
    { 3, DxbcInstClass::VectorDot, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Dp3                                  */
    { 3, DxbcInstClass::VectorDot, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Dp4                                  */
    { 3, DxbcInstClass::VectorDot, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Else                                 */
    { 0, DxbcInstClass::ControlFlow },
    /* Emit                                 */
    { 0, DxbcInstClass::GeometryEmit },
    /* EmitThenCut                          */
    { },
    /* EndIf                                */
    { 0, DxbcInstClass::ControlFlow },
    /* EndLoop                              */
    { 0, DxbcInstClass::ControlFlow },
    /* EndSwitch                            */
    { },
    /* Eq                                   */
    { 3, DxbcInstClass::VectorCmp, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Uint32  },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Exp                                  */
    { 2, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Frc                                  */
    { },
    /* FtoI                                 */
    { },
    /* FtoU                                 */
    { },
    /* Ge                                   */
    { 3, DxbcInstClass::VectorCmp, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Uint32  },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* IAdd                                 */
    { 3, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
    } },
    /* If                                   */
    { 1, DxbcInstClass::ControlFlow, {
      { DxbcOperandKind::SrcReg, DxbcScalarType::Uint32 },
    } },
    /* IEq                                  */
    { 3, DxbcInstClass::VectorCmp, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
    } },
    /* IGe                                  */
    { 3, DxbcInstClass::VectorCmp, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
    } },
    /* ILt                                  */
    { 3, DxbcInstClass::VectorCmp, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
    } },
    /* IMad                                 */
    { 4, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
    } },
    /* IMax                                 */
    { 3, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
    } },
    /* IMin                                 */
    { 3, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
    } },
    /* IMul                                 */
    { 4, DxbcInstClass::VectorImul, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* INe                                  */
    { 3, DxbcInstClass::VectorCmp, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
    } },
    /* INeg                                 */
    { 2, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Sint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Sint32 },
    } },
    /* IShl                                 */
    { },
    /* IShr                                 */
    { },
    /* ItoF                                 */
    { },
    /* Label                                */
    { },
    /* Ld                                   */
    { },
    /* LdMs                                 */
    { },
    /* Log                                  */
    { 2, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Loop                                 */
    { 0, DxbcInstClass::ControlFlow },
    /* Lt                                   */
    { 3, DxbcInstClass::VectorCmp, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Uint32  },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Mad                                  */
    { 4, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Min                                  */
    { 3, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Max                                  */
    { 3, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* CustomData                           */
    { },
    /* Mov                                  */
    { 2, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Movc                                 */
    { 4, DxbcInstClass::VectorCmov, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Uint32  },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Mul                                  */
    { 3, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Ne                                   */
    { 3, DxbcInstClass::VectorCmp, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Uint32  },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Nop                                  */
    { },
    /* Not                                  */
    { },
    /* Or                                   */
    { },
    /* ResInfo                              */
    { },
    /* Ret                                  */
    { 0, DxbcInstClass::ControlFlow },
    /* Retc                                 */
    { 1, DxbcInstClass::ControlFlow, {
      { DxbcOperandKind::SrcReg, DxbcScalarType::Uint32 },
    } },
    /* RoundNe                              */
    { },
    /* RoundNi                              */
    { },
    /* RoundPi                              */
    { },
    /* RoundZ                               */
    { },
    /* Rsq                                  */
    { 2, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Sample                               */
    { 4, DxbcInstClass::TextureSample, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* SampleC                              */
    { },
    /* SampleClz                            */
    { },
    /* SampleL                              */
    { },
    /* SampleD                              */
    { },
    /* SampleB                              */
    { },
    /* Sqrt                                 */
    { 2, DxbcInstClass::VectorAlu, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* Switch                               */
    { },
    /* SinCos                               */
    { 3, DxbcInstClass::VectorSinCos, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* UDiv                                 */
    { 4, DxbcInstClass::VectorIdiv, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Uint32 },
      { DxbcOperandKind::DstReg, DxbcScalarType::Uint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Uint32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Uint32 },
    } },
    /* ULt                                  */
    { },
    /* UGe                                  */
    { },
    /* UMul                                 */
    { 4, DxbcInstClass::VectorImul, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::SrcReg, DxbcScalarType::Float32 },
    } },
    /* UMad                                 */
    { },
    /* UMax                                 */
    { },
    /* UMin                                 */
    { },
    /* UShr                                 */
    { },
    /* UtoF                                 */
    { },
    /* Xor                                  */
    { },
    /* DclResource                          */
    { 2, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::Imm32,  DxbcScalarType::Uint32  },
    } },
    /* DclConstantBuffer                    */
    { 1, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
    } },
    /* DclSampler                           */
    { 1, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
    } },
    /* DclIndexRange                        */
    { },
    /* DclGsOutputPrimitiveTopology         */
    { 0, DxbcInstClass::Declaration },
    /* DclGsInputPrimitive                  */
    { 0, DxbcInstClass::Declaration },
    /* DclMaxOutputVertexCount              */
    { 1, DxbcInstClass::Declaration, {
      { DxbcOperandKind::Imm32,  DxbcScalarType::Uint32 },
    } },
    /* DclInput                             */
    { 1, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
    } },
    /* DclInputSgv                          */
    { 2, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::Imm32,  DxbcScalarType::Uint32  },
    } },
    /* DclInputSiv                          */
    { 2, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::Imm32,  DxbcScalarType::Uint32  },
    } },
    /* DclInputPs                           */
    { 1, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
    } },
    /* DclInputPsSgv                        */
    { 2, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::Imm32,  DxbcScalarType::Uint32  },
    } },
    /* DclInputPsSiv                        */
    { 2, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::Imm32,  DxbcScalarType::Uint32  },
    } },
    /* DclOutput                            */
    { 1, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
    } },
    /* DclOutputSgv                         */
    { 2, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::Imm32,  DxbcScalarType::Uint32  },
    } },
    /* DclOutputSiv                         */
    { 2, DxbcInstClass::Declaration, {
      { DxbcOperandKind::DstReg, DxbcScalarType::Float32 },
      { DxbcOperandKind::Imm32,  DxbcScalarType::Uint32  },
    } },
    /* DclTemps                             */
    { 1, DxbcInstClass::Declaration, {
      { DxbcOperandKind::Imm32, DxbcScalarType::Uint32 },
    } },
    /* DclIndexableTemp                     */
    { },
    /* DclGlobalFlags                       */
    { 0, DxbcInstClass::Declaration },
    /* Reserved0                            */
    { 0, DxbcInstClass::Undefined },
    /* Lod                                  */
    { },
    /* Gather4                              */
    { },
    /* SamplePos                            */
    { },
    /* SampleInfo                           */
    { },
    /* Reserved1                            */
    { },
    /* HsDecls                              */
    { },
    /* HsControlPointPhase                  */
    { },
    /* HsForkPhase                          */
    { },
    /* HsJoinPhase                          */
    { },
    /* EmitStream                           */
    { },
    /* CutStream                            */
    { },
    /* EmitThenCutStream                    */
    { },
    /* InterfaceCall                        */
    { },
    /* BufInfo                              */
    { },
    /* DerivRtxCoarse                       */
    { },
    /* DerivRtxFine                         */
    { },
    /* DerivRtyCoarse                       */
    { },
    /* DerivRtyFine                         */
    { },
    /* Gather4C                             */
    { },
    /* Gather4Po                            */
    { },
    /* Gather4PoC                           */
    { },
    /* Rcp                                  */
    { },
    /* F32toF16                             */
    { },
    /* F16toF32                             */
    { },
    /* UAddc                                */
    { },
    /* USubb                                */
    { },
    /* CountBits                            */
    { },
    /* FirstBitHi                           */
    { },
    /* FirstBitLo                           */
    { },
    /* FirstBitShi                          */
    { },
    /* UBfe                                 */
    { },
    /* IBfe                                 */
    { },
    /* Bfi                                  */
    { },
    /* BfRev                                */
    { },
    /* Swapc                                */
    { },
    /* DclStream                            */
    { },
    /* DclFunctionBody                      */
    { },
    /* DclFunctionTable                     */
    { },
    /* DclInterface                         */
    { },
    /* DclInputControlPointCount            */
    { },
    /* DclOutputControlPointCount           */
    { },
    /* DclTessDomain                        */
    { },
    /* DclTessPartitioning                  */
    { },
    /* DclTessOutputPrimitive               */
    { },
    /* DclHsMaxTessFactor                   */
    { },
    /* DclHsForkPhaseInstanceCount          */
    { },
    /* DclHsJoinPhaseInstanceCount          */
    { },
    /* DclThreadGroup                       */
    { },
    /* DclUavTyped                          */
    { },
    /* DclUavRaw                            */
    { },
    /* DclUavStructured                     */
    { },
    /* DclThreadGroupSharedMemoryRaw        */
    { },
    /* DclThreadGroupSharedMemoryStructured */
    { },
    /* DclResourceRaw                       */
    { },
    /* DclResourceStructured                */
    { },
    /* LdUavTyped                           */
    { },
    /* StoreUavTyped                        */
    { },
    /* LdRaw                                */
    { },
    /* StoreRaw                             */
    { },
    /* LdStructured                         */
    { },
    /* StoreStructured                      */
    { },
    /* AtomicAnd                            */
    { },
    /* AtomicOr                             */
    { },
    /* AtomicXor                            */
    { },
    /* AtomicCmpStore                       */
    { },
    /* AtomicIAdd                           */
    { },
    /* AtomicIMax                           */
    { },
    /* AtomicIMin                           */
    { },
    /* AtomicUMax                           */
    { },
    /* AtomicUMin                           */
    { },
    /* ImmAtomicAlloc                       */
    { },
    /* ImmAtomicConsume                     */
    { },
    /* ImmAtomicIAdd                        */
    { },
    /* ImmAtomicAnd                         */
    { },
    /* ImmAtomicOr                          */
    { },
    /* ImmAtomicXor                         */
    { },
    /* ImmAtomicExch                        */
    { },
    /* ImmAtomicCmpExch                     */
    { },
    /* ImmAtomicImax                        */
    { },
    /* ImmAtomicImin                        */
    { },
    /* ImmAtomicUmax                        */
    { },
    /* ImmAtomicUmin                        */
    { },
    /* Sync                                 */
    { },
    /* DAdd                                 */
    { },
    /* DMax                                 */
    { },
    /* DMin                                 */
    { },
    /* DMul                                 */
    { },
    /* DEq                                  */
    { },
    /* DGe                                  */
    { },
    /* DLt                                  */
    { },
    /* DNe                                  */
    { },
    /* DMov                                 */
    { },
    /* DMovc                                */
    { },
    /* DtoF                                 */
    { },
    /* FtoD                                 */
    { },
    /* EvalSnapped                          */
    { },
    /* EvalSampleIndex                      */
    { },
    /* EvalCentroid                         */
    { },
    /* DclGsInstanceCount                   */
    { },
  }};
  
  
  DxbcInstFormat dxbcInstructionFormat(DxbcOpcode opcode) {
    const uint32_t idx = static_cast<uint32_t>(opcode);
    
    return (idx < g_instructionFormats.size())
      ? g_instructionFormats.at(idx)
      : DxbcInstFormat();
  }
  
}