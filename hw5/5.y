%{
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/TargetSelect.h"
using namespace llvm;

#define YYSTYPE Value*
extern "C" {
    int yyparse();
    int yylex();
    void yyerror(char *s) {
        std::cerr << s << "\n";
    }
    int yywrap(void){return 1;}
}

#include <SDL2/SDL.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#define GEN_SIZE 50
#define BLOCK_SIZE 10
#define WINDOW_WIDTH GEN_SIZE * BLOCK_SIZE
#define REPEAT_SIZE 300
#define FRAME_TICKS 300

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

LLVMContext context;
IRBuilder<>* builder;
Module* module;
Function *curFunc;
FunctionCallee simFlushFunc;
FunctionCallee simPutPixelFunc;
FunctionCallee simRandFunc;


typedef struct {
    GlobalVariable* irVal;
    int realVal;
} value_t;
std::map<std::string, value_t> GlobalValueMap;
std::map<std::string, Value*> LocalValueMap;
std::vector<Value*> curFuncExprs;
std::vector<std::string> curFuncArgs;


typedef struct {
    GlobalVariable* irVal;
    int size;
    int initVal;
    int* realVal;
} array_t;
std::map<std::string, array_t> ArrayMap;

std::map<std::string, BasicBlock *> BBMap;
std::vector<BasicBlock *> BBStack;

Uint32 BBCounter = 0;

int main(int argc, char **argv)
{
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    // ; ModuleID = 'top'
    // source_filename = "top"
    module = new Module("top", context);
    builder = new IRBuilder<> (context);

    Type *voidType = Type::getVoidTy(context);
    // declare void @simPutPixel(i32 noundef, i32 noundef, i32 noundef)
    ArrayRef<Type *> simPutPixelParamTypes = {Type::getInt32Ty(context),
                                                Type::getInt32Ty(context),
                                                Type::getInt32Ty(context)};
    FunctionType *simPutPixelType =
        FunctionType::get(voidType, simPutPixelParamTypes, false);
    simPutPixelFunc =
        module->getOrInsertFunction("simPutPixel", simPutPixelType);

    // declare void @simFlush(...)
    FunctionType *simFlushType = FunctionType::get(voidType, {voidType}, false);
    simFlushFunc =
        module->getOrInsertFunction("simFlush", simFlushType);

      // declare void @simRand()
    Type *int32Type = Type::getInt32Ty(context);
    FunctionType *simRandType =
        FunctionType::get(int32Type, {voidType}, false);
    simRandFunc =
        module->getOrInsertFunction("simRand", simRandType);

    yyparse();

    outs() << "#[LLVM IR]:\n";
    module->print(outs(), nullptr);

    // Interpreter of LLVM IR
    outs() << "Running code...\n";
	ExecutionEngine *ee = EngineBuilder(std::unique_ptr<Module>(module)).create();
    ee->InstallLazyFunctionCreator([&](const std::string &fnName) -> void * {
        if (fnName == "simFlush") {
            return reinterpret_cast<void *>(simFlush);
        }
        if (fnName == "simPutPixel") {
            return reinterpret_cast<void *>(simPutPixel);
        }
        if (fnName == "simRand") {
            return reinterpret_cast<void *>(simRand);
        }
        return nullptr;
    });

    for (auto& value : GlobalValueMap) {
        ee->addGlobalMapping(value.second.irVal, &value.second.realVal);
    }
    for (auto& array : ArrayMap) {
        array.second.realVal = new int[array.second.size];
        for (int i = 0; i < array.second.size; i++) {
            array.second.realVal[i] = array.second.initVal;
        }
        ee->addGlobalMapping(array.second.irVal, array.second.realVal);
    }

    ee->finalizeObject();

    simInit();

	std::vector<GenericValue> noargs;
    Function *mainFunc = module->getFunction("main");
    if (mainFunc == nullptr) {
	    outs() << "Can't find main\n";
        return -1;
    }
	ee->runFunction(mainFunc, noargs);
	outs() << "Code was run.\n";

    for (auto& value : GlobalValueMap) {
        outs() << value.first << " = " <<  value.second.realVal << "\n";
    }
    for (auto& array : ArrayMap) {
        outs() << array.first << "[" << array.second.size << "] =";
        for (int i = 0; i < array.second.size; i++) {
            outs() << " " << array.second.realVal[i];
        }
        outs() << "\n";
        delete array.second.realVal;
    }

    simExit();
    return 0;
}
%}


%token IntLiteral
%token FunctionBegin
%token FunctionEnd
%token CallFunction
%token Identifier
%token IfToken
%token GotoToken
%token PutToken
%token FlushToken
%token ForToken
%token InToken
%token ElseToken
%token IntToken
%token VoidToken
%token RandToken
%token DefineToken
%token WhileToken

%%

Parse: GlobalVariableDeclarationSection Program {YYACCEPT;}

Program: RoutineDeclaration {} | Program RoutineDeclaration {}

GlobalVariableDeclarationSection : DefineToken '{' GlobalVariableDeclarationSeq '}' { printf("GVDSec\n"); }

GlobalVariableDeclarationSeq : GlobalVariableDeclaration {} | GlobalVariableDeclarationSeq GlobalVariableDeclaration {}

GlobalVariableDeclaration : Identifier '=' IntLiteral ';' {
                            printf("Identifier '=' IntLiteral ';'\n");
                            module->getOrInsertGlobal((char*)$1, builder->getInt32Ty());
                            value_t val;
                            val.irVal = module->getNamedGlobal((char*)$1);
                            val.realVal = atoi((char*)$3);
                            GlobalValueMap.insert({(char*)$1, val});
                        }
                    | Identifier '[' IntLiteral ']''=' IntLiteral ';' {
                            printf("Identifier '[' IntLiteral ']''=' IntLiteral ';'\n");
                            int size = atoi((char*)$3);
                            ArrayType *arrayType = ArrayType::get(builder->getInt32Ty(), size);
                            module->getOrInsertGlobal((char*)$1, arrayType);
                            array_t arr;
                            arr.irVal = module->getNamedGlobal((char*)$1);
                            arr.size = atoi((char*)$3);
                            arr.initVal = atoi((char*)$6);
                            ArrayMap.insert({(char*)$1, arr});
                        }

LocalVariableDeclarationSection : DefineToken '{' LocalVariableDeclarationSeq '}' { printf("LVDSec\n"); }

LocalVariableDeclarationSeq : LocalVariableDeclaration {} | LocalVariableDeclarationSeq LocalVariableDeclaration {}

LocalVariableDeclaration : Identifier '=' IntLiteral ';' {
                            printf("Identifier '=' IntLiteral ';'\n");
                            Value *ptr = builder->CreateAlloca(builder->getInt32Ty());
                            builder->CreateStore(builder->getInt32(atoi((char*)$3)), ptr);
                            LocalValueMap.insert({(char*)$1, ptr});
                        }

RoutineDeclaration : FunctionBegin { LocalValueMap.clear(); curFuncArgs.clear(); } Identifier ArgsList '{' {
                            printf("FunctionBegin Identifier ...\n");
                            // declare void @Identifier()
                            Function *func = module->getFunction((char*)$3);
                            if (func == nullptr) {
                                std::vector<Type *> funcTypesV;
                                funcTypesV.assign(curFuncArgs.size(), Type::getInt32Ty(context));
                                ArrayRef<Type *> funcTypes = funcTypesV;
                                FunctionType *funcType = 
                                                        FunctionType::get(builder->getVoidTy(), funcTypes, false);
                                func = Function::Create(funcType, Function::ExternalLinkage, (char*)$3, module);
                            }
                            curFunc = func;
                            // entry:
                            BasicBlock *entryBB = BasicBlock::Create(context, std::to_string(BBCounter++), curFunc);
                            builder->SetInsertPoint(entryBB);
                            LocalValueMap.clear();
                            for (int i = 0; i < curFuncArgs.size(); i++) {
                                Value *ptr = builder->CreateAlloca(builder->getInt32Ty());
                                builder->CreateStore(curFunc->getArg(i), ptr);
                                LocalValueMap.insert({curFuncArgs[i], ptr});
                            }
                        } LocalVariableDeclarationSection {  
                        } Statements '}' {
                            printf("... Statements FunctionEnd\n");
                            builder->CreateRetVoid();
                        }

Statements: StatementsP {printf("Assignment\n");}
            | Statements StatementsP {printf("Statements\n");}

StatementsP: Assignment {printf("Assignment\n");}
            | RoutineCall {printf("RoutineCall\n");}
            | IfStatement {printf("IfStatement\n");}
            | GoTo {printf("GoTo\n");}
            | Label {printf("Label\n");}
            | Put {printf("Put\n");}
            | Flush {printf("Flush\n");}
            | ForStatment {printf("For\n");}
            | CoolIfStatement {printf("CoolIf\n");}
            | WhileStatment {printf("While\n");}

Put : PutToken '('Expression','Expression','Expression')' ';' {
                            Value *args[] = {$3, $5, $7};
                            builder->CreateCall(simPutPixelFunc, args); }

Flush : FlushToken ';' { builder->CreateCall(simFlushFunc); }

Assignment: Value '=' Expression ';' { printf("Value '=' Expression ';'\n"); builder->CreateStore($3, $1); }

RoutineCall: CallFunction { curFuncExprs.clear(); } Identifier ExprList ';' {
                            Function *func = module->getFunction((char*)$3);
                            builder->CreateCall(func, curFuncExprs);
                        }

IfStatement: IfToken Expression '|' Identifier '|' Identifier ';' {
                            if (BBMap.find((char*)$4) == BBMap.end()) {
                                BBMap.insert({(char*)$4, BasicBlock::Create(context, (char*)$4, curFunc)});
                            }
                            if (BBMap.find((char*)$6) == BBMap.end()) {
                                BBMap.insert({(char*)$6, BasicBlock::Create(context, (char*)$6, curFunc)});
                            }
                            Value *cond = builder->CreateICmpNE($2, builder->getInt32(0));
                            builder->CreateCondBr(cond, BBMap[(char*)$4], BBMap[(char*)$6]);
                        }

CoolIfStatement: IfToken '(' Expression ')' '{' {
                            BasicBlock *BB1 = BasicBlock::Create(context, std::to_string(BBCounter), curFunc);
                            BBMap.insert({std::to_string(BBCounter++), BB1});
                            BasicBlock *BB2 = BasicBlock::Create(context, std::to_string(BBCounter), curFunc);
                            BBMap.insert({std::to_string(BBCounter++), BB2});
                            BasicBlock *BB3 = BasicBlock::Create(context, std::to_string(BBCounter), curFunc);
                            BBMap.insert({std::to_string(BBCounter++), BB3});
                            Value *cond = builder->CreateICmpNE($3, builder->getInt32(0));
                            builder->CreateCondBr(cond, BB1, BB2);
                            builder->SetInsertPoint(BB1);
                            BBStack.push_back(BB3);
                            BBStack.push_back(BB3);
                            BBStack.push_back(BB2);
                            BBStack.push_back(BB3);
                        } Statements '}' {
                            builder->CreateBr(BBStack.back());
                            BBStack.pop_back();
                            builder->SetInsertPoint(BBStack.back());
                            BBStack.pop_back();
                        } ElseToken '{' Statements '}' {
                            builder->CreateBr(BBStack.back());
                            BBStack.pop_back();
                            builder->SetInsertPoint(BBStack.back());
                            BBStack.pop_back();
                        }

GoTo:  GotoToken Identifier ';' {
                            if (BBMap.find((char*)$2) == BBMap.end()) {
                                BBMap.insert({(char*)$2, BasicBlock::Create(context, (char*)$2, curFunc)});
                            }
                            BasicBlock *BB = BBMap[(char*)$2];
                            builder->CreateBr(BB);
                        }


Label: Identifier ':'   {
                            if (BBMap.find((char*)$1) == BBMap.end()) {
                                BBMap.insert({(char*)$1, BasicBlock::Create(context, (char*)$1, curFunc)});
                            }
                            BasicBlock *BB = BBMap[(char*)$1];
                            builder->SetInsertPoint(BB);
                        }

ForStatment: ForToken Value InToken '[' Expression ',' Expression ']' '{' {
                            BasicBlock *BB = BasicBlock::Create(context, std::to_string(BBCounter), curFunc);
                            BBMap.insert({std::to_string(BBCounter++), BB});
                            BasicBlock *BB1 = BasicBlock::Create(context, std::to_string(BBCounter), curFunc);
                            BBMap.insert({std::to_string(BBCounter++), BB1});
                            BasicBlock *BB2 = BasicBlock::Create(context, std::to_string(BBCounter), curFunc);
                            BBMap.insert({std::to_string(BBCounter++), BB2});
                            BBStack.push_back(BB2);
                            BBStack.push_back(BB);
                            builder->CreateStore($5, $2);
                            builder->CreateBr(BB);
                            builder->SetInsertPoint(BB);
                            Value* iter_var = builder->CreateLoad(builder->getInt32Ty(), $2);
                            Value *cond = builder->CreateICmpSLT(iter_var, $7);
                            builder->CreateCondBr(cond, BB1, BB2);
                            builder->SetInsertPoint(BB1);
                        } Statements '}' {
                            Value* inc = builder->CreateLoad(builder->getInt32Ty(), $2);
                            Value* incp = builder->CreateAdd(inc, builder->getInt32(1));
                            builder->CreateStore(incp, $2);
                            builder->CreateBr(BBStack.back());
                            BBStack.pop_back();
                            builder->SetInsertPoint(BBStack.back());
                            BBStack.pop_back();
                            printf("... Statements}\n");
                        }

WhileStatment: WhileToken '(' { BasicBlock *BB = BasicBlock::Create(context, std::to_string(BBCounter), curFunc);
                                BBMap.insert({std::to_string(BBCounter++), BB});
                                BasicBlock *BB1 = BasicBlock::Create(context, std::to_string(BBCounter), curFunc);
                                BBMap.insert({std::to_string(BBCounter++), BB1});
                                BasicBlock *BB2 = BasicBlock::Create(context, std::to_string(BBCounter), curFunc);
                                BBMap.insert({std::to_string(BBCounter++), BB2});
                                builder->CreateBr(BB);
                                builder->SetInsertPoint(BB);
                                BBStack.push_back(BB2);
                                BBStack.push_back(BB);
                                BBStack.push_back(BB1);
                                BBStack.push_back(BB2);
                                BBStack.push_back(BB1);
                              } Expression ')' '{' {
                                Value *cond = builder->CreateICmpNE($4, builder->getInt32(0));
                                BasicBlock *BB1 = BBStack.back();
                                BBStack.pop_back();
                                BasicBlock *BB2 = BBStack.back();
                                BBStack.pop_back();
                                builder->CreateCondBr(cond, BB1, BB2);
                                builder->SetInsertPoint(BBStack.back());
                                BBStack.pop_back();
                              } Statements '}' {
                                builder->CreateBr(BBStack.back());
                                BBStack.pop_back();
                                builder->SetInsertPoint(BBStack.back());
                                BBStack.pop_back();
                              }


Expression: Booleror
            | Expression '|''|' Booleror { Value *condf = builder->CreateZExt(builder->CreateICmpNE($1, builder->getInt32(0)), builder->getInt32Ty());
                                           Value *conds = builder->CreateZExt(builder->CreateICmpNE($4, builder->getInt32(0)), builder->getInt32Ty()); 
                                           $$ = builder->CreateAdd(condf, conds);}
        
Booleror: Boolerand
            | Booleror '&''&' Boolerand { Value *condf = builder->CreateZExt(builder->CreateICmpNE($1, builder->getInt32(0)), builder->getInt32Ty());
                                          Value *conds = builder->CreateZExt(builder->CreateICmpNE($4, builder->getInt32(0)), builder->getInt32Ty()); 
                                          $$ = builder->CreateMul(condf, conds);}

Boolerand: Simple
            | Boolerand '!''=' Simple { $$ = builder->CreateZExt(builder->CreateICmpNE($1, $4), builder->getInt32Ty()); }
            | Boolerand '=''=' Simple { $$ = builder->CreateZExt(builder->CreateICmpEQ($1, $4), builder->getInt32Ty()); }
            | Boolerand '<'    Simple { $$ = builder->CreateZExt(builder->CreateICmpSLT($1, $3), builder->getInt32Ty()); }
            | Boolerand '<''=' Simple { $$ = builder->CreateZExt(builder->CreateICmpSLE($1, $4), builder->getInt32Ty()); }
            | Boolerand '>'    Simple { $$ = builder->CreateZExt(builder->CreateICmpSGT($1, $3), builder->getInt32Ty()); }
            | Boolerand '>''=' Simple { $$ = builder->CreateZExt(builder->CreateICmpSGE($1, $4), builder->getInt32Ty()); }
;
Simple:     Summand
            | Simple '+' Summand { $$ = builder->CreateAdd($1, $3); }
            | Simple '-' Summand { $$ = builder->CreateSub($1, $3); }

Summand:    Factor
            | Summand '*' Factor  { $$ = builder->CreateMul($1, $3); }
            | Summand '/' Factor  { $$ = builder->CreateSDiv($1, $3); }
            | Summand '%' Factor  { $$ = builder->CreateSRem($1, $3); }
;

Factor:     Primary { $$ = $1; }
            | '-' Primary { $$ = builder->CreateNeg($2); }
            | '(' Expression ')' { $$ =$2; }
;

Primary:    IntLiteral { $$ = builder->getInt32(atoi((char*)$1)); }
            | Value { $$ = builder->CreateLoad(builder->getInt32Ty(), $1); }
            | RandToken { Value *res = builder->CreateCall(simRandFunc); $$ = res; }
;

Value:      Identifier  {
                            Value *ptr;
                            if (LocalValueMap.find((char*)$1) == LocalValueMap.end()) {
                                ptr = builder->CreateConstGEP1_32(builder->getInt32Ty(), GlobalValueMap[(char*)$1].irVal, 0);
                            } else {
                                ptr = LocalValueMap[(char*)$1];
                            }
                            $$ = ptr;
                        }
            | Identifier '[' Expression ']' {
                            ArrayType *arrayType = ArrayType::get(builder->getInt32Ty(), ArrayMap[(char*)$1].size);
                            std::vector<Value *> gepArgs;
                            gepArgs.push_back(builder->getInt32(0));
                            gepArgs.push_back($3);
                            $$ = builder->CreateGEP(arrayType, ArrayMap[(char*)$1].irVal, gepArgs);
                        }

/* TypeIdentifier: VoidToken {
                            $$ = builder->getVoidTy();
                        }
                | Identifier {
                            $$ = builder->getInt32Ty();
                        } */

ExprList : '('')' { printf("EXPRList()\n"); } | '(' ExprSeq ')' { printf("EXPRList(...)\n"); }

ExprSeq: Expression { curFuncExprs.push_back($1); } | ExprSeq ',' Expression { curFuncExprs.push_back($3); }

ArgsList : '('')' { printf("ARGSList()\n"); } | '(' ArgsSeq ')' { printf("ARGSList(...)\n"); }

ArgsSeq : Identifier { curFuncArgs.push_back((char*)$1); } 
        | ArgsSeq ',' Identifier { curFuncArgs.push_back((char*)$3); }


/* ArgsList : '('')' { printf("ARGSList()\n"); } | '(' ArgsSeq ')' { printf("ARGSList(...)\n"); }

ArgsSeq : ArgDeclaration {} | ArgsSeq ',' ArgDeclaration {}

ArgDeclaration : Identifier {
                            printf("Identifier\n");
                            Value *ptr = builder->CreateAlloca(builder->getInt32Ty());
                            builder->CreateStore(builder->getInt32(777), ptr);
                            LocalValueMap.insert({(char*)$1, ptr});
                        } */

%%
