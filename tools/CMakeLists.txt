if( NOT FCCT_BUILT_STANDALONE AND NOT LLVM_BUILD_TOOLS )
  set(EXCLUDE_FROM_ALL ON)
endif()

add_llvm_library(fcctPlugin
  ClangPlugin.cpp
  )
if (NOT FCCT_BUILD_STATIC_ONLY)
  add_llvm_loadable_module(fcct
    ClangPlugin.cpp
    PLUGIN_TOOL
    clang
  )

  target_link_libraries(fcct PRIVATE
    fcctTransform
  )
endif()
