//--------------------------------------------------------------------*- C++ -*-
// author:  Vassil Vassilev <v.g.vassilev-at-gmail.com>
//------------------------------------------------------------------------------

#include "fcct/Transformers/CoroBuilder.h"

using namespace clang;

namespace fcct {
  CXXRecordDecl *CoroBuilder::Transform(FunctionDecl* FD) {
    // coroutine<int()> foo(float a, float b) {}
    //
    // class foo : public coroutine<int()> {
    //   float a;
    //   float b;
    //   int __body() {}
    // };

    return nullptr;
  }
} // end namespace fcct

