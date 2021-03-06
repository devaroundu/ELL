////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AsyncEmitterTest.cpp (emitters_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AsyncEmitterTest.h"

// emitters
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IRAsyncTask.h"
#include "IRBlockRegion.h"
#include "IREmitter.h"
#include "IRExecutionEngine.h"
#include "IRFunctionEmitter.h"
#include "IRModuleEmitter.h"
#include "LLVMUtilities.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

using namespace ell;
using namespace ell::emitters;

//
// Helpers
//

using UnaryScalarDoubleFunction = double (*)(double);

//
// Tests
//
void TestIRAsyncTask()
{
    std::cout << "Testing IRAsyncTask Function" << std::endl;
    IRModuleEmitter module("CompilableIRFunction");
    module.DeclarePrintf();

    // Common stuff for both functions
    NamedVariableTypeList args;
    args.push_back({ "x", VariableType::Double });

    //
    // Regular function
    //
    std::string syncFunctionName = "TestSync";
    auto syncFunction = module.BeginFunction(syncFunctionName, VariableType::Double, args);
    llvm::Value* syncArg = syncFunction.GetFunctionArgument("x");
    llvm::Value* syncSum = syncFunction.Operator(emitters::GetAddForValueType<double>(), syncArg, syncFunction.Literal<double>(5.0));
    syncFunction.Return(syncSum);
    module.EndFunction();

    //
    // Async task
    //
    auto& context = module.GetLLVMContext();
    llvm::Type* asyncReturnType = llvm::Type::getDoubleTy(context);
    std::string asyncFunctionName = "TestAsync";
    auto asyncFunction = module.BeginFunction(asyncFunctionName, VariableType::Double, args);
    asyncFunction.Print("Begin outer function\n");
    
    llvm::Value* asyncArg = asyncFunction.GetFunctionArgument("x");
    auto f = module.BeginFunction("asyncTaskFunction", asyncReturnType, GetLLVMTypes({asyncArg}));
    {
        auto arguments = f.Arguments().begin();
        auto argVal = &(*arguments++);
        f.Printf("Begin async task, input: %f\n", {argVal});
        llvm::Value* asyncSum = f.Operator(emitters::GetAddForValueType<double>(), argVal, f.Literal<double>(5.0));
        f.Return(asyncSum);
    }
    module.EndFunction();

    asyncFunction.Print("Before calling Sync\n");
    auto t = asyncFunction.Async(f, { asyncArg }); // TODO: pass in local values to capture == args to async function
    t.Sync();
    asyncFunction.Print("After calling Sync\n");
    asyncFunction.Return(t.GetReturnValue());
    module.EndFunction();

    // print the generated code
    // module.DebugDump();

    IRExecutionEngine executionEngine(std::move(module));
    UnaryScalarDoubleFunction syncCompiledFunction = (UnaryScalarDoubleFunction)executionEngine.ResolveFunctionAddress(syncFunctionName);
    UnaryScalarDoubleFunction asyncCompiledFunction = (UnaryScalarDoubleFunction)executionEngine.ResolveFunctionAddress(asyncFunctionName);

    std::vector<double> data({ 1.1, 2.1, 3.1, 4.1, 5.1 });
    std::vector<double> computedResult;
    std::vector<double> syncCompiledResult;
    std::vector<double> asyncCompiledResult;
    for (auto x : data)
    {
        computedResult.push_back(x + 5.0);
        syncCompiledResult.push_back(syncCompiledFunction(x));
        asyncCompiledResult.push_back(asyncCompiledFunction(x));
    }
    testing::ProcessTest("Testing compilable syncFunction", testing::IsEqual(computedResult, syncCompiledResult));
    testing::ProcessTest("Testing compilable asyncFunction", testing::IsEqual(computedResult, asyncCompiledResult));
    // std::cout << "Computed result " << computedResult << ", compiled: " << asyncCompiledResult << std::endl;
}

// void TestIRAsyncLambdaTask()
// {
//     std::cout << "Testing IRAsyncTask Function" << std::endl;
//     IRModuleEmitter module("CompilableIRFunction");
//     module.DeclarePrintf();

//     // Common stuff for both functions
//     NamedVariableTypeList args;
//     args.push_back({ "x", VariableType::Double });

//     //
//     // Async task
//     //
//     auto& context = module.GetLLVMContext();
//     llvm::Type* asyncReturnType = llvm::Type::getDoubleTy(context);
//     std::string asyncFunctionName = "TestAsync";
//     auto asyncFunction = module.BeginFunction(asyncFunctionName, VariableType::Double, args);

//     asyncFunction.Print("Begin outer function\n");

//     llvm::Value* asyncArg = asyncFunction.GetFunctionArgument("x");

//     auto t = asyncFunction.Async(asyncArg, [](IRAsyncEmitter& t, llvm::Value& asyncArg) {
//         auto& f = t.GetFunction();
//         f.Print("Begin async task\n");
//         llvm::Value* asyncSum = f.Operator(emitters::GetAddForValueType<double>(), asyncArg, f.Literal<double>(5.0));
//         return asyncSum;
//     });

//     asyncFunction.Print("Before calling Sync\n");
//     t.Sync();
//     asyncFunction.Print("After calling Sync\n");
//     asyncFunction.Return(t.GetReturnValue());
//     module.EndFunction();

//     // print the generated code
//     module.DebugDump();

//     IRExecutionEngine executionEngine(std::move(module));
//     UnaryScalarDoubleFunction syncCompiledFunction = (UnaryScalarDoubleFunction)executionEngine.ResolveFunctionAddress(syncFunctionName);
//     UnaryScalarDoubleFunction asyncCompiledFunction = (UnaryScalarDoubleFunction)executionEngine.ResolveFunctionAddress(asyncFunctionName);

//     std::vector<double> data({ 1.1, 2.1, 3.1, 4.1, 5.1 });
//     std::vector<double> computedResult;
//     std::vector<double> syncCompiledResult;
//     std::vector<double> asyncCompiledResult;
//     for (auto x : data)
//     {
//         computedResult.push_back(x + 5.0);
//         syncCompiledResult.push_back(syncCompiledFunction(x));
//         asyncCompiledResult.push_back(asyncCompiledFunction(x));
//     }
//     testing::ProcessTest("Testing compilable syncFunction", testing::IsEqual(computedResult, syncCompiledResult));
//     testing::ProcessTest("Testing compilable asyncFunction", testing::IsEqual(computedResult, asyncCompiledResult));
// }
