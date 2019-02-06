//--------------------------------------------------------------------*- C++ -*-
// author:  Vassil Vassilev <v.g.vassilev-at-gmail.com>
//------------------------------------------------------------------------------


#ifndef FCCT_CORO_BUILDER
#define FCCT_CORO_BUILDER

namespace clang {
  class CXXRecordDecl;
  class FunctionDecl;
  class Sema;
}

namespace fcct {
  class CoroBuilder {
    clang::Sema& SemaRef;
  public:
    CoroBuilder(clang::Sema& Sema) : SemaRef(Sema) {}
    clang::CXXRecordDecl *Transform(clang::FunctionDecl* FD);
  };

} // end namespace fcct

#endif // FCCT_CORO_BUILDER
