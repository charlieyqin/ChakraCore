//-------------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
//-------------------------------------------------------------------------------------------------------
#pragma once

enum LegalForms
{
    L_None =    0,

    L_Reg  =    1,
    L_RegMask = L_Reg,

    L_ImmLog12 =    0x10,
    L_ImmU12 =      0x20,
    L_ImmU12Lsl12 = 0x40,
    L_ImmU6 =       0x80,
    L_ImmU16 =      0x100,
    L_Imm =         0x200,
    L_ImmU6U6 =     0x400,
    L_ImmMask =     (L_ImmLog12 | L_ImmU12 | L_ImmU12Lsl12 | L_ImmU6 | L_ImmU16 | L_Imm | L_ImmU6U6),

    L_IndirSU12I9 = 0x1000,
    L_IndirSI7 =    0x2000,
    L_IndirU12 =    0x4000,
    L_IndirMask =  (L_IndirSU12I9 | L_IndirSI7 | L_IndirU12),

    L_SymSU12I9 =  0x10000,
    L_SymSI7 =     0x20000,
    L_SymU12 =     0x40000,
    L_SymMask =    (L_SymSU12I9 | L_SymSI7 | L_SymU12),

    L_Lab20 =      0x100000,

    L_RegBV =      0x1000000,
};

struct LegalInstrForms
{
    LegalForms dst, src[2];
};

#define LEGAL_NONE     { L_None,    { L_None,    L_None } }

// Used for opcodes that should be converted before legalizer kicks in
#define LEGAL_PSEUDO   LEGAL_NONE

#define LEGAL_ADDSUB   { L_Reg,     { L_Reg,     (LegalForms)(L_Reg | L_ImmU12 | L_ImmU12Lsl12) } }
#define LEGAL_ALU2     { L_Reg,     { (LegalForms)(L_Reg | L_ImmLog12), L_None } }
#define LEGAL_ALU3     { L_Reg,     { L_Reg,     (LegalForms)(L_Reg | L_ImmLog12) } }
#define LEGAL_SHIFT    { L_Reg,     { L_Reg,     (LegalForms)(L_Reg | L_ImmU6) } }
#define LEGAL_BITFIELD { L_Reg,     { L_Reg,     L_ImmU6U6 } }
#define LEGAL_BLAB     LEGAL_NONE
#define LEGAL_CALL     { L_Reg,     { L_Lab20 ,  L_None } } // Not currently generated, offset check is missing
#define LEGAL_CBZ      { L_None,    { L_Reg } }
#define LEGAL_LDIMM    { L_Reg,     { L_Imm,     L_None } }
#define LEGAL_LOAD     { L_Reg,     { (LegalForms)(L_IndirSU12I9 | L_SymSU12I9), L_None } }
#define LEGAL_LOADP    { L_Reg,     { (LegalForms)(L_IndirSI7 | L_SymSI7), L_Reg } }
#define LEGAL_PLD      { L_None,    { (LegalForms)(L_IndirSU12I9 | L_SymSU12I9), L_None } }
#define LEGAL_REG1     { L_Reg,     { L_None,    L_None } }
#define LEGAL_REG2     { L_Reg,     { L_Reg,     L_None } }
#define LEGAL_REG2_ND  { L_None,    { L_Reg,     L_None } }
#define LEGAL_REG3     { L_Reg,     { L_Reg,     L_Reg  } }
#define LEGAL_REG3_ND  { L_None,    { L_Reg,     L_Reg } }
#define LEGAL_STORE    { (LegalForms)(L_IndirSU12I9 | L_SymSU12I9), { L_Reg, L_None } }
#define LEGAL_STOREP   { (LegalForms)(L_IndirSI7 | L_SymSI7), { L_Reg, L_Reg } }
#define LEGAL_TBZ      { L_None,    { L_Reg,     L_ImmU6 } }

class LegalizeMD
{
public:
    static void LegalizeInstr(IR::Instr * instr, bool fPostRegAlloc);
    static void LegalizeDst(IR::Instr * instr, bool fPostRegAlloc);
    static void LegalizeSrc(IR::Instr * instr, IR::Opnd * opnd, uint opndNum, bool fPostRegAlloc);

    static bool LegalizeDirectBranch(IR::BranchInstr *instr, uint32 branchOffset); // DirectBranch has no src & dst operands.
    //Returns IndexOpnd which is removed from VFP indirect operand
    static void LegalizeIndirOpndForVFP(IR::Instr* insertInstr, IR::IndirOpnd *indirOpnd, bool fPostRegAlloc);

private:
    static IR::Instr *LegalizeStore(IR::Instr *instr, LegalForms forms, bool fPostRegAlloc);
    static IR::Instr *LegalizeLoad(IR::Instr *instr, uint opndNum, LegalForms forms, bool fPostRegAlloc);
    static void LegalizeIndirOffset(IR::Instr * instr, IR::IndirOpnd * indirOpnd, LegalForms forms, bool fPostRegAlloc);
    static void LegalizeSymOffset(IR::Instr * instr, IR::SymOpnd * indirOpnd, LegalForms forms, bool fPostRegAlloc);
    static void LegalizeImmed(IR::Instr * instr, IR::Opnd * opnd, uint opndNum, IntConstType immed, LegalForms forms, bool fPostRegAlloc);
    static void LegalizeLabelOpnd(IR::Instr * instr, IR::Opnd * opnd, uint opndNum, bool fPostRegAlloc);

    static void LegalizeLDIMM(IR::Instr * instr, IntConstType immed);
    static void LegalizeLdLabel(IR::Instr * instr, IR::Opnd * opnd);
    static IR::Instr * GenerateLDIMM(IR::Instr * instr, uint opndNum, RegNum scratchReg);

    static void ObfuscateLDIMM(IR::Instr * instrMov, IR::Instr * instrMovt);
    static void EmitRandomNopBefore(IR::Instr * instrMov, UINT_PTR rand, RegNum targetReg);

#ifdef DBG
    static void IllegalInstr(IR::Instr * instr, const char16 * msg, ...);
#endif
};