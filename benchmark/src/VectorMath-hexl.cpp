//==================================================================================
// BSD 2-Clause License
//
// Copyright (c) 2014-2022, NJIT, Duality Technologies Inc. and other contributors
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
  This code benchmarks vector operations.
 */
#define _USE_MATH_DEFINES
#define MAX_MODULUS_SIZE_HEXL 51

#include "vechelper.h"
#include "lattice/elemparamfactory.h"

#include "benchmark/benchmark.h"

#include <iostream>

using namespace lbcrypto;

// vec + vec
template <typename V>
static void VecVec_Add(V& a, const V& b) {
    a = a + b;
}
template <typename V>
static void BM_VecVec_Add(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    V b    = makeVector<V>(p, q);
    while (state.KeepRunning())
        VecVec_Add<V>(a, b);
}

// vec += vec
template <typename V>
static void VecVec_AddEq(V& a, const V& b) {
    a += b;
}
template <typename V>
static void BM_VecVec_AddEq(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    V b    = makeVector<V>(p, q);
    while (state.KeepRunning())
        VecVec_AddEq<V>(a, b);
}

// vec * vec
template <typename V>
static void VecVec_Mul(V& a, const V& b) {
    a = a * b;
}
template <typename V>
static void BM_VecVec_Mul(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    V b    = makeVector<V>(p, q);
    while (state.KeepRunning())
        VecVec_Mul<V>(a, b);
}

// vec *= vec
template <typename V>
static void VecVec_MulEq(V& a, const V& b) {
    a *= b;
}
template <typename V>
static void BM_VecVec_MulEq(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V b    = makeVector<V>(p, q);
    V a    = makeVector<V>(p, q);
    while (state.KeepRunning())
        VecVec_MulEq<V>(a, b);
}

// vec + scal
template <typename V>
static void VecInt_Add(V& a, const typename V::Integer& b) {
    a = a + b;
}
template <typename V>
static void BM_VecInt_Add(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    typename V::Integer b(37 % q);
    while (state.KeepRunning())
        VecInt_Add<V>(a, b);
}

// vec += scal
template <typename V>
static void VecInt_AddEq(V& a, const typename V::Integer& b) {
    a += b;
}
template <typename V>
static void BM_VecInt_AddEq(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    typename V::Integer b(42 % q);
    while (state.KeepRunning())
        VecInt_AddEq<V>(a, b);
}

// vec * scal
template <typename V>
static void VecInt_Mul(V& a, const typename V::Integer& b) {
    a = a * b;
}

template <typename V>
static void BM_VecInt_Mul(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    typename V::Integer b(10101 % q);
    while (state.KeepRunning())
        VecInt_Mul<V>(a, b);
}

// vec *= scal
template <typename V>
static void VecInt_MulEq(V& a, const typename V::Integer& b) {
    a *= b;
}
template <typename V>
static void BM_VecInt_MulEq(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    typename V::Integer b(55 % q);
    while (state.KeepRunning())
        VecInt_MulEq<V>(a, b);
}

template <typename V>
static void VecVec_ModAddNoCheckEq(V& a, const V& b) {
    a.ModAddNoCheckEq(b);
}
template <typename V>
static void BM_VecVec_ModAddNoCheckEq(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    V b    = makeVector<V>(p, q);
    while (state.KeepRunning())
        VecVec_ModAddNoCheckEq<V>(a, b);
}

template <typename V>
static void VecVec_ModMulNoCheckEq(V& a, const V& b) {
    a.ModMulNoCheckEq(b);
}
template <typename V>
static void BM_VecVec_ModMulNoCheckEq(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    V b    = makeVector<V>(p, q);
    while (state.KeepRunning())
        VecVec_ModMulNoCheckEq<V>(a, b);
}

template <typename V>
static void VecInt_SwitchModulus(V a, const typename V::Integer& b) {
    a.SwitchModulus(b);
}
template <typename V>
static void BM_VecInt_SwitchModulus_Up(benchmark::State& state) {
    auto p  = state.range(0);
    auto f1 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q1 = PreviousPrime<typename V::Integer>(f1, p + 1);
    auto f2 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL - 1, (p >> 1) + 1);
    auto q2 = PreviousPrime<typename V::Integer>(f2, (p >> 1) + 1);
    V a     = makeVector<V>(p, q2);
    while (state.KeepRunning())
        VecInt_SwitchModulus<V>(a, q1);
}
template <typename V>
static void BM_VecInt_SwitchModulus_Down(benchmark::State& state) {
    auto p  = state.range(0);
    auto f1 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q1 = PreviousPrime<typename V::Integer>(f1, p + 1);
    auto f2 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL - 1, (p >> 1) + 1);
    auto q2 = PreviousPrime<typename V::Integer>(f2, (p >> 1) + 1);
    V a     = makeVector<V>(p, q1);
    while (state.KeepRunning())
        VecInt_SwitchModulus<V>(a, q2);
}

template <typename V>
static void VecInt_Mod(const V& a, const typename V::Integer& b) {
    V c = a.Mod(b);
}
template <typename V>
static void BM_VecInt_Mod_Up(benchmark::State& state) {
    auto p  = state.range(0);
    auto f1 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q1 = PreviousPrime<typename V::Integer>(f1, p + 1);
    auto f2 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL - 1, (p >> 1) + 1);
    auto q2 = PreviousPrime<typename V::Integer>(f2, (p >> 1) + 1);
    V a     = makeVector<V>(p, q2);
    while (state.KeepRunning())
        VecInt_Mod<V>(a, q1);
}
template <typename V>
static void BM_VecInt_Mod_Down(benchmark::State& state) {
    auto p  = state.range(0);
    auto f1 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q1 = PreviousPrime<typename V::Integer>(f1, p + 1);
    auto f2 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL - 1, (p >> 1) + 1);
    auto q2 = PreviousPrime<typename V::Integer>(f2, (p >> 1) + 1);
    V a     = makeVector<V>(p, q1);
    while (state.KeepRunning())
        VecInt_Mod<V>(a, q2);
}
template <typename V>
static void BM_VecInt_Mod_ModByTwo(benchmark::State& state) {
    auto p  = state.range(0);
    auto f1 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q1 = PreviousPrime<typename V::Integer>(f1, p + 1);
    V a     = makeVector<V>(p, q1);
    typename V::Integer b(2);
    while (state.KeepRunning())
        VecInt_Mod<V>(a, b);
}

template <typename V>
static void VecInt_ModEq(V a, const typename V::Integer& b) {
    a.ModEq(b);
}
template <typename V>
static void BM_VecInt_ModEq_Up(benchmark::State& state) {
    auto p  = state.range(0);
    auto f1 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q1 = PreviousPrime<typename V::Integer>(f1, p + 1);
    auto f2 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL - 1, (p >> 1) + 1);
    auto q2 = PreviousPrime<typename V::Integer>(f2, (p >> 1) + 1);
    V a     = makeVector<V>(p, q2);
    while (state.KeepRunning())
        VecInt_ModEq<V>(a, q1);
}
template <typename V>
static void BM_VecInt_ModEq_Down(benchmark::State& state) {
    auto p  = state.range(0);
    auto f1 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q1 = PreviousPrime<typename V::Integer>(f1, p + 1);
    auto f2 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL - 1, (p >> 1) + 1);
    auto q2 = PreviousPrime<typename V::Integer>(f2, (p >> 1) + 1);
    V a     = makeVector<V>(p, q1);
    while (state.KeepRunning())
        VecInt_ModEq<V>(a, q2);
}
template <typename V>
static void BM_VecInt_ModEq_ModByTwo(benchmark::State& state) {
    auto p  = state.range(0);
    auto f1 = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q1 = PreviousPrime<typename V::Integer>(f1, p + 1);
    V a     = makeVector<V>(p, q1);
    typename V::Integer b(2);
    while (state.KeepRunning())
        VecInt_ModEq<V>(a, b);
}

// vec + scal
template <typename V>
static void VecInt_ModAdd(V& a, const typename V::Integer& b) {
    a = a.ModAdd(b);
}
template <typename V>
static void BM_VecInt_ModAdd(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    typename V::Integer b(37 % q);
    while (state.KeepRunning())
        VecInt_ModAdd<V>(a, b);
}

// vec += scal
template <typename V>
static void VecInt_ModAddEq(V& a, const typename V::Integer& b) {
    a.ModAddEq(b);
}
template <typename V>
static void BM_VecInt_ModAddEq(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    typename V::Integer b(42 % q);
    while (state.KeepRunning())
        VecInt_ModAddEq<V>(a, b);
}

template <typename V>
static void VecVec_ModAdd(V& a, const V& b) {
    a = a.ModAdd(b);
}
template <typename V>
static void BM_VecVec_ModAdd(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    V b    = makeVector<V>(p, q);
    while (state.KeepRunning())
        VecVec_ModAdd<V>(a, b);
}

// vec += vec
template <typename V>
static void VecVec_ModAddEq(V& a, const V& b) {
    a.ModAddEq(b);
}
template <typename V>
static void BM_VecVec_ModAddEq(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    V b    = makeVector<V>(p, q);
    while (state.KeepRunning())
        VecVec_ModAddEq<V>(a, b);
}

template <typename V>
static void VecInt_ModMul(V& a, const typename V::Integer& b) {
    a = a.ModMul(b);
}
template <typename V>
static void BM_VecInt_ModMul(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    typename V::Integer b(10101 % q);
    while (state.KeepRunning())
        VecInt_ModMul<V>(a, b);
}

template <typename V>
static void VecInt_ModMulEq(V& a, const typename V::Integer& b) {
    a.ModMulEq(b);
}
template <typename V>
static void BM_VecInt_ModMulEq(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    typename V::Integer b(55 % q);
    while (state.KeepRunning())
        VecInt_ModMulEq<V>(a, b);
}

template <typename V>
static void VecVec_ModMul(V& a, const V& b) {
    a = a.ModMul(b);
}
template <typename V>
static void BM_VecVec_ModMul(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V a    = makeVector<V>(p, q);
    V b    = makeVector<V>(p, q);
    while (state.KeepRunning())
        VecVec_ModMul<V>(a, b);
}

template <typename V>
static void VecVec_ModMulEq(V& a, const V& b) {
    a.ModMulEq(b);
}
template <typename V>
static void BM_VecVec_ModMulEq(benchmark::State& state) {
    auto p = state.range(0);
    auto f = FirstPrime<typename V::Integer>(MAX_MODULUS_SIZE_HEXL, p + 1);
    auto q = PreviousPrime<typename V::Integer>(f, p + 1);
    V b    = makeVector<V>(p, q);
    V a    = makeVector<V>(p, q);
    while (state.KeepRunning())
        VecVec_ModMulEq<V>(a, b);
}

#define DO_NATIVEVECTOR_BENCHMARK(X)                                                                      \
    BENCHMARK_TEMPLATE(X, NativeVector)->Unit(benchmark::kMicrosecond)->ArgName("RingDm")->Arg(1024);     \
    /*BENCHMARK_TEMPLATE(X, NativeVector)->Unit(benchmark::kMicrosecond)->ArgName("RingDm")->Arg(2048);*/ \
    BENCHMARK_TEMPLATE(X, NativeVector)->Unit(benchmark::kMicrosecond)->ArgName("RingDm")->Arg(4096);     \
    /*BENCHMARK_TEMPLATE(X, NativeVector)->Unit(benchmark::kMicrosecond)->ArgName("RingDm")->Arg(8192);*/ \
    BENCHMARK_TEMPLATE(X, NativeVector)->Unit(benchmark::kMicrosecond)->ArgName("RingDm")->Arg(16384);    \
    BENCHMARK_TEMPLATE(X, NativeVector)->Unit(benchmark::kMicrosecond)->ArgName("RingDm")->Arg(32768);

DO_NATIVEVECTOR_BENCHMARK(BM_VecVec_ModAddNoCheckEq)
DO_NATIVEVECTOR_BENCHMARK(BM_VecVec_ModMulNoCheckEq)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_SwitchModulus_Up)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_SwitchModulus_Down)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_Mod_Up)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_Mod_Down)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_Mod_ModByTwo)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_ModEq_Up)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_ModEq_Down)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_ModEq_ModByTwo)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_ModAdd)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_ModAddEq)
DO_NATIVEVECTOR_BENCHMARK(BM_VecVec_ModAdd)
DO_NATIVEVECTOR_BENCHMARK(BM_VecVec_ModAddEq)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_ModMul)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_ModMulEq)
DO_NATIVEVECTOR_BENCHMARK(BM_VecVec_ModMul)
DO_NATIVEVECTOR_BENCHMARK(BM_VecVec_ModMulEq)

DO_NATIVEVECTOR_BENCHMARK(BM_VecVec_Add)
DO_NATIVEVECTOR_BENCHMARK(BM_VecVec_AddEq)
DO_NATIVEVECTOR_BENCHMARK(BM_VecVec_Mul)
DO_NATIVEVECTOR_BENCHMARK(BM_VecVec_MulEq)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_Add)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_AddEq)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_Mul)
DO_NATIVEVECTOR_BENCHMARK(BM_VecInt_MulEq)

// execute the benchmarks
BENCHMARK_MAIN();
