//==================================================================================
// BSD 2-Clause License
//
// Copyright (c) 2014-2024, NJIT, Duality Technologies Inc. and other contributors
//
// All rights reserved.
//
// Author TPOC: contact@openfhe.org
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==================================================================================

/*
 * This code benchmarks polynomial operations for ring dimension of 16k.
 */

#include "math/hal/basicint.h"
#include "poly-benchmark.h"
#include <iostream>

constexpr uint32_t RING_DIM_LOG = 14;
constexpr uint32_t DCRTBITS     = 50;

class Setup {
public:
    Setup() {
        std::cerr << "Generating polynomials for the benchmark..." << std::endl;
        GeneratePolys((1 << (RING_DIM_LOG + 1)), DCRTBITS, NativepolysEval, NativepolysCoef);
        GenerateDCRTPolys((1 << (RING_DIM_LOG + 1)), DCRTBITS, DCRTpolysEval, DCRTpolysCoef);
        std::cerr << "Polynomials for the benchmark are generated" << std::endl;
    }
} TestParameters;

BENCHMARK_MAIN();
