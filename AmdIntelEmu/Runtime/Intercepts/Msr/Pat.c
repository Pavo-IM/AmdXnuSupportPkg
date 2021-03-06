#include <Base.h>

#include <Register/ArchitecturalMsr.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#include "../../AmdIntelEmuRuntime.h"

VOID
AmdIntelEmuInternalRdmsrPat (
  IN OUT AMD_VMCB_SAVE_STATE_AREA_NON_ES  *SaveState,
  IN OUT AMD_INTEL_EMU_REGISTERS          *Registers,
  OUT    AMD_VMCB_CLEAN_FIELD             *CleanField
  )
{
  UINT64 Value;

  ASSERT (SaveState != NULL);
  ASSERT (Registers != NULL);

  if (mAmdIntelEmuInternalNp) {
    Value = SaveState->G_PAT;
  } else {
    Value = AsmReadMsr64 (MSR_IA32_PAT);
  }

  AmdIntelEmuInternalWriteMsrValue64 (&SaveState->RAX, Registers, Value);
}

VOID
AmdIntelEmuInternalWrmsrPat (
  IN OUT AMD_VMCB_SAVE_STATE_AREA_NON_ES  *SaveState,
  IN OUT AMD_INTEL_EMU_REGISTERS          *Registers,
  OUT    AMD_VMCB_CLEAN_FIELD             *CleanField
  )
{
  MSR_IA32_PAT_REGISTER PatMsr;

  ASSERT (SaveState != NULL);
  ASSERT (Registers != NULL);
  ASSERT (CleanField != NULL);
  //
  // Undocumented fix by Bronya: Force WT-by-default PAT1 and PAT5 entries into
  // WC mode.  This is also done in the Linux kernel.
  //
  PatMsr.Uint64 = AmdIntelEmuInternalReadMsrValue64 (
                    &SaveState->RAX,
                    Registers
                    );
  PatMsr.Bits.PA1 = PAT_WC;
  PatMsr.Bits.PA5 = PAT_WC;

  if (mAmdIntelEmuInternalNp) {
    //
    // Inform the Guest of the new PAT for when Nested Paging is used.
    //
    SaveState->G_PAT    = PatMsr.Uint64;
    CleanField->Bits.NP = 0;
  } else {
    AsmWriteMsr64 (MSR_IA32_PAT, PatMsr.Uint64);
  }
}
