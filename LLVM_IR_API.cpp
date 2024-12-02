#include <iostream>
#include "llvm/IR/LLVMContext.h"    // Common
#include "llvm/IR/Module.h"         // Abstract of A Source File {Function, Global Variable}
#include "llvm/IR/IRBuilder.h"      // Instruction Generator {Basic Instruction {add, sub, sem...}, Variable Type}
#include "llvm/IR/Verifier.h"       // Verify Function or Module

using namespace llvm;

int main() {
    LLVMContext* llvmContext = new LLVMContext();
    Module* module = new Module("HelloModule", *llvmContext);
    IRBuilder<>* irBuilder = new IRBuilder<>(*llvmContext);

    // int a;
    module->getOrInsertGlobal("a", irBuilder->getInt32Ty());
    

    module->print(outs(), nullptr);
    std::cout << 1111 << std::endl;
    return 0;
}