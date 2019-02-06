//--------------------------------------------------------------------*- C++ -*-
// author:  Vassil Vassilev <v.g.vassilev-at-gmail.com>
//------------------------------------------------------------------------------

#ifndef FCCT_CLANG_PLUGIN
#define FCCT_CLANG_PLUGIN

#include "fcct/Transformers/CoroBuilder.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

namespace clang {
  class CompilerInstance;
  class DeclGroupRef;
}

namespace fcct {
  namespace plugin {
    struct TransformationOptions {
      TransformationOptions()
        : DumpSourceFn(false), DumpSourceFnAST(false), DumpTransformedFn(false),
          DumpTransformedAST(false), GenerateSourceFile(false) { }

      bool DumpSourceFn : 1;
      bool DumpSourceFnAST : 1;
      bool DumpTransformedFn : 1;
      bool DumpTransformedAST : 1;
      bool GenerateSourceFile : 1;
    };

    class FCCTPlugin : public clang::ASTConsumer {
    private:
      clang::CompilerInstance& m_CI;
      TransformationOptions m_TO;
      std::unique_ptr<CoroBuilder> m_CoroBuilder;
    public:
      FCCTPlugin(clang::CompilerInstance& CI, TransformationOptions& TO);
      ~FCCTPlugin();

      virtual bool HandleTopLevelDecl(clang::DeclGroupRef DGR);
    private:
      bool ShouldProcessDecl(clang::DeclGroupRef DGR);
    };

    template<typename ConsumerType>
    class Action : public clang::PluginASTAction {
    private:
      TransformationOptions m_TO;
    protected:
      std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance& CI,
        llvm::StringRef InFile) {
        return std::unique_ptr<clang::ASTConsumer>(new ConsumerType(CI, m_TO));
      }

      bool ParseArgs(const clang::CompilerInstance &CI,
                     const std::vector<std::string>& args) {
        for (unsigned i = 0, e = args.size(); i != e; ++i) {
          if (args[i] == "-fdump-source-fn") {
            m_TO.DumpSourceFn = true;
          }
          else if (args[i] == "-fdump-source-fn-ast") {
            m_TO.DumpSourceFnAST = true;
          }
          else if (args[i] == "-fdump-transformed-fn") {
            m_TO.DumpTransformedFn = true;
          }
          else if (args[i] == "-fdump-transformed-fn-ast") {
            m_TO.DumpTransformedAST = true;
          }
          else if (args[i] == "-fgenerate-source-file") {
            m_TO.GenerateSourceFile = true;
          }
          else if (args[i] == "-help") {
            // Print some help info.
            llvm::errs() <<
              "Option set for the first class coroutines transformer - fcct:\n\n" <<
              "-fdump-source-fn - Prints out the source code of the function.\n" <<
              "-fdump-source-fn-ast - Prints out the AST of the function.\n" <<
              "-fdump-transformed-fn - Prints out the source code of the transformed function.\n" <<
              "-fdump-transformed-fn-ast - Prints out the AST of the transformed function.\n" <<
              "-fgenerate-source-file - Produces a file containing the transformed functions.\n";

            llvm::errs() << "-help - Prints out this screen.\n\n";
          }
          else {
            llvm::errs() << "fcct: Error: invalid option "
                         << args[i] << "\n";
            return false; // Tells clang not to create the plugin.
          }
        }
        return true;
      }

      PluginASTAction::ActionType getActionType() override {
        return AddBeforeMainAction;
      }
    };
  } // end namespace plugin
} // end namespace fcct

#endif // FCCT_CLANG_PLUGIN
