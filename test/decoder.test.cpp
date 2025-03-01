#include "deps.hh"
#include <gtest/gtest.h>
#include <spdlog/common.h>

#include "luce/Support/isa/IDisassembler.hpp"

#include "luce/Support/isa/riscv32/Disassembler.hpp"
#include "luce/Support/isa/riscv32/instruction/Multiply.hpp"
#include "luce/Support/isa/IDisassembler.hpp"

using namespace accat::auxilia;
using namespace accat::luce::isa;
using namespace accat::luce::isa::riscv32;
using enum FormatPolicy;

AC_SPDLOG_INITIALIZATION(luce, info)

auto createDisassembler() -> std::unique_ptr<IDisassembler> {
  return std::make_unique<Disassembler>();
}
TEST(decode, uninitialized) {
  auto disassembler = createDisassembler();
  // Disassembler is not initialized, so it should not be able to decode any
  // instructions.
  uint32_t instr = 0xdeadbeef;
  EXPECT_THROW(disassembler->disassemble(instr), std::runtime_error);
}
TEST(decode, base) {
  auto disassembler = createDisassembler();
  disassembler->initializeDefault();
  // ADD instruction (rv32I): add x1, x2, x3
  // Encoding: [ funct7 | rs2   | rs1   | funct3 | rd   | opcode ]
  //         = [ 0x00   | x3(3) | x2(2) | 0x0    | x1(1)| 0x33   ]
  uint32_t add_instr =
      (0x00 << 25) | (3 << 20) | (2 << 15) | (0x0 << 12) | (1 << 7) | 0x33;
  auto add_inst = disassembler->disassemble(add_instr);
  ASSERT_TRUE(add_inst);
  EXPECT_EQ("add x1, x2, x3", add_inst->to_string(kDefault));

  // SUB instruction (rv32I): sub x1, x2, x3
  // Encoding: [ funct7   | rs2   | rs1   | funct3 | rd   | opcode ]
  //         = [ 0x20     | x3(3) | x2(2) | 0x0    | x1(1)| 0x33   ]
  uint32_t sub_instr =
      (0x20 << 25) | (3 << 20) | (2 << 15) | (0x0 << 12) | (1 << 7) | 0x33;
  auto sub_inst = disassembler->disassemble(sub_instr);
  ASSERT_TRUE(sub_inst);
  EXPECT_EQ("sub x1, x2, x3", sub_inst->to_string(kDefault));

  // AND instruction: and x4, x5, x6
  uint32_t and_instr =
      (0x00 << 25) | (6 << 20) | (5 << 15) | (0x7 << 12) | (4 << 7) | 0x33;
  auto and_inst = disassembler->disassemble(and_instr);
  ASSERT_TRUE(and_inst);
  EXPECT_EQ("and x4, x5, x6", and_inst->to_string(kDefault));

  // OR instruction: or x7, x8, x9
  uint32_t or_instr =
      (0x00 << 25) | (9 << 20) | (8 << 15) | (0x6 << 12) | (7 << 7) | 0x33;
  auto or_inst = disassembler->disassemble(or_instr);
  ASSERT_TRUE(or_inst);
  EXPECT_EQ("or x7, x8, x9", or_inst->to_string(kDefault));

  // XOR instruction: xor x10, x11, x12
  uint32_t xor_instr =
      (0x00 << 25) | (12 << 20) | (11 << 15) | (0x4 << 12) | (10 << 7) | 0x33;
  auto xor_inst = disassembler->disassemble(xor_instr);
  ASSERT_TRUE(xor_inst);
  EXPECT_EQ("xor x10, x11, x12", xor_inst->to_string(kDefault));

  // SLL instruction: sll x13, x14, x15
  uint32_t sll_instr =
      (0x00 << 25) | (15 << 20) | (14 << 15) | (0x1 << 12) | (13 << 7) | 0x33;
  auto sll_inst = disassembler->disassemble(sll_instr);
  ASSERT_TRUE(sll_inst);
  EXPECT_EQ("sll x13, x14, x15", sll_inst->to_string(kDefault));

  // SRL instruction: srl x16, x17, x18
  uint32_t srl_instr =
      (0x00 << 25) | (18 << 20) | (17 << 15) | (0x5 << 12) | (16 << 7) | 0x33;
  auto srl_inst = disassembler->disassemble(srl_instr);
  ASSERT_TRUE(srl_inst);
  EXPECT_EQ("srl x16, x17, x18", srl_inst->to_string(kDefault));

  // SRA instruction: sra x19, x20, x21
  uint32_t sra_instr =
      (0x20 << 25) | (21 << 20) | (20 << 15) | (0x5 << 12) | (19 << 7) | 0x33;
  auto sra_inst = disassembler->disassemble(sra_instr);
  ASSERT_TRUE(sra_inst);
  EXPECT_EQ("sra x19, x20, x21", sra_inst->to_string(kDefault));

  // SLT instruction: slt x22, x23, x24
  uint32_t slt_instr =
      (0x00 << 25) | (24 << 20) | (23 << 15) | (0x2 << 12) | (22 << 7) | 0x33;
  auto slt_inst = disassembler->disassemble(slt_instr);
  ASSERT_TRUE(slt_inst);
  EXPECT_EQ("slt x22, x23, x24", slt_inst->to_string(kDefault));

  // SLTU instruction: sltu x25, x26, x27
  uint32_t sltu_instr =
      (0x00 << 25) | (27 << 20) | (26 << 15) | (0x3 << 12) | (25 << 7) | 0x33;
  auto sltu_inst = disassembler->disassemble(sltu_instr);
  ASSERT_TRUE(sltu_inst);
  EXPECT_EQ("sltu x25, x26, x27", sltu_inst->to_string(kDefault));
  // TODO: Add tests for the base instruction set.
}

TEST(decode, multiply) {
  auto disassembler = createDisassembler();
  disassembler->initializeDefault().addDecoder(
      std::make_unique<instruction::multiply::Decoder>());

  // The following values are constructed using the RISC-V R-type format for
  // instructions with:
  //   opcode = 0x33,
  //   rd = x3 (3 << 7),
  //   rs1 = x1 (1 << 15),
  //   rs2 = x2 (2 << 20),
  //   funct7 = 0x01 (<< 25),
  // and the funct3 field set accordingly.

  // MUL: Multiply low (signed × signed)
  // funct3 = 0x0
  uint32_t mul =
      (0x01 << 25) | (2 << 20) | (1 << 15) | (0x0 << 12) | (3 << 7) | 0x33;
  auto mul_inst = disassembler->disassemble(mul);
  EXPECT_EQ("mul x3, x1, x2", mul_inst->to_string(kDefault));

  // MULH: Multiply high (signed × signed)
  // funct3 = 0x1
  uint32_t mulh =
      (0x01 << 25) | (2 << 20) | (1 << 15) | (0x1 << 12) | (3 << 7) | 0x33;
  auto mulh_inst = disassembler->disassemble(mulh);
  EXPECT_EQ("mulh x3, x1, x2", mulh_inst->to_string(kDefault));

  // MULHSU: Multiply high (signed × unsigned)
  // funct3 = 0x2
  uint32_t mulsu =
      (0x01 << 25) | (2 << 20) | (1 << 15) | (0x2 << 12) | (3 << 7) | 0x33;
  auto mulhsu_inst = disassembler->disassemble(mulsu);
  EXPECT_EQ("mulsu x3, x1, x2", mulhsu_inst->to_string(kDefault));

  // MULHU: Multiply high (unsigned × unsigned)
  // funct3 = 0x3
  uint32_t mulu =
      (0x01 << 25) | (2 << 20) | (1 << 15) | (0x3 << 12) | (3 << 7) | 0x33;
  auto mulhu_inst = disassembler->disassemble(mulu);
  EXPECT_EQ("mulu x3, x1, x2", mulhu_inst->to_string(kDefault));

  // DIV: Signed division
  // funct3 = 0x4
  uint32_t div =
      (0x01 << 25) | (2 << 20) | (1 << 15) | (0x4 << 12) | (3 << 7) | 0x33;
  auto div_inst = disassembler->disassemble(div);
  EXPECT_EQ("div x3, x1, x2", div_inst->to_string(kDefault));

  // DIVU: Unsigned division
  // funct3 = 0x5
  uint32_t divu =
      (0x01 << 25) | (2 << 20) | (1 << 15) | (0x5 << 12) | (3 << 7) | 0x33;
  auto divu_inst = disassembler->disassemble(divu);
  EXPECT_EQ("divu x3, x1, x2", divu_inst->to_string(kDefault));

  // REM: Signed remainder
  // funct3 = 0x6
  uint32_t rem =
      (0x01 << 25) | (2 << 20) | (1 << 15) | (0x6 << 12) | (3 << 7) | 0x33;
  auto rem_inst = disassembler->disassemble(rem);
  EXPECT_EQ("rem x3, x1, x2", rem_inst->to_string(kDefault));

  // REMU: Unsigned remainder
  // funct3 = 0x7
  uint32_t remu =
      (0x01 << 25) | (2 << 20) | (1 << 15) | (0x7 << 12) | (3 << 7) | 0x33;
  auto remu_inst = disassembler->disassemble(remu);
  EXPECT_EQ("remu x3, x1, x2", remu_inst->to_string(kDefault));
}

TEST(decode, unregistered) {
  auto disassembler = createDisassembler();
  disassembler->initializeDefault();

  uint32_t div =
      (0x01 << 25) | (2 << 20) | (1 << 15) | (0x4 << 12) | (3 << 7) | 0x33;
  auto div_inst = disassembler->disassemble(div);
  EXPECT_FALSE(div_inst);
}
