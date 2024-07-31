#ifndef JIT_H
#define JIT_H

#include <llvm/ADT/StringRef.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutorProcessControl.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/Shared/ExecutorSymbolDef.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>
#include <memory>
#include <utility>

using namespace llvm;
using namespace llvm::orc;

class LLanguageJIT {
private:
  std::unique_ptr<ExecutionSession> es;

  DataLayout dl;
  MangleAndInterner mangle;

  RTDyldObjectLinkingLayer objLayer;
  IRCompileLayer compilerLayer;

  JITDylib &mainJD;

public:
  LLanguageJIT(std::unique_ptr<ExecutionSession> es,
               JITTargetMachineBuilder jitTargetMB, DataLayout dl)
      : es(std::move(es)), dl(std::move(dl)), mangle(*this->es, this->dl),
        objLayer(*this->es,
                 []() { return std::make_unique<SectionMemoryManager>(); }),
        compilerLayer(
            *this->es, objLayer,
            std::make_unique<ConcurrentIRCompiler>(std::move(jitTargetMB))),
        mainJD(this->es->createBareJITDylib("<main>")) {
    mainJD.addGenerator(
        cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(
            dl.getGlobalPrefix())));
    if (jitTargetMB.getTargetTriple().isOSBinFormatCOFF()) {
      objLayer.setOverrideObjectFlagsWithResponsibilityFlags(true);
      objLayer.setAutoClaimResponsibilityForObjectSymbols(true);
    }
  }

  ~LLanguageJIT() {
    if (auto err = es->endSession()) {
      es->reportError(std::move(err));
    }
  }
  static Expected<std::unique_ptr<LLanguageJIT>> Create() {
    auto epc = SelfExecutorProcessControl::Create();
    if (!epc)
      return epc.takeError();

    auto es = std::make_unique<ExecutionSession>(std::move(*epc));

    JITTargetMachineBuilder jitTargetMB(
        es->getExecutorProcessControl().getTargetTriple());

    auto dl = jitTargetMB.getDefaultDataLayoutForTarget();

    if (!dl)
      return dl.takeError();

    return std::make_unique<LLanguageJIT>(std::move(es), std::move(jitTargetMB),
                                          std::move(*dl));
  }

  const DataLayout &getDataLayout() const { return dl; }

  JITDylib &getMainJITDylib() { return mainJD; }

  Error addModule(ThreadSafeModule tsm, ResourceTrackerSP rt = nullptr) {
    if (!rt)
      rt = mainJD.getDefaultResourceTracker();
    return compilerLayer.add(rt, std::move(tsm));
  }

  Expected<ExecutorSymbolDef> lookup(StringRef name) {
    return es->lookup({&mainJD}, mangle(name.str()));
  }
};

#endif