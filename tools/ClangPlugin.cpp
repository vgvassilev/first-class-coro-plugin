//--------------------------------------------------------------------*- C++ -*-
// author:  Vassil Vassilev <v.g.vassilev-at-gmail.com>
//------------------------------------------------------------------------------

#include "ClangPlugin.h"

#include "fcct/Transformers/CoroBuilder.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Timer.h"

using namespace clang;

namespace {
  class SimpleTimer {
    bool WantTiming;
    llvm::TimeRecord Start;
    std::string Output;

  public:
    explicit SimpleTimer(bool WantTiming) : WantTiming(WantTiming) {
      if (WantTiming)
        Start = llvm::TimeRecord::getCurrentTime();
    }

    void setOutput(const Twine &Output) {
      if (WantTiming)
        this->Output = Output.str();
    }

    ~SimpleTimer() {
      if (WantTiming) {
        llvm::TimeRecord Elapsed = llvm::TimeRecord::getCurrentTime();
        Elapsed -= Start;
        llvm::errs() << Output << ": user | system | process | all :";
        Elapsed.print(Elapsed, llvm::errs());
        llvm::errs() << '\n';
      }
    }
  };
}


namespace fcct {
  namespace plugin {
    FCCTPlugin::FCCTPlugin(CompilerInstance& CI, TransformationOptions& TO)
      : m_CI(CI), m_TO(TO) { }
    FCCTPlugin::~FCCTPlugin() {}

    bool FCCTPlugin::HandleTopLevelDecl(DeclGroupRef DGR) {
      if (!ShouldProcessDecl(DGR))
        return true;

      if (!m_CoroBuilder)
        m_CoroBuilder.reset(new CoroBuilder(m_CI.getSema()));

      // Instantiate all pending for instantiations templates, because we will
      // need the full bodies to produce coroutines.
      m_CI.getSema().PerformPendingInstantiations();

      //set up printing policy
      clang::LangOptions LangOpts;
      LangOpts.CPlusPlus = true;
      clang::PrintingPolicy Policy(LangOpts);
      FunctionDecl* FD = cast<FunctionDecl>(DGR.getSingleDecl());

      // if enabled, print source code of the original functions
      if (m_TO.DumpSourceFn) {
        FD->print(llvm::outs(), Policy);
      }
      // if enabled, print ASTs of the original functions
      if (m_TO.DumpSourceFnAST) {
        FD->dumpColor();
      }

      CXXRecordDecl* CoroDecl = nullptr;
      {
        bool WantTiming = getenv("LIBFCCT_TIMING");
        SimpleTimer Timer(WantTiming);
        Timer.setOutput("Generation time for "
                        + FD->getNameAsString());

        CoroDecl = m_CoroBuilder->Transform(FD);
      }

      if (CoroDecl) {
        // if enabled, print source code of the transformed functions
        if (m_TO.DumpTransformedFn) {
          CoroDecl->print(llvm::outs(), Policy);
        }
        // if enabled, print ASTs of the transformed functions
        if (m_TO.DumpTransformedAST) {
          CoroDecl->dumpColor();
        }
        // if enabled, print the transformed decls in a file.
        if (m_TO.GenerateSourceFile) {
          std::error_code err;
          llvm::raw_fd_ostream f("FirstClassCoroutines.cpp", err,
                                 llvm::sys::fs::F_Append);
          CoroDecl->print(f, Policy);
          f.flush();
        }
        // FIXME: Call CodeGen only if the produced decl is a top-most decl.
        bool isTU = CoroDecl->getDeclContext()->isTranslationUnit();
        if (isTU) {
          m_CI.getASTConsumer().HandleTopLevelDecl(DeclGroupRef(CoroDecl));
        }
      }
      return true; // Happiness
    }

    bool FCCTPlugin::ShouldProcessDecl(DeclGroupRef DGR) {
      if (!DGR.isSingleDecl())
        return false;
      if (!isa<FunctionDecl>(DGR.getSingleDecl()))
        return false;
      //FIXME: Check if this is a FunctionDecl with the right return type.
      return true;
    }


  } // end namespace plugin
} // end namespace fcct

using namespace fcct::plugin;
// register the PluginASTAction in the registry.
static clang::FrontendPluginRegistry::Add<Action<FCCTPlugin> >
X("fcct", "Produces first class coroutines");
