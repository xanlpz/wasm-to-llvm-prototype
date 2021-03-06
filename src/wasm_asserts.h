/*
// Copyright (c) 2015 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/
#ifndef H_WASMASSERTS
#define H_WASMASSERTS

#include <deque>

#include "wasm_assert.h"

// Forward declaration.
class WasmAssert;
class WasmFile;

class WasmAsserts {
  protected:
    std::deque<WasmAssert*> asserts_;

  public:
    WasmAsserts() {
    }

    void AddAssert(WasmAssert* a) {
      asserts_.push_front(a);
    }

    void Dump() const {
      for(auto elem : asserts_) {
        elem->Dump();
      }
    }

    void GenerateGeneralAssertCalls(WasmFile* file);

    void Generate(WasmFile* file) {
      for(auto elem : asserts_) {
        elem->Codegen(file);
      }

      // Now we want to call each of the methods.
      GenerateGeneralAssertCalls(file);
    }
};

#endif
