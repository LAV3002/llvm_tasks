#include "sim.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#define FRAME_TICKS 500

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

int main() {
  LLVMContext context;
  // ; ModuleID = 'app.c'
  // source_filename = "app.c"
  Module *module = new Module("app.c", context);
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

  // define dso_local void @drawShape() {
  ArrayRef<Type *> drawShapeParamTypes = {Type::getInt32Ty(context),
                                            Type::getInt32Ty(context),
                                            Type::getInt32Ty(context)};
  FunctionType *drawShapeFuncType = FunctionType::get(voidType, drawShapeParamTypes, false);
  Function *drawShapeFunc =
      Function::Create(drawShapeFuncType, Function::ExternalLinkage, "draw_shape", module);
  drawShapeFunc->setDSOLocal(true);

  // BasicBlocks:

  BasicBlock *BB331 = BasicBlock::Create(context, "", drawShapeFunc);
  BasicBlock *BB11 = BasicBlock::Create(context, "", drawShapeFunc);
  BasicBlock *BB17 = BasicBlock::Create(context, "", drawShapeFunc);
  BasicBlock *BB20 = BasicBlock::Create(context, "", drawShapeFunc);
  BasicBlock *BB26 = BasicBlock::Create(context, "", drawShapeFunc);
  BasicBlock *BB30 = BasicBlock::Create(context, "", drawShapeFunc);
  BasicBlock *BB33 = BasicBlock::Create(context, "", drawShapeFunc);
  BasicBlock *BB34 = BasicBlock::Create(context, "", drawShapeFunc);
  BasicBlock *BB37 = BasicBlock::Create(context, "", drawShapeFunc);

  builder.SetInsertPoint(BB331);
  Value *val4 = builder.CreateAlloca(builder.getInt32Ty());
  Value *val5 = builder.CreateAlloca(builder.getInt32Ty());
  Value *val6 = builder.CreateAlloca(builder.getInt32Ty());
  Value *val7 = builder.CreateAlloca(builder.getInt32Ty());
  Value *val8 = builder.CreateAlloca(builder.getInt32Ty());
  builder.CreateStore(drawShapeFunc->getArg(0), val4);
  builder.CreateStore(drawShapeFunc->getArg(1), val5);
  builder.CreateStore(drawShapeFunc->getArg(2), val6);
  builder.CreateStore(builder.getInt32(0), val7);
  builder.CreateStore(builder.getInt32(0), val8);
  Value *val9 = builder.CreateLoad(builder.getInt32Ty(), val4);
  Value *val10 = builder.CreateMul(val9, builder.getInt32(10), "", false, true);
  builder.CreateStore(val10, val7);
  builder.CreateBr(BB11);

  // 11:
  builder.SetInsertPoint(BB11);
  Value *val12 = builder.CreateLoad(builder.getInt32Ty(), val7);
  Value *val13 = builder.CreateLoad(builder.getInt32Ty(), val4);
  Value *val14 = builder.CreateAdd(val13, builder.getInt32(1), "", false, true);
  Value *val15 = builder.CreateMul(val14, builder.getInt32(10), "", false, true);
  Value *val16 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_ULT, val12, val15);
  builder.CreateCondBr(val16, BB17, BB37);

  // 17:
  builder.SetInsertPoint(BB17);
  Value *val18 = builder.CreateLoad(builder.getInt32Ty(), val5);
  Value *val19 = builder.CreateMul(val18, builder.getInt32(10), "", false, true);
  builder.CreateStore(val19, val8);
  builder.CreateBr(BB20);

  // 20:
  builder.SetInsertPoint(BB20);
  Value *val21 = builder.CreateLoad(builder.getInt32Ty(), val8);
  Value *val22 = builder.CreateLoad(builder.getInt32Ty(), val5);
  Value *val23 = builder.CreateMul(val22, builder.getInt32(1), "", false, true);
  Value *val24 = builder.CreateMul(val23, builder.getInt32(10), "", false, true);
  Value *val25 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_ULT, val21, val24);
  builder.CreateCondBr(val25, BB26, BB33);

  // 26:
  builder.SetInsertPoint(BB26);
  Value *val27 = builder.CreateLoad(builder.getInt32Ty(), val7);
  Value *val28 = builder.CreateLoad(builder.getInt32Ty(), val8);
  Value *val29 = builder.CreateLoad(builder.getInt32Ty(), val6);
  Value *args[] = {val27, val28, val29};
  builder.CreateCall(simPutPixelFunc, args);
  builder.CreateBr(BB30);

  // 30:
  builder.SetInsertPoint(BB30);
  Value *val31 = builder.CreateLoad(builder.getInt32Ty(), val8);
  Value *val32 = builder.CreateAdd(val31, builder.getInt32(1), "", false, false);
  builder.CreateStore(val32, val8);
  builder.CreateBr(BB20);

  // 33:
  builder.SetInsertPoint(BB33);
  builder.CreateBr(BB34);

  // 34:
  builder.SetInsertPoint(BB34);
  Value *val35 = builder.CreateLoad(builder.getInt32Ty(), val7);
  Value *val36 = builder.CreateAdd(val35, builder.getInt32(1), "", false, false);
  builder.CreateStore(val36, val7);
  builder.CreateBr(BB11);

  // 37:
  builder.SetInsertPoint(BB37);
  builder.CreateRetVoid();

  // define dso_local void @drawGen() {
  ArrayRef<Type *> drawGenParamTypes = {Type::getInt32PtrTy(context)};
  FunctionType *drawGenFuncType = FunctionType::get(voidType, drawGenParamTypes, false);
  Function *drawGenFunc =
      Function::Create(drawGenFuncType, Function::ExternalLinkage, "drawGen", module);
  drawGenFunc->setDSOLocal(true);

  BasicBlock *BB332 = BasicBlock::Create(context, "", drawGenFunc);
  BasicBlock *BB5 = BasicBlock::Create(context, "", drawGenFunc);
  BasicBlock *BB8 = BasicBlock::Create(context, "", drawGenFunc);
  BasicBlock *BB9 = BasicBlock::Create(context, "", drawGenFunc);
  BasicBlock *BB12 = BasicBlock::Create(context, "", drawGenFunc);
  BasicBlock *BB25 = BasicBlock::Create(context, "", drawGenFunc);
  BasicBlock *BB28 = BasicBlock::Create(context, "", drawGenFunc);
  BasicBlock *BB29 = BasicBlock::Create(context, "", drawGenFunc);
  BasicBlock *BB32 = BasicBlock::Create(context, "", drawGenFunc);

  builder.SetInsertPoint(BB332);
  Value *val2 = builder.CreateAlloca(Type::getInt32PtrTy(context));
  Value *val3 = builder.CreateAlloca(builder.getInt32Ty());
  val4 = builder.CreateAlloca(builder.getInt32Ty());
  builder.CreateStore(drawGenFunc->getArg(0), val2);
  builder.CreateStore(builder.getInt32(0), val3);
  builder.CreateStore(builder.getInt32(0), val4);
  builder.CreateStore(builder.getInt32(0), val4);
  builder.CreateBr(BB5);

  // 5:
  builder.SetInsertPoint(BB5);
  val6 = builder.CreateLoad(builder.getInt32Ty(), val4);
  val7 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_ULT, val6, builder.getInt32(50));
  builder.CreateCondBr(val7, BB8, BB32);

  // 8:
  builder.SetInsertPoint(BB8);
  builder.CreateStore(builder.getInt32(0), val3);
  builder.CreateBr(BB9);

  // 9:
  builder.SetInsertPoint(BB9);
  val10 = builder.CreateLoad(builder.getInt32Ty(), val3);
  Value *val11 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_ULT, val10, builder.getInt32(50));
  builder.CreateCondBr(val11, BB12, BB28);

  // 12:
  builder.SetInsertPoint(BB12);
  val13 = builder.CreateLoad(builder.getInt32Ty(), val3);
  val14 = builder.CreateLoad(builder.getInt32Ty(), val4);
  val15 = builder.CreateLoad(Type::getInt32PtrTy(context), val2);
  val16 = builder.CreateLoad(builder.getInt32Ty(), val4);
  Value *val17 = builder.CreateMul(val16, builder.getInt32(50), "", false, false);
  val18 = builder.CreateLoad(builder.getInt32Ty(), val3);
  val19 = builder.CreateAdd(val17, val18, "", false, false);
  Value *val20 = builder.CreateZExt(val19, Type::getInt64Ty(context));
  val21 = builder.CreateInBoundsGEP(Type::getInt32Ty(context), val15, val20);
  val22 = builder.CreateLoad(builder.getInt32Ty(), val21);
  val23 = builder.CreateMul(builder.getInt32(65280), val22, "", false, false);
  val24 = builder.CreateAdd(builder.getInt32(-16777216), val23, "", false, false);
  Value *args1[] = {val13, val14, val24};
  builder.CreateCall(drawShapeFunc, args1);
  builder.CreateBr(BB25);

  // 25:
  builder.SetInsertPoint(BB25);
  Value *val26 = builder.CreateLoad(builder.getInt32Ty(), val3);
  val27 = builder.CreateAdd(val26, builder.getInt32(1), "", false, false);
  builder.CreateStore(val27, val3);
  builder.CreateBr(BB9);

  // 28:
  builder.SetInsertPoint(BB28);
  builder.CreateBr(BB29);

  // 29:
  builder.SetInsertPoint(BB29);
  Value *val30 = builder.CreateLoad(builder.getInt32Ty(), val4);
  val31 = builder.CreateAdd(val30, builder.getInt32(1), "", false, false);
  builder.CreateStore(val31, val4);
  builder.CreateBr(BB5);

  // 32:
  builder.SetInsertPoint(BB32);
  builder.CreateRetVoid();

  // define dso_local void @calcGen(){
  ArrayRef<Type *> calcGenParamTypes = {Type::getInt32PtrTy(context), Type::getInt32PtrTy(context)};
  FunctionType *calcGenFuncType = FunctionType::get(voidType, calcGenParamTypes, false);
  Function *calcGenFunc =
      Function::Create(calcGenFuncType, Function::ExternalLinkage, "calcGen", module);
  calcGenFunc->setDSOLocal(true);

  BasicBlock *BB333 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB10 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB13 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB14 = BasicBlock::Create(context, "", calcGenFunc);
  BB17 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB18 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB21 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB22 = BasicBlock::Create(context, "", calcGenFunc);
  BB25 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB44 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB47 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB48 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB51 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB71 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB74 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB77 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB85 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB93 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB94 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB97 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB105 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB113 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB114 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB115 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB118 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB119 = BasicBlock::Create(context, "", calcGenFunc);
  BasicBlock *BB122 = BasicBlock::Create(context, "", calcGenFunc);

  builder.SetInsertPoint(BB333);
  val3 = builder.CreateAlloca(Type::getInt32PtrTy(context));
  val4 = builder.CreateAlloca(Type::getInt32PtrTy(context));
  val5 = builder.CreateAlloca(builder.getInt32Ty());
  val6 = builder.CreateAlloca(builder.getInt32Ty());
  val7 = builder.CreateAlloca(builder.getInt32Ty());
  val8 = builder.CreateAlloca(builder.getInt32Ty());
  val9 = builder.CreateAlloca(builder.getInt32Ty());
  builder.CreateStore(calcGenFunc->getArg(0), val3);
  builder.CreateStore(calcGenFunc->getArg(1), val4);
  builder.CreateStore(builder.getInt32(0), val5);
  builder.CreateStore(builder.getInt32(0), val6);
  builder.CreateStore(builder.getInt32(0), val7);
  builder.CreateStore(builder.getInt32(0), val8);
  builder.CreateStore(builder.getInt32(0), val9);
  builder.CreateStore(builder.getInt32(0), val6);
  builder.CreateBr(BB10);

  // 10:
  builder.SetInsertPoint(BB10);
  val11 = builder.CreateLoad(builder.getInt32Ty(), val6);
  val12 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SLT, val11, builder.getInt32(50));
  builder.CreateCondBr(val12, BB13, BB122);

  // 13:
  builder.SetInsertPoint(BB13);
  builder.CreateStore(builder.getInt32(0), val5);
  builder.CreateBr(BB14);

  // 14:
  builder.SetInsertPoint(BB14);
  val15 = builder.CreateLoad(builder.getInt32Ty(), val5);
  val16 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SLT, val15, builder.getInt32(50));
  builder.CreateCondBr(val16, BB17, BB118);

  // 17:
  builder.SetInsertPoint(BB17);
  builder.CreateStore(builder.getInt32(0), val9);
  builder.CreateStore(builder.getInt32(-1), val7);
  builder.CreateBr(BB18);

  // 18:
  builder.SetInsertPoint(BB18);
  val19 = builder.CreateLoad(builder.getInt32Ty(), val7);
  val20 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SLT, val19, builder.getInt32(2));
  builder.CreateCondBr(val20, BB21, BB51);

  // 21:
  builder.SetInsertPoint(BB21);
  builder.CreateStore(builder.getInt32(-1), val8);
  builder.CreateBr(BB22);

  // 22:
  builder.SetInsertPoint(BB22);
  val23 = builder.CreateLoad(builder.getInt32Ty(), val8);
  val24 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SLT, val23, builder.getInt32(2));
  builder.CreateCondBr(val24, BB25, BB47);

  // 25:
  builder.SetInsertPoint(BB25);
  val26 = builder.CreateLoad(Type::getInt32PtrTy(context), val4);
  val27 = builder.CreateLoad(builder.getInt32Ty(), val6);
  val28 = builder.CreateLoad(builder.getInt32Ty(), val8);
  val29 = builder.CreateAdd(val27, val28, "", false, true);
  val30 = builder.CreateAdd(val29, builder.getInt32(50), "", false, true);
  val31 = builder.CreateSRem(val30, builder.getInt32(50));
  val32 = builder.CreateMul(val31, builder.getInt32(50), "", false, true);
  Value *val33 = builder.CreateLoad(builder.getInt32Ty(), val5);
  Value *val34 = builder.CreateLoad(builder.getInt32Ty(), val7);
  val35 = builder.CreateAdd(val33, val34, "", false, true);
  val36 = builder.CreateAdd(val35, builder.getInt32(50), "", false, true);
  Value *val37 = builder.CreateSRem(val36, builder.getInt32(50));
  Value *val38 = builder.CreateAdd(val32, val37, "", false, true);
  Value *val39 = builder.CreateSExt(val38, Type::getInt64Ty(context));
  Value *val40 = builder.CreateInBoundsGEP(Type::getInt32Ty(context), val26, val39);
  Value *val41 = builder.CreateLoad(builder.getInt32Ty(), val40);
  Value *val42 = builder.CreateLoad(builder.getInt32Ty(), val9);
  Value *val43 = builder.CreateAdd(val42, val41, "", false, false);
  builder.CreateStore(val43, val9);
  builder.CreateBr(BB44);

  // 44:
  builder.SetInsertPoint(BB44);
  Value *val45 = builder.CreateLoad(builder.getInt32Ty(), val8);
  Value *val46 = builder.CreateAdd(val45, builder.getInt32(1), "", false, true);
  builder.CreateStore(val46, val8);
  builder.CreateBr(BB22);

  // 47:
  builder.SetInsertPoint(BB47);
  builder.CreateBr(BB48);

  // 48:
  builder.SetInsertPoint(BB48);
  Value *val49 = builder.CreateLoad(builder.getInt32Ty(), val7);
  Value *val50  = builder.CreateAdd(val49, builder.getInt32(1), "", false, true);
  builder.CreateStore(val50, val7);
  builder.CreateBr(BB18);

 // 51:
  builder.SetInsertPoint(BB51);
  Value *val52 = builder.CreateLoad(Type::getInt32PtrTy(context), val4);
  Value *val53 = builder.CreateLoad(builder.getInt32Ty(), val6);
  Value *val54 = builder.CreateMul(val53, builder.getInt32(50), "", false, true);
  Value *val55 = builder.CreateLoad(builder.getInt32Ty(), val5);
  Value *val56 = builder.CreateAdd(val54, val55, "", false, true);
  Value *val57 = builder.CreateSExt(val56, Type::getInt64Ty(context));
  Value *val58 = builder.CreateInBoundsGEP(Type::getInt32Ty(context), val52, val57);
  Value *val59= builder.CreateLoad(builder.getInt32Ty(), val58);
  Value *val60 = builder.CreateLoad(builder.getInt32Ty(), val9);
  Value *val61 = builder.CreateSub(val60, val59, "", false, false);
  builder.CreateStore(val61, val9);
  Value *val62 = builder.CreateLoad(Type::getInt32PtrTy(context), val4);
  Value *val63 = builder.CreateLoad(builder.getInt32Ty(), val6);
  Value *val64 = builder.CreateMul(val63, builder.getInt32(50), "", false, true);
  Value *val65 = builder.CreateLoad(builder.getInt32Ty(), val5);
  Value *val66 = builder.CreateAdd(val64, val65, "", false, true);
  Value *val67 = builder.CreateSExt(val66, Type::getInt64Ty(context));
  Value *val68 = builder.CreateInBoundsGEP(Type::getInt32Ty(context), val62, val67);
  Value *val69 = builder.CreateLoad(builder.getInt32Ty(), val68);
  Value *val70 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_NE, val69, builder.getInt32(0));
  builder.CreateCondBr(val70, BB71, BB94);

  // 71:
  builder.SetInsertPoint(BB71);
  Value* val72 = builder.CreateLoad(builder.getInt32Ty(), val9);
  Value* val73 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, val72, builder.getInt32(2));
  builder.CreateCondBr(val73, BB77, BB74);

  // 74:
  builder.SetInsertPoint(BB74);
  Value* val75 = builder.CreateLoad(builder.getInt32Ty(), val9);
  Value* val76 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, val75, builder.getInt32(3));
  builder.CreateCondBr(val76, BB77, BB85);

  // 77:
  builder.SetInsertPoint(BB77);
  Value *val78 = builder.CreateLoad(Type::getInt32PtrTy(context), val3);
  Value *val79 = builder.CreateLoad(builder.getInt32Ty(), val6);
  Value *val80 = builder.CreateMul(val79, builder.getInt32(50), "", false, true);
  Value *val81 = builder.CreateLoad(builder.getInt32Ty(), val5);
  Value *val82 = builder.CreateAdd(val80, val81, "", false, true);
  Value *val83 = builder.CreateSExt(val82, Type::getInt64Ty(context));
  Value *val84 = builder.CreateInBoundsGEP(Type::getInt32Ty(context), val78, val83);
  builder.CreateStore(builder.getInt32(1), val84);
  builder.CreateBr(BB93);

  // 85:
  builder.SetInsertPoint(BB85);
  Value *val86 = builder.CreateLoad(Type::getInt32PtrTy(context), val3);
  Value *val87 = builder.CreateLoad(builder.getInt32Ty(), val6);
  Value *val88 = builder.CreateMul(val87, builder.getInt32(50), "", false, true);
  Value *val89 = builder.CreateLoad(builder.getInt32Ty(), val5);
  Value *val90 = builder.CreateAdd(val88, val89, "", false, true);
  Value *val91 = builder.CreateSExt(val90, Type::getInt64Ty(context));
  Value *val92 = builder.CreateInBoundsGEP(Type::getInt32Ty(context), val86, val91);
  builder.CreateStore(builder.getInt32(0), val92);
  builder.CreateBr(BB93);

  // 93:
  builder.SetInsertPoint(BB93);
  builder.CreateBr(BB114);

  // 94:
  builder.SetInsertPoint(BB94);
  Value* val95 = builder.CreateLoad(builder.getInt32Ty(), val9);
  Value* val96 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, val95, builder.getInt32(3));
  builder.CreateCondBr(val96, BB97, BB105);

  // 97:
  builder.SetInsertPoint(BB97);
  Value *val98 = builder.CreateLoad(Type::getInt32PtrTy(context), val3);
  Value *val99 = builder.CreateLoad(builder.getInt32Ty(), val6);
  Value *val100 = builder.CreateMul(val99, builder.getInt32(50), "", false, true);
  Value *val101 = builder.CreateLoad(builder.getInt32Ty(), val5);
  Value *val102 = builder.CreateAdd(val100, val101, "", false, true);
  Value *val103 = builder.CreateSExt(val102, Type::getInt64Ty(context));
  Value *val104 = builder.CreateInBoundsGEP(Type::getInt32Ty(context), val98, val103);
  builder.CreateStore(builder.getInt32(1), val104);
  builder.CreateBr(BB113);

  // 105:
  builder.SetInsertPoint(BB105);
  Value *val106 = builder.CreateLoad(Type::getInt32PtrTy(context), val3);
  Value *val107 = builder.CreateLoad(builder.getInt32Ty(), val6);
  Value *val108 = builder.CreateMul(val107, builder.getInt32(50), "", false, true);
  Value *val109 = builder.CreateLoad(builder.getInt32Ty(), val5);
  Value *val110 = builder.CreateAdd(val108, val109, "", false, true);
  Value *val111 = builder.CreateSExt(val110, Type::getInt64Ty(context));
  Value *val112 = builder.CreateInBoundsGEP(Type::getInt32Ty(context), val106, val111);
  builder.CreateStore(builder.getInt32(0), val112);
  builder.CreateBr(BB113);

  // 113:
  builder.SetInsertPoint(BB113);
  builder.CreateBr(BB114);

  // 114:
  builder.SetInsertPoint(BB114);
  builder.CreateBr(BB115);

  // 115:
  builder.SetInsertPoint(BB115);
  Value *val116 = builder.CreateLoad(builder.getInt32Ty(), val5);
  Value *val117  = builder.CreateAdd(val116, builder.getInt32(1), "", false, true);
  builder.CreateStore(val117, val5);
  builder.CreateBr(BB14);

  // 118:
  builder.SetInsertPoint(BB118);
  builder.CreateBr(BB119);

  // 119:
  builder.SetInsertPoint(BB119);
  Value *val120 = builder.CreateLoad(builder.getInt32Ty(), val6);
  Value *val121  = builder.CreateAdd(val120, builder.getInt32(1), "", false, true);
  builder.CreateStore(val121, val6);
  builder.CreateBr(BB10);

  // 122:
  builder.SetInsertPoint(BB122);
  builder.CreateRetVoid();



  // define dso_local void @app() {
  FunctionType *appFuncType = FunctionType::get(builder.getInt32Ty(), false);
  Function *appFunc =
      Function::Create(appFuncType, Function::ExternalLinkage, "app", module);
  appFunc->setDSOLocal(true);

  // BasicBlocks:
  BasicBlock *BB334 = BasicBlock::Create(context, "", appFunc);
  BB11 = BasicBlock::Create(context, "", appFunc);
  BB14 = BasicBlock::Create(context, "", appFunc);
  BasicBlock *BB15 = BasicBlock::Create(context, "", appFunc);
  BB18 = BasicBlock::Create(context, "", appFunc);
  BB30 = BasicBlock::Create(context, "", appFunc);
  BB33 = BasicBlock::Create(context, "", appFunc);
  BB34 = BasicBlock::Create(context, "", appFunc);
  BB37 = BasicBlock::Create(context, "", appFunc);
  BasicBlock *BB38 = BasicBlock::Create(context, "", appFunc);
  BasicBlock *BB41 = BasicBlock::Create(context, "", appFunc);
  BB48 = BasicBlock::Create(context, "", appFunc);
  BB51 = BasicBlock::Create(context, "", appFunc);

  builder.SetInsertPoint(BB334);
  Value *val1 = builder.CreateAlloca(ArrayType::get(Type::getInt32Ty(context), 2500));
  val2 = builder.CreateAlloca(ArrayType::get(Type::getInt32Ty(context), 2500));
  val3 = builder.CreateAlloca(Type::getInt32PtrTy(context));
  val4 = builder.CreateAlloca(Type::getInt32PtrTy(context));
  val5 = builder.CreateAlloca(builder.getInt32Ty());
  val6 = builder.CreateAlloca(builder.getInt32Ty());
  val7 = builder.CreateAlloca(builder.getInt32Ty());
  val8 = builder.CreateAlloca(Type::getInt32PtrTy(context));
  val9 = builder.CreateConstInBoundsGEP2_64(ArrayType::get(Type::getInt32Ty(context), 2500), val1, 0, 0);
  builder.CreateStore(val9, val3);
  val10 = builder.CreateConstInBoundsGEP2_64(ArrayType::get(Type::getInt32Ty(context), 2500), val2, 0, 0);
  builder.CreateStore(val10, val4);
  builder.CreateStore(builder.getInt32(0), val5);
  builder.CreateBr(BB11);

  // 11:
  builder.SetInsertPoint(BB11);
  val12 = builder.CreateLoad(builder.getInt32Ty(), val5);
  val13 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SLT, val12, builder.getInt32(50));
  builder.CreateCondBr(val13, BB14, BB37);

  // 14:
  builder.SetInsertPoint(BB14);
  builder.CreateStore(builder.getInt32(0), val6);
  builder.CreateBr(BB15);

  // 15:
  builder.SetInsertPoint(BB15);
  val16 = builder.CreateLoad(builder.getInt32Ty(), val6);
  val17 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SLT, val16, builder.getInt32(50));
  builder.CreateCondBr(val17, BB18, BB33);

  // 18:
  builder.SetInsertPoint(BB18);
  val19 = builder.CreateCall(simRandFunc);
  val20 = builder.CreateSRem(val19, builder.getInt32(6));
  val21 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, val20, builder.getInt32(0));
  val22 = builder.CreateZExt(val21, Type::getInt32Ty(context));
  val23 = builder.CreateLoad(Type::getInt32PtrTy(context), val4);
  val24 = builder.CreateLoad(Type::getInt32Ty(context), val6);
  val25 = builder.CreateMul(val24, builder.getInt32(50), "", false, true);
  val26 = builder.CreateLoad(Type::getInt32Ty(context), val5);
  val27 = builder.CreateAdd(val25, val26, "", false, true);
  val28 = builder.CreateSExt(val27, Type::getInt64Ty(context));
  val29 = builder.CreateInBoundsGEP(Type::getInt32Ty(context), val23, val28);
  builder.CreateStore(val22, val29);
  builder.CreateBr(BB30);

  // 30:
  builder.SetInsertPoint(BB30);
  val31 = builder.CreateLoad(builder.getInt32Ty(), val6);
  val32 = builder.CreateAdd(val31, builder.getInt32(1), "", false, true);
  builder.CreateStore(val32, val6);
  builder.CreateBr(BB15);

  // 33:
  builder.SetInsertPoint(BB33);
  builder.CreateBr(BB34);

  // 34:
  builder.SetInsertPoint(BB34);
  val35 = builder.CreateLoad(builder.getInt32Ty(), val5);
  val36 = builder.CreateAdd(val35, builder.getInt32(1), "", false, true);
  builder.CreateStore(val36, val5);
  builder.CreateBr(BB11);

  // 37:
  builder.SetInsertPoint(BB37);
  builder.CreateCall(simFlushFunc);
  builder.CreateStore(builder.getInt32(0), val7);
  builder.CreateStore(builder.getInt32(0), val7);
  builder.CreateBr(BB38);

  // 38:
  builder.SetInsertPoint(BB38);
  val39 = builder.CreateLoad(builder.getInt32Ty(), val7);
  val40 = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_ULT, val39, builder.getInt32(2));
  builder.CreateCondBr(val40, BB41, BB51);

  // 41:
  builder.SetInsertPoint(BB41);
  val42 = builder.CreateLoad(Type::getInt32PtrTy(context), val3);
  val43 = builder.CreateLoad(Type::getInt32PtrTy(context), val4);
  Value *args2[] = {val42, val43};
  builder.CreateCall(calcGenFunc, args2);
  Value *val44 = builder.CreateLoad(Type::getInt32PtrTy(context), val3);
  Value *args3[] = {val44};
  builder.CreateCall(drawGenFunc, args3);
  val45 = builder.CreateLoad(Type::getInt32PtrTy(context), val4);
  builder.CreateStore(val45, val8);
  val46 = builder.CreateLoad(Type::getInt32PtrTy(context), val3);
  builder.CreateStore(val46, val4);
  Value *val47 = builder.CreateLoad(Type::getInt32PtrTy(context), val8);
  builder.CreateStore(val47, val3);
  builder.CreateCall(simFlushFunc);
  builder.CreateBr(BB48);

  // 48:
  builder.SetInsertPoint(BB48);
  val49 = builder.CreateLoad(builder.getInt32Ty(), val7);
  val50  = builder.CreateAdd(val49, builder.getInt32(1), "", false, false);
  builder.CreateStore(val50, val7);
  builder.CreateBr(BB38);

 // 51:
  builder.SetInsertPoint(BB51);
  builder.CreateRetVoid();
  


  // Dump LLVM IR
  module->print(outs(), nullptr);

  // Interpreter of LLVM IR
  outs() << "Running code...\n";
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  ExecutionEngine *ee = EngineBuilder(std::unique_ptr<Module>(module)).create();
  ee->InstallLazyFunctionCreator([&](const std::string &fnName) -> void * {
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
  });
  ee->finalizeObject();

  simInit();

  ArrayRef<GenericValue> noargs;
  GenericValue v = ee->runFunction(appFunc, noargs);
  outs() << "Code was run.\n";

  simExit();
  return EXIT_SUCCESS;
}
