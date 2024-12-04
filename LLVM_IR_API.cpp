#include <iostream>
#include "llvm/IR/LLVMContext.h"    // Common Data Struct
#include "llvm/IR/Module.h"         // Abstract of A Source File {Function, Global Variable}
#include "llvm/IR/IRBuilder.h"      // Instruction Generator {Basic Instruction {add, sub, sem...}, Basic Variable Type}
#include "llvm/IR/Verifier.h"       // Verify Function or Module

using namespace llvm;

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
            ConstantFP::get(Type::getFloatTy(*llvmContext), 1.1),
            ConstantFP::get(Type::getDoubleTy(*llvmContext), 2.2),
            ConstantPointerNull::get(PointerType::getInt32PtrTy(*llvmContext))
        })
    );


    // -------------------------------- Pointer Array Type End ----------------------------- //
    //TODO


    // -------------------------------- Pointer Array Type Start --------------------------- //


    // -------------------------------- Function Start ------------------------------------- //


    // -------------------------------- Function End --------------------------------------- //
    module->print(outs(), nullptr);
    return 0; 
}