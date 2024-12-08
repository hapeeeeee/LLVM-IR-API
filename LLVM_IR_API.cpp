#include <iostream>
#include "llvm/IR/LLVMContext.h"    // Common Data Struct
#include "llvm/IR/Module.h"         // Abstract of A Source File {Function, Global Variable}
#include "llvm/IR/IRBuilder.h"      // Instruction Generator {Basic Instruction {add, sub, sem...}, Basic Variable Type}
#include "llvm/IR/Verifier.h"       // Verify Function or Module
#include <string>
using namespace llvm;

std::string aa() {return NULL;}

int main() {
    LLVMContext* llvmContext = new LLVMContext();
    Module* module = new Module("HelloModule", *llvmContext);
    IRBuilder<>* irBuilder = new IRBuilder<>(*llvmContext);

    // -------------------------------- Basic Type Start -------------------------------- //
    // int varInt;
    module->getOrInsertGlobal("varInt", Type::getInt32Ty(*llvmContext));
    // set varInt = 10;
    GlobalVariable* varInt = module->getNamedGlobal("varInt");
    varInt->setInitializer(irBuilder->getInt32(10)); 
    // set GlobalVariable(intVal) to `const`
    varInt->setConstant(true);

    // // float
    // module->getOrInsertGlobal("varFloat", irBuilder->getFloatTy());
    // -------------------------------- Basic Type End ------------------------------------ //

    
    // -------------------------------- Pointer Type Start -------------------------------- //
    // int *ptrInt
    GlobalVariable* ptrInt = static_cast<GlobalVariable*>(
        module->getOrInsertGlobal("ptrInit", PointerType::getInt32PtrTy(*llvmContext))
    );
    ptrInt->setInitializer(varInt);
    // -------------------------------- Pointer Type End --------------------------------- //


    // -------------------------------- Array Type Start -------------------------------- //
    // Array: double arrDouble[2];
    auto typeArr = ArrayType::get(Type::getDoubleTy(*llvmContext), 2);
    module->getOrInsertGlobal("arrDouble", typeArr);
    GlobalVariable* arrDouble = module->getNamedGlobal("arrDouble");
    // arrDouble = {1.1, 2.2}
    arrDouble->setInitializer(ConstantArray::get(
            typeArr,
            {
                ConstantFP::get(Type::getDoubleTy(*llvmContext), 1.1),
                ConstantFP::get(Type::getDoubleTy(*llvmContext), 2.2),
            }
        )
    );
    // -------------------------------- Array Type End ----------------------------------- //


    // -------------------------------- Struct Type Start -------------------------------- //
    // struct structA {int varInt, float varFloat, double varDouble, int* ptrInt}
    auto typeStruct = StructType::create(*llvmContext, "structA");
    typeStruct->setBody({
        irBuilder->getInt32Ty(), 
        irBuilder->getFloatTy(), 
        irBuilder->getDoubleTy(), 
        PointerType::getInt32PtrTy(*llvmContext),
    });
    
    module->getOrInsertGlobal("varStructA",  typeStruct);
    GlobalVariable* varStructA = module->getNamedGlobal("varStructA");
    varStructA->setInitializer(ConstantStruct::get(
        typeStruct, 
        {
            irBuilder->getInt32(2),
            ConstantFP::get(Type::getFloatTy(*llvmContext), 1),
            ConstantFP::get(Type::getDoubleTy(*llvmContext), 2.2),
            ConstantPointerNull::get(PointerType::getInt32PtrTy(*llvmContext))
        })
    );


    // -------------------------------- Pointer Array Type End ----------------------------- //
    //TODO
    // -------------------------------- Pointer Array Type Start --------------------------- //


    // -------------------------------- Function Start ------------------------------------- //

    /*
        int LLVM_MAX(int a, int b) {
            int localVarC;
            if (a < b) {
                localVarC = b;
            } else {
                localVarC = a;
            }
            return localVarC;
        }
    */
    Type* typeInt = irBuilder->getInt32Ty();
    FunctionType* typeFunc = FunctionType::get(typeInt, {typeInt, typeInt}, false);
    Function* func = Function::Create(typeFunc, GlobalValue::ExternalLinkage, "LLVM_MAX", *module);

    std::vector<std::string> params{"a", "b"};
    int idx = 0;
    for (Function::arg_iterator it = func->arg_begin(); it != func->arg_end(); ++it, ++idx) {
        it->setName(params[idx]);
    }
    BasicBlock* funcBody = BasicBlock::Create(*llvmContext, "", func);
    BasicBlock* thenBody = BasicBlock::Create(*llvmContext, "", func);
    BasicBlock* elseBody = BasicBlock::Create(*llvmContext, "", func);
    BasicBlock* mergeBody = BasicBlock::Create(*llvmContext, "", func);
    // set block to function
    irBuilder->SetInsertPoint(funcBody);
    // new local var ***ptr*** in function block
    Value* localVar_c = irBuilder->CreateAlloca(irBuilder->getInt32Ty(), nullptr, "localVar_c");
    // compare a and b of function parameter
    Value* cmpRet = irBuilder->CreateICmpSLT(func->getArg(0), func->getArg(1), "cmpRet");
    // use compare ret
    irBuilder->CreateCondBr(cmpRet, thenBody, elseBody);
    irBuilder->SetInsertPoint(thenBody);
    irBuilder->CreateStore(func->getArg(1), localVar_c);
    irBuilder->SetInsertPoint(elseBody);
    irBuilder->CreateStore(func->getArg(0), localVar_c);
    irBuilder->SetInsertPoint(mergeBody);
    Value* loadValue = irBuilder->CreateLoad(irBuilder->getInt32Ty(), localVar_c);
    // set return to function
    irBuilder->CreateRet(loadValue);
    verifyFunction(*func);
    // -------------------------------- Function End --------------------------------------- //


    // -------------------------------- For Start --------------------------------------- //
    // int sum(int n) {
    //     int s = 0;
    //     for (int i = 0; i < n; i++) {
    //         s = s + i;
    //     }
    //     return s;
    // }

    // Fcuntion Declare
    Type* int32Type = irBuilder->getInt32Ty();
    FunctionType* funcTypeSum = FunctionType::get(int32Type, {int32Type}, false);
    Function* funcSum = Function::Create(funcTypeSum, GlobalValue::ExternalLinkage, "LLVM_FOR", *module);
    BasicBlock* funcBodySum = BasicBlock::Create(*llvmContext, "", funcSum);

    // Function Local Var
    irBuilder->SetInsertPoint(funcBodySum);
    Value* localVar_n = irBuilder->CreateAlloca(int32Type, nullptr, "localVar_n");
    Value* localVar_s = irBuilder->CreateAlloca(int32Type, nullptr, "localVar_s");
    Value* localVar_i = irBuilder->CreateAlloca(int32Type, nullptr, "localVar_i");
    irBuilder->CreateStore(funcSum->getArg(0), localVar_n);
    irBuilder->CreateStore(irBuilder->getInt32(0), localVar_s);
    irBuilder->CreateStore(irBuilder->getInt32(0), localVar_i);

    // loop 
    BasicBlock* loopCondtionBlock = BasicBlock::Create(*llvmContext, "", funcSum);
    BasicBlock* loopBodyBlock = BasicBlock::Create(*llvmContext, "", funcSum);
    irBuilder->CreateBr(loopCondtionBlock);
    irBuilder->SetInsertPoint(loopCondtionBlock);

    Value* iVal = irBuilder->CreateLoad(int32Type, localVar_i);
    Value* nVal = irBuilder->CreateLoad(int32Type, localVar_n);
    Value* comRet = irBuilder->CreateICmpSLT(iVal, nVal);

    BasicBlock* forBody = BasicBlock::Create(*llvmContext, "", funcSum);
    BasicBlock* exitBody = BasicBlock::Create(*llvmContext, "", funcSum);
    irBuilder->CreateCondBr(comRet, forBody, exitBody);

    irBuilder->SetInsertPoint(forBody);
    iVal = irBuilder->CreateLoad(int32Type, localVar_i);
    Value* sVal = irBuilder->CreateLoad(int32Type, localVar_s);
    Value* sumVal = irBuilder->CreateNSWAdd(sVal, iVal);
    irBuilder->CreateStore(sumVal, localVar_s);

    BasicBlock* iAddBody = BasicBlock::Create(*llvmContext, "", funcSum);
    irBuilder->CreateBr(iAddBody);
    irBuilder->SetInsertPoint(iAddBody);
    iVal = irBuilder->CreateLoad(int32Type, localVar_i);
    irBuilder->CreateStore(irBuilder->CreateNSWAdd(iVal, irBuilder->getInt32(1)), localVar_i);
    irBuilder->CreateBr(loopCondtionBlock);

    irBuilder->SetInsertPoint(exitBody);
    sVal = irBuilder->CreateLoad(int32Type, localVar_s);
    irBuilder->CreateRet(sVal);
    // -------------------------------- For End --------------------------------------- //
    module->print(outs(), nullptr);

    return 0; 
}