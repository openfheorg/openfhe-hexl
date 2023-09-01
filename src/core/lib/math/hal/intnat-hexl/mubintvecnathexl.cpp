//==================================================================================
// BSD 2-Clause License
//
// Copyright (c) 2014-2023, NJIT, Duality Technologies Inc. and other contributors
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
  This code provides basic arithmetic functionality for vectors of HEXL accelerated native integers
 */

// TODO: Hexl-fy more functions

#include "config_core.h"
#ifdef WITH_INTEL_HEXL

    #include "hexl/hexl.hpp"

    #include "math/math-hal.h"
    #include "math/hal/intnat-hexl/mubintvecnathexl.h"
    #include "math/nbtheory-impl.h"

    #include "utils/exception.h"

namespace intnathexl {

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::ModAdd(const IntegerType& b) const {
    if constexpr (HEXL_ADD_ENABLE && std::is_same_v<IntegerType, NativeInteger>) {
        NativeVectorT ans(m_data.size(), m_modulus);
        uint64_t* res       = reinterpret_cast<uint64_t*>(&ans.m_data[0]);
        const uint64_t* op1 = reinterpret_cast<const uint64_t*>(&m_data[0]);
        intel::hexl::EltwiseAddMod(res, op1, b.m_value >= m_modulus.m_value ? b.m_value % m_modulus.m_value : b.m_value,
                                   m_data.size(), m_modulus.m_value);
        return ans;
    }
    else {
        auto ans(*this);
        auto mv{m_modulus};
        auto bv{b};
        if (bv.m_value >= mv.m_value)
            bv.ModEq(mv);
        for (size_t i = 0; i < ans.m_data.size(); ++i)
            ans.m_data[i] = ans.m_data[i].ModAddFast(bv, mv);
        return ans;
    }
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::ModAddEq(const IntegerType& b) {
    if constexpr (HEXL_ADD_ENABLE && std::is_same_v<IntegerType, NativeInteger>) {
        uint64_t* op1 = reinterpret_cast<uint64_t*>(&m_data[0]);
        intel::hexl::EltwiseAddMod(op1, op1, b.m_value >= m_modulus.m_value ? b.m_value % m_modulus.m_value : b.m_value,
                                   m_data.size(), m_modulus.m_value);
    }
    else {
        auto mv{m_modulus};
        auto bv{b};
        if (bv.m_value >= mv.m_value)
            bv.ModEq(mv);
        for (size_t i = 0; i < m_data.size(); ++i)
            m_data[i] = m_data[i].ModAddFast(bv, mv);
    }
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::ModAdd(const NativeVectorT& b) const {
    if (m_modulus != b.m_modulus || m_data.size() != b.m_data.size())
        OPENFHE_THROW(lbcrypto::math_error, "ModAdd called on NativeVectorT's with different parameters.");
    if constexpr (HEXL_ADD_ENABLE && std::is_same_v<IntegerType, NativeInteger>) {
        NativeVectorT ans(m_data.size(), m_modulus);
        uint64_t* res       = reinterpret_cast<uint64_t*>(&ans.m_data[0]);
        const uint64_t* op1 = reinterpret_cast<const uint64_t*>(&m_data[0]);
        const uint64_t* op2 = reinterpret_cast<const uint64_t*>(&b.m_data[0]);
        intel::hexl::EltwiseAddMod(res, op1, op2, m_data.size(), m_modulus.m_value);
        return ans;
    }
    else {
        auto mv{m_modulus};
        auto ans(*this);
        for (size_t i = 0; i < ans.m_data.size(); ++i)
            ans.m_data[i].ModAddFastEq(b[i], mv);
        return ans;
    }
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::ModAddEq(const NativeVectorT& b) {
    if (m_data.size() != b.m_data.size() || m_modulus != b.m_modulus)
        OPENFHE_THROW(lbcrypto::math_error, "ModAddEq called on NativeVectorT's with different parameters.");
    if constexpr (HEXL_ADD_ENABLE && std::is_same_v<IntegerType, NativeInteger>) {
        uint64_t* op1       = reinterpret_cast<uint64_t*>(&m_data[0]);
        const uint64_t* op2 = reinterpret_cast<const uint64_t*>(&b.m_data[0]);
        intel::hexl::EltwiseAddMod(op1, op1, op2, m_data.size(), m_modulus.m_value);
    }
    else {
        auto mv{m_modulus};
        for (size_t i = 0; i < m_data.size(); ++i)
            m_data[i].ModAddFastEq(b[i], mv);
    }
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::ModMul(const IntegerType& b) const {
    if constexpr (HEXL_MUL_ENABLE && std::is_same_v<IntegerType, NativeInteger>) {
        NativeVectorT ans(m_data.size(), m_modulus);
        uint64_t* res       = reinterpret_cast<uint64_t*>(&ans.m_data[0]);
        const uint64_t* op1 = reinterpret_cast<const uint64_t*>(&m_data[0]);
        intel::hexl::EltwiseFMAMod(res, op1, b.m_value >= m_modulus.m_value ? b.m_value % m_modulus.m_value : b.m_value,
                                   nullptr, m_data.size(), m_modulus.m_value, 1);
        return ans;
    }
    else {
        auto mv{m_modulus};
        auto bv{b};
        auto ans(*this);
        if (bv.m_value >= mv.m_value)
            bv.ModEq(mv);
        auto bconst{bv.PrepModMulConst(mv)};
        for (size_t i = 0; i < ans.m_data.size(); ++i)
            ans[i].ModMulFastConstEq(bv, mv, bconst);
        return ans;
    }
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::ModMulEq(const IntegerType& b) {
    if constexpr (HEXL_MUL_ENABLE && std::is_same_v<IntegerType, NativeInteger>) {
        uint64_t* op1 = reinterpret_cast<uint64_t*>(&m_data[0]);
        intel::hexl::EltwiseFMAMod(op1, op1, b.m_value >= m_modulus.m_value ? b.m_value % m_modulus.m_value : b.m_value,
                                   nullptr, m_data.size(), m_modulus.m_value, 1);
    }
    else {
        auto mv{m_modulus};
        auto bv{b};
        if (bv.m_value >= mv.m_value)
            bv.ModEq(mv);
        auto bconst{bv.PrepModMulConst(mv)};
        for (size_t i = 0; i < m_data.size(); ++i)
            m_data[i].ModMulFastConstEq(bv, mv, bconst);
    }
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::ModMul(const NativeVectorT& b) const {
    if (m_data.size() != b.m_data.size() || m_modulus != b.m_modulus)
        OPENFHE_THROW(lbcrypto::math_error, "ModMul called on NativeVectorT's with different parameters.");
    if constexpr (HEXL_MUL_ENABLE && std::is_same_v<IntegerType, NativeInteger>) {
        NativeVectorT ans(m_data.size(), m_modulus);
        uint64_t* res       = reinterpret_cast<uint64_t*>(&ans.m_data[0]);
        const uint64_t* op1 = reinterpret_cast<const uint64_t*>(&m_data[0]);
        const uint64_t* op2 = reinterpret_cast<const uint64_t*>(&b.m_data[0]);
        intel::hexl::EltwiseMultMod(res, op1, op2, m_data.size(), m_modulus.m_value, 1);
        return ans;
    }
    else {
        auto ans(*this);
        uint32_t size(m_data.size());
        auto mv{m_modulus};
    #ifdef NATIVEINT_BARRET_MOD
        auto mu{m_modulus.ComputeMu()};
        for (uint32_t i = 0; i < size; ++i)
            ans[i].ModMulFastEq(b[i], mv, mu);
    #else
        for (uint32_t i = 0; i < size; ++i)
            ans[i].ModMulFastEq(b[i], mv);
    #endif
        return ans;
    }
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::ModMulEq(const NativeVectorT& b) {
    if (m_data.size() != b.m_data.size() || m_modulus != b.m_modulus)
        OPENFHE_THROW(lbcrypto::math_error, "ModMulEq called on NativeVectorT's with different parameters.");
    if constexpr (HEXL_MUL_ENABLE && std::is_same_v<IntegerType, NativeInteger>) {
        uint64_t* op1       = reinterpret_cast<uint64_t*>(&m_data[0]);
        const uint64_t* op2 = reinterpret_cast<const uint64_t*>(&b[0]);
        intel::hexl::EltwiseMultMod(op1, op1, op2, m_data.size(), m_modulus.m_value, 1);
    }
    else {
        auto mv{m_modulus};
        size_t size{m_data.size()};
    #ifdef NATIVEINT_BARRET_MOD
        auto mu{m_modulus.ComputeMu()};
        for (size_t i = 0; i < size; ++i)
            m_data[i].ModMulFastEq(b[i], mv, mu);
    #else
        for (size_t i = 0; i < size; ++i)
            m_data[i].ModMulFastEq(b[i], mv);
    #endif
    }
    return *this;
}

template <class IntegerType>
void NativeVectorT<IntegerType>::SwitchModulus(const IntegerType& modulus) {
    uint64_t om{m_modulus.m_value};
    NativeVectorT::SetModulus(modulus);
    uint64_t nm{modulus.m_value};
    uint64_t* op1 = reinterpret_cast<uint64_t*>(&m_data[0]);
    if (nm > om)
        intel::hexl::EltwiseCmpAdd(op1, op1, m_data.size(), intel::hexl::CMPINT::NLE, (om >> 1), nm - om);
    else
        intel::hexl::EltwiseCmpSubMod(op1, op1, m_data.size(), nm, intel::hexl::CMPINT::NLE, (om >> 1), (om - nm) % nm);
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::Mod(const IntegerType& modulus) const {
    uint64_t nm{modulus.m_value};
    if (nm == 2)
        return NativeVectorT::ModByTwo();
    uint64_t om{m_modulus.m_value};
    NativeVectorT ans(m_data.size(), m_modulus);
    uint64_t* res       = reinterpret_cast<uint64_t*>(&ans.m_data[0]);
    const uint64_t* op1 = reinterpret_cast<const uint64_t*>(&m_data[0]);
    if (nm > om)
        intel::hexl::EltwiseCmpAdd(res, op1, m_data.size(), intel::hexl::CMPINT::NLE, (om >> 1), nm - om);
    else
        intel::hexl::EltwiseCmpSubMod(res, op1, m_data.size(), nm, intel::hexl::CMPINT::NLE, (om >> 1), (om - nm) % nm);
    return ans;
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::ModEq(const IntegerType& modulus) {
    uint64_t nm{modulus.m_value};
    if (nm == 2)
        return NativeVectorT::ModByTwoEq();
    uint64_t om{m_modulus.m_value};
    uint64_t* op1 = reinterpret_cast<uint64_t*>(&m_data[0]);
    if (nm > om)
        intel::hexl::EltwiseCmpAdd(op1, op1, m_data.size(), intel::hexl::CMPINT::NLE, (om >> 1), nm - om);
    else
        intel::hexl::EltwiseCmpSubMod(op1, op1, m_data.size(), nm, intel::hexl::CMPINT::NLE, (om >> 1), (om - nm) % nm);
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType>::NativeVectorT(usint length, const IntegerType& modulus,
                                          std::initializer_list<std::string> rhs) noexcept
    : m_data(length), m_modulus{modulus} {
    const size_t len = (rhs.size() < m_data.size()) ? rhs.size() : m_data.size();
    for (size_t i = 0; i < len; ++i)
        m_data[i] = *(rhs.begin() + i) % m_modulus;
}

template <class IntegerType>
NativeVectorT<IntegerType>::NativeVectorT(usint length, const IntegerType& modulus,
                                          std::initializer_list<uint64_t> rhs) noexcept
    : m_data(length), m_modulus{modulus} {
    const size_t len = (rhs.size() < m_data.size()) ? rhs.size() : m_data.size();
    for (size_t i = 0; i < len; ++i)
        m_data[i].m_value = BasicInt(*(rhs.begin() + i)) % m_modulus.m_value;
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::operator=(std::initializer_list<std::string> rhs) noexcept {
    const size_t len = rhs.size();
    if (m_data.size() < len)
        m_data.resize(len);
    for (size_t i = 0; i < m_data.size(); ++i) {
        if (i < len) {
            m_data[i] = *(rhs.begin() + i);
            if (m_modulus.m_value != 0)
                m_data[i].m_value = m_data[i].m_value % m_modulus.m_value;
        }
        else {
            m_data[i].m_value = 0;
        }
    }
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::operator=(std::initializer_list<uint64_t> rhs) noexcept {
    const size_t len = rhs.size();
    if (m_data.size() < len)
        m_data.resize(len);
    for (size_t i = 0; i < m_data.size(); ++i) {
        if (i < len) {
            m_data[i].m_value = BasicInt(*(rhs.begin() + i));
            if (m_modulus.m_value != 0)
                m_data[i].m_value = m_data[i].m_value % m_modulus.m_value;
        }
        else {
            m_data[i].m_value = 0;
        }
    }
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::ModAddAtIndex(size_t i, const IntegerType& b) const {
    auto ans(*this);
    ans.at(i).ModAddEq(b, m_modulus);
    return ans;
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::ModAddAtIndexEq(size_t i, const IntegerType& b) {
    NativeVectorT::at(i).ModAddEq(b, m_modulus);
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::ModSub(const IntegerType& b) const {
    auto mv{m_modulus};
    auto bv{b};
    auto ans(*this);
    if (bv.m_value >= mv.m_value)
        bv.ModEq(mv);
    for (size_t i = 0; i < ans.m_data.size(); ++i)
        ans[i].ModSubFastEq(bv, mv);
    return ans;
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::ModSubEq(const IntegerType& b) {
    auto mv{m_modulus};
    auto bv{b};
    if (bv.m_value >= mv.m_value)
        bv.ModEq(mv);
    for (size_t i = 0; i < m_data.size(); ++i)
        m_data[i].ModSubFastEq(bv, mv);
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::ModSub(const NativeVectorT& b) const {
    if (m_data.size() != b.m_data.size() || m_modulus != b.m_modulus)
        OPENFHE_THROW(lbcrypto::math_error, "ModSub called on NativeVectorT's with different parameters.");
    auto mv{m_modulus};
    auto ans(*this);
    for (size_t i = 0; i < ans.m_data.size(); ++i)
        ans[i].ModSubFastEq(b[i], mv);
    return ans;
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::ModSubEq(const NativeVectorT& b) {
    if (m_data.size() != b.m_data.size() || m_modulus != b.m_modulus)
        OPENFHE_THROW(lbcrypto::math_error, "ModSubEq called on NativeVectorT's with different parameters.");
    for (size_t i = 0; i < m_data.size(); ++i)
        m_data[i].ModSubFastEq(b[i], m_modulus);
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::ModByTwo() const {
    auto ans(*this);
    auto halfQ{m_modulus.m_value >> 1};
    for (size_t i = 0; i < ans.m_data.size(); ++i)
        ans[i].m_value = 0x1 & (ans[i].m_value ^ (ans[i].m_value > halfQ));
    return ans;
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::ModByTwoEq() {
    auto halfQ{m_modulus.m_value >> 1};
    for (size_t i = 0; i < m_data.size(); ++i)
        m_data[i].m_value = 0x1 & (m_data[i].m_value ^ (m_data[i].m_value > halfQ));
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::ModExp(const IntegerType& b) const {
    auto mv{m_modulus};
    auto bv{b};
    auto ans(*this);
    if (bv.m_value >= mv.m_value)
        bv.ModEq(mv);
    for (size_t i = 0; i < ans.m_data.size(); ++i)
        ans[i] = ans[i].ModExp(bv, mv);
    return ans;
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::ModExpEq(const IntegerType& b) {
    auto mv{m_modulus};
    auto bv{b};
    if (bv.m_value >= mv.m_value)
        bv.ModEq(mv);
    for (size_t i = 0; i < m_data.size(); ++i)
        m_data[i] = m_data[i].ModExp(bv, mv);
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::MultWithOutMod(const NativeVectorT& b) const {
    if (m_data.size() != b.m_data.size() || m_modulus != b.m_modulus)
        OPENFHE_THROW(lbcrypto::math_error, "ModMul called on NativeVectorT's with different parameters.");
    auto ans(*this);
    for (size_t i = 0; i < ans.m_data.size(); ++i)
        ans[i].m_value = ans[i].m_value * b[i].m_value;
    return ans;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::MultiplyAndRound(const IntegerType& p,
                                                                        const IntegerType& q) const {
    auto halfQ{m_modulus.m_value >> 1};
    auto mv{m_modulus};
    auto ans(*this);
    for (size_t i = 0; i < ans.m_data.size(); ++i) {
        if (ans[i].m_value > halfQ) {
            auto&& tmp{mv - ans[i]};
            ans[i] = mv - tmp.MultiplyAndRound(p, q);
        }
        else {
            ans[i] = ans[i].MultiplyAndRound(p, q).Mod(mv);
        }
    }
    return ans;
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::MultiplyAndRoundEq(const IntegerType& p, const IntegerType& q) {
    auto halfQ{m_modulus.m_value >> 1};
    auto mv{m_modulus};
    for (size_t i = 0; i < m_data.size(); ++i) {
        if (m_data[i].m_value > halfQ) {
            auto&& tmp{mv - m_data[i]};
            m_data[i] = mv - tmp.MultiplyAndRound(p, q);
        }
        else {
            m_data[i] = m_data[i].MultiplyAndRound(p, q).Mod(mv);
        }
    }
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::DivideAndRound(const IntegerType& q) const {
    auto halfQ{m_modulus.m_value >> 1};
    auto mv{m_modulus};
    auto ans(*this);
    for (size_t i = 0; i < ans.m_data.size(); ++i) {
        if (ans[i].m_value > halfQ) {
            auto&& tmp{mv - ans[i]};
            ans[i] = mv - tmp.DivideAndRound(q);
        }
        else {
            ans[i] = ans[i].DivideAndRound(q);
        }
    }
    return ans;
}

template <class IntegerType>
NativeVectorT<IntegerType>& NativeVectorT<IntegerType>::DivideAndRoundEq(const IntegerType& q) {
    auto halfQ{m_modulus.m_value >> 1};
    auto mv{m_modulus};
    for (size_t i = 0; i < m_data.size(); ++i) {
        if (m_data[i].m_value > halfQ) {
            auto&& tmp{mv - m_data[i]};
            m_data[i] = mv - tmp.DivideAndRound(q);
        }
        else {
            m_data[i] = m_data[i].DivideAndRound(q);
        }
    }
    return *this;
}

template <class IntegerType>
NativeVectorT<IntegerType> NativeVectorT<IntegerType>::GetDigitAtIndexForBase(usint index, usint base) const {
    auto ans(*this);
    for (size_t i = 0; i < ans.m_data.size(); ++i)
        ans[i].m_value = static_cast<BasicInt>(ans[i].GetDigitAtIndexForBase(index, base));
    return ans;
}

template class NativeVectorT<NativeInteger>;

}  // namespace intnathexl

#endif
