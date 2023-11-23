#include <fstream>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#define FRAME_TICKS 1000

#include "sim.h"

extern void app();

static SDL_Renderer* renderer;
static SDL_Window* window;
static Uint32 Ticks = 0;

void simInit()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    srand(time(NULL));
}

void simExit()
{
    SDL_Event event;
    while (1)
    {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void simFlush()
{
    SDL_PumpEvents();
    Uint32 cur_ticks = SDL_GetTicks() - Ticks;
    if (cur_ticks < FRAME_TICKS)
    {
        SDL_Delay(FRAME_TICKS - cur_ticks);
    }
    SDL_RenderPresent(renderer);
}

void simPutPixel(int x, int y, int argb)
{
    Uint8 a = argb >> 24;
    Uint8 r = (argb >> 16) & 0xFF;
    Uint8 g = (argb >> 8) & 0xFF;
    Uint8 b = argb & 0xFF;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawPoint(renderer, x, y);
    Ticks = SDL_GetTicks();
}

int simRand()
{
    return rand();
}

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

//////////////////////////////
// All instructions
//////////////////////////////

enum InsnId_t {
  EXIT,  // 0
  NEG,   // 1r
  ADD,   // 3r
  SUB,   // 3r
  MUL,   // 3r
  DIV,   // 3r
  ADDI,  // 2r imm
  SUBI,  // 2r imm
  MULI,  // 2r imm
  DIVI,  // 2r imm
  RET,   // 0
  BL,    // imm
  BEQ,   // 2r imm
  BNE,   // 2r imm
  BGE,   // 2r imm
  READ,  // 1r
  WRITE, // 1r
  ALLOCA,// 1r imm
  SETI,  // 2r imm
  GET,   // 3r
  MOV,   // 2r
  MOVI,  // 1r imm
  CALL,  // label
  LCALL, // imm
  REMI,  // 2r, imm
  ADDRMI,// 2r, imm
  BNEI,  // 1r, 2imm
  BINCI, // 1r, 2imm
  BLE,   // 2r, imm
  BEI,   // 1r, 2imm
  B,     // imm
  BLEI,
  DUMPREG,
  DUMPSTORAGE
};

//////////////////////////////
// Model for simulation
//////////////////////////////

using RegId_t = uint8_t;
using RegVal_t = uint32_t;
using Stack_t = std::stack<RegVal_t>;

const int REG_FILE_SIZE = 16;
const int STORAGE_FILE_SIZE = 10000;
class CPU {
public:
  RegVal_t REG_FILE[REG_FILE_SIZE] = {};
  RegVal_t STORAGE_FILE[STORAGE_FILE_SIZE] = {};
  RegVal_t PC;
  RegVal_t NEXT_PC;
  Stack_t CALL_STACK;
  Stack_t STACK;
  uint32_t RUN;
  uint32_t STORAGE_POINTER;
  bool stack_ok() {
    if (STACK.empty()) {
      RUN = 0;
      outs() << "[RUNTIME ERROR] STACK ERROR\n";
      return false;
    }
    return true;
  }
  bool call_stack_ok() {
    if (CALL_STACK.empty()) {
      RUN = 0;
      outs() << "[RUNTIME ERROR] CALL STACK ERROR\n";
      return false;
    }
    return true;
  }
};

//////////////////////////////
// Universal Instruction
//////////////////////////////

class Instr {
public:
  InsnId_t m_ID;
  void (*m_INSTR)(CPU *, Instr *);
  RegId_t m_rs1;
  RegId_t m_rs2;
  RegId_t m_rs3;
  RegVal_t m_imm;
  RegVal_t m_imm_l;
  std::string m_name;
  std::string m_label;
  Instr(InsnId_t ID, void (*do_INSTR)(CPU *, Instr *), std::string name)
      : m_ID(ID), m_INSTR(do_INSTR), m_name(name) {}
  Instr(InsnId_t ID, void (*do_INSTR)(CPU *, Instr *), std::string name,
        RegId_t rs1)
      : m_ID(ID), m_INSTR(do_INSTR), m_name(name), m_rs1(rs1) {}
  Instr(InsnId_t ID, void (*do_INSTR)(CPU *, Instr *), std::string name,
        RegVal_t imm)
      : m_ID(ID), m_INSTR(do_INSTR), m_name(name), m_imm(imm) {}
  Instr(InsnId_t ID, void (*do_INSTR)(CPU *, Instr *), std::string name,
        RegId_t rs1, RegId_t rs2, RegId_t rs3)
      : m_ID(ID), m_INSTR(do_INSTR), m_name(name), m_rs1(rs1), m_rs2(rs2),
        m_rs3(rs3) {}
  Instr(InsnId_t ID, void (*do_INSTR)(CPU *, Instr *), std::string name,
        RegId_t rs1, RegId_t rs2, RegVal_t imm)
      : m_ID(ID), m_INSTR(do_INSTR), m_name(name), m_rs1(rs1), m_rs2(rs2),
        m_imm(imm) {}
  Instr(InsnId_t ID, void (*do_INSTR)(CPU *, Instr *), std::string name,
        std::string label)
      : m_ID(ID), m_INSTR(do_INSTR), m_name(name), m_label(label) {}
  Instr(InsnId_t ID, void (*do_INSTR)(CPU *, Instr *), std::string name,
        RegId_t rs1, RegVal_t imm, RegVal_t imm_l)
      : m_ID(ID), m_INSTR(do_INSTR), m_name(name), m_rs1(rs1), m_imm(imm),
        m_imm_l(imm_l) {}
  Instr(InsnId_t ID, void (*do_INSTR)(CPU *, Instr *), std::string name,
        RegId_t rs1, RegVal_t imm)
      : m_ID(ID), m_INSTR(do_INSTR), m_name(name), m_rs1(rs1), m_imm(imm) {}
  Instr(InsnId_t ID, void (*do_INSTR)(CPU *, Instr *), std::string name,
        RegId_t rs1, RegVal_t imm, std::string label)
      : m_ID(ID), m_INSTR(do_INSTR), m_name(name), m_rs1(rs1), m_imm(imm),
       m_label(label) {}
  Instr(InsnId_t ID, void (*do_INSTR)(CPU *, Instr *), std::string name,
        RegId_t rs1, RegId_t rs2)
      : m_ID(ID), m_INSTR(do_INSTR), m_name(name), m_rs1(rs1), m_rs2(rs2) {outs() << "!!!!!!!\n";}
  void dump() { outs() << m_name << "\n"; }
};

//////////////////////////////
// Interpreter function
//////////////////////////////

void do_exit(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->RUN = 0;
}
void do_dumpreg(CPU *cpu, Instr *instr) {
  for (int i = 0; i < REG_FILE_SIZE; i++) {
    outs() << "[" << i << "] " << cpu->REG_FILE[i] << "\n";
  }
}
void do_dumpstorage(CPU *cpu, Instr *instr) {
  for (int i = 0; i < 5000; i++) {
    outs() << "[" << i << "] " << cpu->STORAGE_FILE[i] << "\n";
  }
}
void do_neg(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] = -cpu->REG_FILE[instr->m_rs1];
}
void do_add(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] =
      cpu->REG_FILE[instr->m_rs2] + cpu->REG_FILE[instr->m_rs3];
}
void do_sub(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] =
      cpu->REG_FILE[instr->m_rs2] - cpu->REG_FILE[instr->m_rs3];
}
void do_mul(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] =
      cpu->REG_FILE[instr->m_rs2] * cpu->REG_FILE[instr->m_rs3];
}
void do_div(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] =
      cpu->REG_FILE[instr->m_rs2] / cpu->REG_FILE[instr->m_rs3];
}
void do_addi(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] = cpu->REG_FILE[instr->m_rs2] + instr->m_imm;
}
void do_subi(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] = cpu->REG_FILE[instr->m_rs2] - instr->m_imm;
}
void do_muli(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] = cpu->REG_FILE[instr->m_rs2] * instr->m_imm;
}
void do_divi(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] = cpu->REG_FILE[instr->m_rs2] / instr->m_imm;
}
void do_b(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->NEXT_PC = instr->m_imm;
}
void do_ret(CPU *cpu, Instr *instr) {
  instr->dump();
  outs() << cpu->CALL_STACK.size() << '\n';
  if (!cpu->call_stack_ok()) {
    return;
  }
  cpu->NEXT_PC = cpu->CALL_STACK.top();
  cpu->CALL_STACK.pop();
}
void do_bl(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->CALL_STACK.push(cpu->PC + 1);
  cpu->NEXT_PC = instr->m_imm;
}
void do_beq(CPU *cpu, Instr *instr) {
  instr->dump();
  if (cpu->REG_FILE[instr->m_rs1] == cpu->REG_FILE[instr->m_rs2])
    cpu->NEXT_PC = instr->m_imm;
}
void do_bne(CPU *cpu, Instr *instr) {
  instr->dump();
  if (cpu->REG_FILE[instr->m_rs1] != cpu->REG_FILE[instr->m_rs2])
    cpu->NEXT_PC = instr->m_imm;
}
void do_bge(CPU *cpu, Instr *instr) {
  instr->dump();
  if (cpu->REG_FILE[instr->m_rs1] >= cpu->REG_FILE[instr->m_rs2])
    cpu->NEXT_PC = instr->m_imm;
}
void do_ble(CPU *cpu, Instr *instr) {
  instr->dump();
  if (cpu->REG_FILE[instr->m_rs1] < cpu->REG_FILE[instr->m_rs2])
    cpu->NEXT_PC = instr->m_imm;
}
void do_read(CPU *cpu, Instr *instr) {
  instr->dump();
  outs() << "[x" << (uint32_t)instr->m_rs1 << "] = ";
  std::cin >> cpu->REG_FILE[instr->m_rs1];
}
void do_write(CPU *cpu, Instr *instr) {
  instr->dump();
  outs() << "[x" << (uint32_t)instr->m_rs1
         << "] = " << cpu->REG_FILE[instr->m_rs1] << "\n";
}
//my asm
void do_alloca(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] = cpu->STORAGE_POINTER;
  cpu->STORAGE_POINTER += instr->m_imm;
}
void do_seti(CPU *cpu, Instr *instr) {
  instr->dump();
  if (instr->m_imm == 1) {
    outs() << "aaaaaaaaaaaaaaaaaaaa" << '\n';
  }
  cpu->STORAGE_FILE[cpu->REG_FILE[instr->m_rs1] + cpu->REG_FILE[instr->m_rs2]] = instr->m_imm;
}
void do_get(CPU *cpu, Instr *instr) {
  outs() << cpu->STORAGE_FILE[cpu->REG_FILE[instr->m_rs2] + cpu->REG_FILE[instr->m_rs3]] << ' ';
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] = cpu->STORAGE_FILE[cpu->REG_FILE[instr->m_rs2] + cpu->REG_FILE[instr->m_rs3]];
}
void do_mov(CPU *cpu, Instr *instr) {
  instr->dump();
  outs() << (int)instr->m_rs1 << " " << (int)instr->m_rs2 << '\n';
  cpu->REG_FILE[instr->m_rs1] = cpu->REG_FILE[instr->m_rs2];
}
void do_movi(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] = instr->m_imm;
}
void do_call(CPU *cpu, Instr *instr) {
  instr->dump();
  std::string f_name = instr->m_label;
  outs() << f_name << '\n';
  if (f_name == "simFlush") {
    simFlush();
  }
  if (f_name == "simPutPixel") {
    simPutPixel(cpu->REG_FILE[0], cpu->REG_FILE[1], cpu->REG_FILE[2]);
  }
  if (f_name == "simRand") {
    cpu->REG_FILE[0] = simRand();
    outs() << cpu->REG_FILE[0] << '\n';
  }
}
void do_lcall(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->CALL_STACK.push(cpu->PC + 1);
  outs() << cpu->CALL_STACK.size() << '\n';
  cpu->NEXT_PC = instr->m_imm;
}
void do_remi(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] = cpu->REG_FILE[instr->m_rs2] % instr->m_imm;
  outs() << cpu->REG_FILE[instr->m_rs2] % instr->m_imm << '\n';
}
void do_bnei(CPU *cpu, Instr *instr) {
  instr->dump();
  if (cpu->REG_FILE[instr->m_rs1] != instr->m_imm)
    cpu->NEXT_PC = instr->m_imm_l;
}
void do_binci(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] += 1;
  if (cpu->REG_FILE[instr->m_rs1] < instr->m_imm)
    cpu->NEXT_PC = instr->m_imm_l;
}
void do_addrmi(CPU *cpu, Instr *instr) {
  instr->dump();
  cpu->REG_FILE[instr->m_rs1] = (cpu->REG_FILE[instr->m_rs2] + instr->m_imm) % instr->m_imm;
}
void do_bei(CPU *cpu, Instr *instr) {
  instr->dump();
  if (cpu->REG_FILE[instr->m_rs1] == instr->m_imm)
    cpu->NEXT_PC = instr->m_imm_l;
}
void do_blei(CPU *cpu, Instr *instr) {
  instr->dump();
  if (cpu->REG_FILE[instr->m_rs1] < instr->m_imm)
    cpu->NEXT_PC = instr->m_imm_l;
}

void *lazyFunctionCreator(const std::string &fnName) {
  if (fnName == "do_exit") {
    return reinterpret_cast<void *>(do_exit);
  }
  if (fnName == "do_neg") {
    return reinterpret_cast<void *>(do_neg);
  }
  if (fnName == "do_add") {
    return reinterpret_cast<void *>(do_add);
  }
  if (fnName == "do_sub") {
    return reinterpret_cast<void *>(do_sub);
  }
  if (fnName == "do_mul") {
    return reinterpret_cast<void *>(do_mul);
  }
  if (fnName == "do_div") {
    return reinterpret_cast<void *>(do_div);
  }
  if (fnName == "do_addi") {
    return reinterpret_cast<void *>(do_addi);
  }
  if (fnName == "do_subi") {
    return reinterpret_cast<void *>(do_subi);
  }
  if (fnName == "do_muli") {
    return reinterpret_cast<void *>(do_muli);
  }
  if (fnName == "do_divi") {
    return reinterpret_cast<void *>(do_divi);
  }
  if (fnName == "do_b") {
    return reinterpret_cast<void *>(do_b);
  }
  if (fnName == "do_ret") {
    return reinterpret_cast<void *>(do_ret);
  }
  if (fnName == "do_bl") {
    return reinterpret_cast<void *>(do_bl);
  }
  if (fnName == "do_beq") {
    return reinterpret_cast<void *>(do_beq);
  }
  if (fnName == "do_bne") {
    return reinterpret_cast<void *>(do_bne);
  }
  if (fnName == "do_bge") {
    return reinterpret_cast<void *>(do_bge);
  }
  if (fnName == "do_blt") {
    return reinterpret_cast<void *>(do_ble);
  }
  if (fnName == "do_read") {
    return reinterpret_cast<void *>(do_read);
  }
  if (fnName == "do_write") {
    return reinterpret_cast<void *>(do_write);
  }
  if (fnName == "do_alloca") {
    return reinterpret_cast<void *>(do_alloca);
  }
  if (fnName == "do_movi") {
    return reinterpret_cast<void *>(do_movi);
  }
  if (fnName == "do_seti") {
    return reinterpret_cast<void *>(do_seti);
  }
  if (fnName == "do_call") {
    return reinterpret_cast<void *>(do_call);
  }
  if (fnName == "do_lcall") {
    return reinterpret_cast<void *>(do_lcall);
  }
  if (fnName == "simPutPixel") {
      return reinterpret_cast<void *>(simPutPixel);
    }
  if (fnName == "simFlush") {
    return reinterpret_cast<void *>(simFlush);
  }
  if (fnName == "simRand") {
    return reinterpret_cast<void *>(simRand);
  }
  return nullptr;
}

//////////////////////////////
// MAIN
//////////////////////////////

int main(int argc, char *argv[]) {
  if (argc != 2) {
    outs() << "[ERROR] Need 1 argument: file with assembler\n";
    return 1;
  }
  std::ifstream input;
  input.open(argv[1]);
  if (!input.is_open()) {
    outs() << "[ERROR] Can't open " << argv[1] << "\n";
    return 1;
  }

  std::string name;
  std::string arg;
  std::unordered_map<std::string, RegVal_t> BB_PC;

  outs() << "\n#[FILE]:\nBBs:";
  RegVal_t pc = 1;
  while (input >> name) {
    // 3 args
    if (!name.compare("add") || !name.compare("addi") || !name.compare("mul") ||
        !name.compare("muli") || !name.compare("bne") || !name.compare("seti")||
        !name.compare("remi") || !name.compare("binci") || !name.compare("ble")||
        !name.compare("get") || !name.compare("blei") || !name.compare("addrmi") ||
        !name.compare("be") || !name.compare("bnei") || !name.compare("sub") || !name.compare("bei")) {
      input >> arg >> arg >> arg;
      pc++;
      continue;
    }
    // 2 args
    if (!name.compare("alloca") || !name.compare("mov") || !name.compare("movi")) {
      input >> arg >> arg;
      pc++;
      continue;
    }
    // 1 arg
    if (!name.compare("b") || !name.compare("call") || !name.compare("lcall")) {
      input >> arg;
      pc++;
      continue;
    }
    // 0 args
    if (!name.compare("exit") || !name.compare("ret") || !name.compare("dumpreg") || !name.compare("dumpstorage")) {
      pc++;
      outs() << " " << name << "(" << pc << ")";
      continue;
    }
    outs() << " " << name << "(" << pc << ")";
    BB_PC[name] = pc;
  }
  outs() << "\n";
  input.close();
  input.open(argv[1]);

  std::string arg1;
  std::string arg2;
  std::string arg3;
  std::vector<Instr *> Instructions;
  Instructions.push_back(
      new Instr(InsnId_t::EXIT, do_exit, "[RUNTIME ERROR] Segmentation fault"));
  // Read instruction from file
  outs() << "#[FILE] BEGIN\n";
  while (input >> name) {
    outs() << name;
    // 0 registers
    if (!name.compare("exit")) {
      Instructions.push_back(new Instr(InsnId_t::EXIT, do_exit, name));
      outs() << "\n";
      continue;
    }
    if (!name.compare("ret")) {
      Instructions.push_back(new Instr(InsnId_t::RET, do_ret, name));
      outs() << "\n";
      continue;
    }
    if (!name.compare("dumpreg")) {
      Instructions.push_back(new Instr(InsnId_t::DUMPREG, do_dumpreg, name));
      outs() << "\n";
      continue;
    }
    if (!name.compare("dumpstorage")) {
      Instructions.push_back(new Instr(InsnId_t::DUMPSTORAGE, do_dumpstorage, name));
      outs() << "\n";
      continue;
    }

    // 3 registers
    if (!name.compare("add") || !name.compare("mul") || !name.compare("get") || !name.compare("sub")) {
      input >> arg1 >> arg2 >> arg3;
      outs() << " " << arg1 << " " << arg2 << " " << arg3 << "\n";
      RegId_t rs1 = stoi(arg1.substr(1));
      RegId_t rs2 = stoi(arg2.substr(1));
      RegId_t rs3 = stoi(arg3.substr(1));
      if (!name.compare("add")) {
        Instructions.push_back(
            new Instr(InsnId_t::ADD, do_add, name, rs1, rs2, rs3));
      }
      if (!name.compare("sub")) {
        Instructions.push_back(
            new Instr(InsnId_t::SUB, do_sub, name, rs1, rs2, rs3));
      }
      if (!name.compare("mul")) {
        Instructions.push_back(
            new Instr(InsnId_t::MUL, do_mul, name, rs1, rs2, rs3));
      }
      if (!name.compare("get")) {
        Instructions.push_back(
            new Instr(InsnId_t::GET, do_get, name, rs1, rs2, rs3));
      }
      continue;
    }

    // 1 imm
    if (!name.compare("b") || !name.compare("bl") || !name.compare("lcall")) {
      input >> arg1;
      outs() << " " << arg1 << "\n";
      RegVal_t imm = BB_PC[arg1];
      if (!name.compare("b")) {
        Instructions.push_back(new Instr(InsnId_t::B, do_b, name, imm));
      }
      if (!name.compare("bl")) {
        Instructions.push_back(new Instr(InsnId_t::BL, do_bl, name, imm));
      }
      if (!name.compare("lcall")) {
        Instructions.push_back(new Instr(InsnId_t::LCALL, do_lcall, name, imm));
      }
      if (!name.compare("call")) {
        Instructions.push_back(new Instr(InsnId_t::CALL, do_call, name, arg1));
      }
      continue;
    }

    //1 label
    if (!name.compare("call")) {
      input >> arg1;
      outs() << " " << arg1 << "\n";
      std::string label = arg1;
      if (!name.compare("call")) {
        Instructions.push_back(new Instr(InsnId_t::CALL, do_call, name, label));
      }
      continue;
    }

    // 2 registers and imm
    if (!name.compare("addi") || !name.compare("subi") ||
        !name.compare("muli") || !name.compare("divi") ||
        !name.compare("seti") || !name.compare("remi") || !name.compare("addrmi")) {
      input >> arg1 >> arg2 >> arg3;
      outs() << " " << arg1 << " " << arg2 << " " << arg3 << "\n";
      RegId_t rs1 = stoul(arg1.substr(1));
      RegId_t rs2 = stoul(arg2.substr(1));
      RegVal_t imm = stoul(arg3);
      if (!name.compare("addi")) {
        Instructions.push_back(
            new Instr(InsnId_t::ADDI, do_addi, name, rs1, rs2, imm));
      }
      if (!name.compare("subi")) {
        Instructions.push_back(
            new Instr(InsnId_t::SUBI, do_subi, name, rs1, rs2, imm));
      }
      if (!name.compare("muli")) {
        Instructions.push_back(
            new Instr(InsnId_t::MULI, do_muli, name, rs1, rs2, imm));
      }
      if (!name.compare("divi")) {
        Instructions.push_back(
            new Instr(InsnId_t::DIVI, do_divi, name, rs1, rs2, imm));
      }
      if (!name.compare("seti")) {
        Instructions.push_back(
            new Instr(InsnId_t::SETI, do_seti, name, rs1, rs2, imm));
      }
      if (!name.compare("remi")) {
        Instructions.push_back(
            new Instr(InsnId_t::REMI, do_remi, name, rs1, rs2, imm));
      }
      if (!name.compare("addrmi")) {
        Instructions.push_back(
            new Instr(InsnId_t::ADDRMI, do_addrmi, name, rs1, rs2, imm));
      }
      continue;
    }

    // 2 registers and label
    if (!name.compare("beq") || !name.compare("bne") || !name.compare("bge") ||
        !name.compare("blt") || !name.compare("ble")) {
      input >> arg1 >> arg2 >> arg3;
      outs() << " " << arg1 << " " << arg2 << " " << arg3 << "\n";
      RegId_t rs1 = stoi(arg1.substr(1));
      RegId_t rs2 = stoi(arg2.substr(1));
      RegVal_t imm = BB_PC[arg3];
      if (!name.compare("beq")) {
        Instructions.push_back(
            new Instr(InsnId_t::BEQ, do_beq, name, rs1, rs2, imm));
      }
      if (!name.compare("bne")) {
        Instructions.push_back(
            new Instr(InsnId_t::BNE, do_bne, name, rs1, rs2, imm));
      }
      if (!name.compare("bge")) {
        Instructions.push_back(
            new Instr(InsnId_t::BGE, do_bge, name, rs1, rs2, imm));
      }
      if (!name.compare("blt")) {
        Instructions.push_back(
            new Instr(InsnId_t::BLE, do_ble, name, rs1, rs2, imm));
      }
      if (!name.compare("ble")) {
        Instructions.push_back(
            new Instr(InsnId_t::BLE, do_ble, name, rs1, rs2, imm));
      }
      continue;
    }

    // 2 registers
    if (!name.compare("mov")) {
      input >> arg1 >> arg2;
      outs() << " " << arg1 << " " << arg2 << "\n";
      RegId_t rs1 = stoi(arg1.substr(1));
      RegId_t rs2 = stoi(arg2.substr(1));
      outs() << " " << arg1 << " " << arg2 << "\n";
      if (!name.compare("mov")) {
        Instructions.push_back(
            new Instr(InsnId_t::MOV, do_mov, name, rs1, rs2));
      }
      continue;
    }

    // 2 registers and imm
    if (!name.compare("beq") || !name.compare("bne") || !name.compare("bge") ||
        !name.compare("blt") || !name.compare("ble")) {
      input >> arg1 >> arg2 >> arg3;
      outs() << " " << arg1 << " " << arg2 << " " << arg3 << "\n";
      RegId_t rs1 = stoi(arg1.substr(1));
      RegId_t rs2 = stoi(arg2.substr(1));
      RegVal_t imm = BB_PC[arg3];
      if (!name.compare("beq")) {
        Instructions.push_back(
            new Instr(InsnId_t::BEQ, do_beq, name, rs1, rs2, imm));
      }
      if (!name.compare("bne")) {
        Instructions.push_back(
            new Instr(InsnId_t::BNE, do_bne, name, rs1, rs2, imm));
      }
      if (!name.compare("bge")) {
        Instructions.push_back(
            new Instr(InsnId_t::BGE, do_bge, name, rs1, rs2, imm));
      }
      if (!name.compare("blt")) {
        Instructions.push_back(
            new Instr(InsnId_t::BLE, do_ble, name, rs1, rs2, imm));
      }
      if (!name.compare("ble")) {
        Instructions.push_back(
            new Instr(InsnId_t::BLE, do_ble, name, rs1, rs2, imm));
      }
      continue;
    }

    // 1 registers and imm
    if (!name.compare("alloca") || !name.compare("movi")) {
      input >> arg1 >> arg2;
      outs() << " " << arg1 << " " << arg2 << "\n";
      RegId_t rs1 = stoi(arg1.substr(1));
      RegVal_t imm = stoul(arg2);
      if (!name.compare("alloca")) {
        Instructions.push_back(
            new Instr(InsnId_t::ALLOCA, do_alloca, name, rs1, imm));
      }
      if (!name.compare("movi")) {
        Instructions.push_back(
            new Instr(InsnId_t::MOVI, do_movi, name, rs1, imm));
      }
      continue;
    }

    // 1 registers and imm and label
    if (!name.compare("bnei") || !name.compare("binci") || !name.compare("bei") || !name.compare("blei")) {
      input >> arg1 >> arg2 >> arg3;
      outs() << " " << arg1 << " " << arg2 << " " << arg3 << "\n";
      RegId_t rs1 = stoi(arg1.substr(1));
      RegVal_t imm = stoul(arg2);
      RegVal_t imm_l = BB_PC[arg3];
      if (!name.compare("bnei")) {
        Instructions.push_back(
            new Instr(InsnId_t::BNEI, do_bnei, name, rs1, imm, imm_l));
      }
      if (!name.compare("binci")) {
        Instructions.push_back(
            new Instr(InsnId_t::BINCI, do_binci, name, rs1, imm, imm_l));
      }
      if (!name.compare("bei")) {
        Instructions.push_back(
            new Instr(InsnId_t::BEI, do_bei, name, rs1, imm, imm_l));
      }
      if (!name.compare("blei")) {
        Instructions.push_back(
            new Instr(InsnId_t::BLEI, do_blei, name, rs1, imm, imm_l));
      }
      continue;
    }

    if (BB_PC.find(name) == BB_PC.end()) {
      outs() << "\n[ERROR] Unknown instruction: " << name << "\n";
      Instructions.clear();
      return 1;
    }
    outs() << "\n";
  }
  outs() << "#[FILE] END\n";

  // simInit();

  // // App simulation
  // outs() << "\n#[EXEC] BEGIN\n";
  CPU cpu;
  for (int i = 0; i < REG_FILE_SIZE; i++) {
    cpu.REG_FILE[i] = 0;
  }
  cpu.RUN = 1;
  cpu.PC = 1;
  cpu.STORAGE_POINTER = 0;
  // cpu.CALL_STACK = 
  // Loop execution
  while (cpu.RUN) {
    cpu.NEXT_PC = cpu.PC + 1;
    Instructions[cpu.PC]->m_INSTR(&cpu, Instructions[cpu.PC]);
    cpu.PC = cpu.NEXT_PC;
  }
  outs() << "#[EXEC] END\n";

  // Dump registers after simulation
  for (int i = 0; i < REG_FILE_SIZE; i++) {
    outs() << "[" << i << "] " << cpu.REG_FILE[i] << "\n";
  }

  simExit();

  // Build IR for application
  outs() << "#[LLVM IR] BEGIN\n";
  LLVMContext context;
  // ; ModuleID = 'top'
  // source_filename = "top"
  Module *module = new Module("top", context);
  IRBuilder<> builder(context);

  // declare void @simRand()
  Type *int32Type = Type::getInt32Ty(context);
  FunctionType *simRandType =
      FunctionType::get(int32Type, {}, false);
  FunctionCallee simRandFunc =
      module->getOrInsertFunction("simRand", simRandType);

  // declare void @simPutPixel(i32 noundef, i32 noundef, i32 noundef)
  Type *voidType = Type::getVoidTy(context);
  ArrayRef<Type *> simPutPixelParamTypes = {Type::getInt32Ty(context),
                                            Type::getInt32Ty(context),
                                            Type::getInt32Ty(context)};
  FunctionType *simPutPixelType =
      FunctionType::get(voidType, simPutPixelParamTypes, false);
  FunctionCallee simPutPixelFunc =
      module->getOrInsertFunction("simPutPixel", simPutPixelType);

  // declare void @simFlush()
  FunctionType *simFlushType = FunctionType::get(voidType, {}, false);
  FunctionCallee simFlushFunc =
      module->getOrInsertFunction("simFlush", simFlushType);

  // declare void @main()
  FunctionType *funcType = FunctionType::get(builder.getVoidTy(), false);
  Function *mainFunc =
      Function::Create(funcType, Function::ExternalLinkage, "main", module);
  // entry:
  BasicBlock *entryBB = BasicBlock::Create(context, "entry", mainFunc);

  builder.SetInsertPoint(entryBB);

  // createCalleeFunctions(builder, module);
  FunctionType *CalleType = FunctionType::get(
      builder.getVoidTy(),
      ArrayRef<Type *>({builder.getInt8PtrTy(), builder.getInt8PtrTy()}),
      false);

  // Get poointer to CPU for function args
  Value *cpu_p = builder.getInt64((uint64_t)&cpu);
  ArrayType *regFileType = ArrayType::get(builder.getInt32Ty(), REG_FILE_SIZE);
  module->getOrInsertGlobal("regFile", regFileType);
  GlobalVariable *regFile = module->getNamedGlobal("regFile");

  ArrayType *storageFileType = ArrayType::get(builder.getInt32Ty(), STORAGE_FILE_SIZE);
  module->getOrInsertGlobal("storageFile", storageFileType);
  GlobalVariable *storageFile = module->getNamedGlobal("storageFile");

  module->getOrInsertGlobal("storagePointer", builder.getInt32Ty());
  GlobalVariable *storagePointer = module->getNamedGlobal("storagePointer");

  std::unordered_map<RegVal_t, BasicBlock *> BBMap;

  for (auto &name : BB_PC) {
    BBMap[name.second] = BasicBlock::Create(context, name.first, mainFunc);
  }

  for (RegVal_t PC = 1; PC < Instructions.size(); PC++) {
    // Set IRBuilder to current BB
    if (BBMap.find(PC) != BBMap.end()) {
      builder.CreateBr(BBMap[PC]);
      builder.SetInsertPoint(BBMap[PC]);
    }

    /*
    B,     // imm
    BEQ,   // 2r imm
    BNE,   // 2r imm
    BGE,   // 2r imm
    BLT,   // 2r imm
    */
    // IR implementation for B instruction
    if (Instructions[PC]->m_ID == B) {
      builder.CreateBr(BBMap[Instructions[PC]->m_imm]);
      continue;
    }
    // IR implementation for COND B instruction
    if (Instructions[PC]->m_ID == BLE) {
      // arg1
      Value *arg1_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs1);
      // arg2
      Value *arg2_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs2);
      Value *cond = nullptr;
      switch (Instructions[PC]->m_ID) {
      case BLE:
        cond = builder.CreateICmpULT(
            builder.CreateLoad(builder.getInt32Ty(), arg1_p),
            builder.CreateLoad(builder.getInt32Ty(), arg2_p));
        break;
      default:
        break;
      }
      if (cond) {
        builder.CreateCondBr(cond, BBMap[Instructions[PC]->m_imm],
                             BBMap[PC + 1]);
      }
      continue;
    }

    // IR implementation for COND B instruction
    if (Instructions[PC]->m_ID == BNEI || Instructions[PC]->m_ID == BLEI || Instructions[PC]->m_ID == BEI) {
      // arg1
      Value *arg1_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs1);
      // arg2
      Value *arg2 = builder.getInt32(Instructions[PC]->m_imm);

      Value *cond = nullptr;
      switch (Instructions[PC]->m_ID) {
      case BNEI:
        cond = builder.CreateICmpNE(
            builder.CreateLoad(builder.getInt32Ty(), arg1_p),
            arg2);
        break;
      case BEI:
        cond = builder.CreateICmpEQ(
            builder.CreateLoad(builder.getInt32Ty(), arg1_p),
            arg2);
        break;
      case BLEI:
        cond = builder.CreateICmpULT(
            builder.CreateLoad(builder.getInt32Ty(), arg1_p),
            arg2);
        break;
      default:
        break;
      }
      if (cond) {
        builder.CreateCondBr(cond, BBMap[Instructions[PC]->m_imm_l],
                             BBMap[PC + 1]);
      }
      continue;
    }

    // IR implementation for COND B instruction
    if (Instructions[PC]->m_ID == BINCI) {
      // arg1
      Value *arg1_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs1);
      // arg2
      Value *arg2 = builder.getInt32(Instructions[PC]->m_imm);

      Value *sum = builder.CreateAdd(builder.CreateLoad(builder.getInt32Ty(), arg1_p), builder.getInt32(1));
      builder.CreateStore(sum, arg1_p);

      Value *cond = builder.CreateICmpULT(sum, arg2);
      builder.CreateCondBr(cond, BBMap[Instructions[PC]->m_imm_l], BBMap[PC + 1]);
      continue;
    }

    // IR implementation for ADD instruction
    if (Instructions[PC]->m_ID == ADD || Instructions[PC]->m_ID == MUL || Instructions[PC]->m_ID == SUB) {
      // res
      Value *res_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                Instructions[PC]->m_rs1);
      // arg1
      Value *arg1_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs2);
      // arg2
      Value *arg2_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs3);
      Value *result = nullptr;
      switch (Instructions[PC]->m_ID) {
      case ADD:
        result = builder.CreateAdd(builder.CreateLoad(builder.getInt32Ty(), arg1_p),
                            builder.CreateLoad(builder.getInt32Ty(), arg2_p));
        break;
      case SUB:
        result = builder.CreateSub(builder.CreateLoad(builder.getInt32Ty(), arg1_p),
                            builder.CreateLoad(builder.getInt32Ty(), arg2_p));
        break;
      case MUL:
        result = builder.CreateMul(builder.CreateLoad(builder.getInt32Ty(), arg1_p),
                            builder.CreateLoad(builder.getInt32Ty(), arg2_p));
        break;
      default:
        break;
      }
      if (result) {
        builder.CreateStore(result, res_p);
      }
      continue;
    }

    // IR implementation for ADDI instruction
    if (Instructions[PC]->m_ID == ADDI || Instructions[PC]->m_ID == MULI || Instructions[PC]->m_ID == SUBI
        || Instructions[PC]->m_ID == ADDRMI || Instructions[PC]->m_ID == REMI) {
      // res
      Value *res_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                Instructions[PC]->m_rs1);
      // arg1
      Value *arg1_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs2);
      // arg2
      Value *arg2 = builder.getInt32(Instructions[PC]->m_imm);
      Value *result = nullptr;
      Value *sum = nullptr;
      switch (Instructions[PC]->m_ID) {
      case ADDI:
        result = builder.CreateAdd(builder.CreateLoad(builder.getInt32Ty(), arg1_p), arg2);
        break;
      case SUBI:
        result = builder.CreateSub(builder.CreateLoad(builder.getInt32Ty(), arg1_p), arg2);
        break;
      case MULI:
        result = builder.CreateMul(builder.CreateLoad(builder.getInt32Ty(), arg1_p), arg2);
        break;
      case REMI:
        result = builder.CreateSRem(builder.CreateLoad(builder.getInt32Ty(), arg1_p), arg2);
        break;
      case ADDRMI:
        sum = builder.CreateAdd(builder.CreateLoad(builder.getInt32Ty(), arg1_p), arg2);
        result = builder.CreateSRem(sum, arg2);
        break;
      default:
        break;
      }
      if (result) {
        builder.CreateStore(result, res_p);
      }
      continue;
    }

    // IR implementation for MOV instruction
    if (Instructions[PC]->m_ID == MOV) {
      // res
      Value *res_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                Instructions[PC]->m_rs1);
      // arg1
      Value *arg1_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs2);

      builder.CreateStore(builder.CreateLoad(builder.getInt32Ty(), arg1_p), res_p);
      continue;
    }

    // IR implementation for MOVI instruction
    if (Instructions[PC]->m_ID == MOVI) {
      // res
      Value *res_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                Instructions[PC]->m_rs1);
      // arg1
      Value *arg1 = builder.getInt32(Instructions[PC]->m_imm);
                                                 
      builder.CreateStore(arg1, res_p);
      continue;
    }

    // IR implementation for EXIT instruction
    if (Instructions[PC]->m_ID == EXIT) {             
      builder.CreateRetVoid();
      continue;
    }

    // IR implementation for ALLOCA instruction
    if (Instructions[PC]->m_ID == ALLOCA) {
      // res
      Value *res_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                Instructions[PC]->m_rs1);
      // arg1
      Value *arg1 = builder.getInt32(Instructions[PC]->m_imm);

      Value *pointer = builder.CreateLoad(builder.getInt32Ty(), storagePointer);
                                           
      builder.CreateStore(pointer, res_p);

      Value *sum = builder.CreateAdd(pointer, arg1);
      builder.CreateStore(sum, pointer);

      continue;
    }

    // IR implementation for SETI instruction
    if (Instructions[PC]->m_ID == SETI) {
      // res
      Value *arg1_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                Instructions[PC]->m_rs1);
      // arg1
      Value *arg2_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs2);
      // arg2
      Value *arg3 = builder.getInt32(Instructions[PC]->m_imm);

      Value *sum = builder.CreateAdd(builder.CreateLoad(builder.getInt32Ty(), arg1_p),
        builder.CreateLoad(builder.getInt32Ty(), arg2_p));

      Value *storage_pointer = builder.CreateConstInBoundsGEP2_64(storageFileType, storageFile, 0, 0);
      Value *sum64 = builder.CreateSExt(sum, Type::getInt64Ty(context));
      Value *pointer = builder.CreateInBoundsGEP(Type::getInt32Ty(context), storage_pointer, sum64);
      builder.CreateStore(arg3, pointer);
      continue;
    }

    // IR implementation for GET instruction
    if (Instructions[PC]->m_ID == GET) {
      // res
      Value *res_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                Instructions[PC]->m_rs1);
      // arg1
      Value *arg1_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs2);
      // arg2
      Value *arg2_p = builder.CreateConstGEP2_32(regFileType, regFile, 0,
                                                 Instructions[PC]->m_rs3);
      Value *sum = builder.CreateAdd(builder.CreateLoad(builder.getInt32Ty(), arg1_p),
       builder.CreateLoad(builder.getInt32Ty(), arg2_p));

      Value *storage_pointer = builder.CreateConstInBoundsGEP2_64(storageFileType, storageFile, 0, 0);
      Value *sum64 = builder.CreateSExt(sum, Type::getInt64Ty(context));
      Value *pointer = builder.CreateInBoundsGEP(Type::getInt32Ty(context), storage_pointer, sum64);
      builder.CreateStore(builder.CreateLoad(builder.getInt32Ty(), pointer), res_p);
      continue;
    }

    // IR implementation for GET instruction
    if (Instructions[PC]->m_ID == CALL) {
      std::string f_name = Instructions[PC]->m_label;
      if (f_name == "simFlush") {
        builder.CreateCall(simFlushFunc);
      }
      if (f_name == "simPutPixel") {
        // arg1
        Value *arg1_p = builder.CreateConstGEP2_32(regFileType, regFile, 0, 0);
        // arg1
        Value *arg2_p = builder.CreateConstGEP2_32(regFileType, regFile, 0, 1);
        // arg2
        Value *arg3_p = builder.CreateConstGEP2_32(regFileType, regFile, 0, 2);
        Value *args[] = {builder.CreateLoad(builder.getInt32Ty(), arg1_p),
                         builder.CreateLoad(builder.getInt32Ty(), arg2_p),
                         builder.CreateLoad(builder.getInt32Ty(), arg3_p)};
        builder.CreateCall(simPutPixelFunc, args);
      }
      if (f_name == "simRand") {
        Value *res_p = builder.CreateConstGEP2_32(regFileType, regFile, 0, 0);
        Value *res = builder.CreateCall(simRandFunc);
        builder.CreateStore(res, res_p);
      }
      continue;
    }

    // Get poointer to instruction for function args
    Value *instr_p = builder.getInt64((uint64_t)Instructions[PC]);
    // Call simulation function for other instructions
    builder.CreateCall(module->getOrInsertFunction(
                           "do_" + Instructions[PC]->m_name, CalleType),
                       ArrayRef<Value *>({cpu_p, instr_p}));
  }
  // ret i32 0
  // builder.CreateRet(builder.getInt32(0));

  outs() << "#[LLVM IR] DUMP\n";
  module->print(outs(), nullptr);
  outs() << "#[LLVM IR] END\n";
  for (int i = 0; i < REG_FILE_SIZE; i++) {
    cpu.REG_FILE[i] = 0;
  }

  // App simulation with execution engine
  outs() << "#[LLVM EE] RUN\n";
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  ExecutionEngine *ee = EngineBuilder(std::unique_ptr<Module>(module)).create();
  ee->InstallLazyFunctionCreator(lazyFunctionCreator);
  ee->addGlobalMapping(regFile, (void *)cpu.REG_FILE);
  ee->finalizeObject();
  ArrayRef<GenericValue> noargs;

  cpu.RUN = 1;
  cpu.PC = 1;
  ee->runFunction(mainFunc, noargs);
  outs() << "#[LLVM EE] END\n";

  // Registers dump after simulation with EE
  for (int i = 0; i < REG_FILE_SIZE; i++) {
    outs() << "[" << i << "] " << cpu.REG_FILE[i] << "\n";
  }

  Instructions.clear();
  return 0;
}