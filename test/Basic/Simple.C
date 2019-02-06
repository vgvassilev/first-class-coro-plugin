// RUN: %fcctclang  %s -I%S/../../include -oSimpleFunctor.out 2>&1
// R: %fcctclang  %s -I%S/../../include -oSimpleFunctor.out 2>&1 | FileCheck %s
// R: ./SimpleFunctor.out | FileCheck -check-prefix=CHECK-EXEC %s

//CHECK-NOT: {{.*error|warning|note:.*}}

int main() {

}

