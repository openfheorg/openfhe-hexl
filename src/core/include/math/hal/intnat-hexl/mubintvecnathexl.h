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
 * This file contains the vector manipulation functionality for native integers
 */

#ifndef LBCRYPTO_MATH_HAL_INTNATHEXL_MUBINTVECNAT_H
#define LBCRYPTO_MATH_HAL_INTNATHEXL_MUBINTVECNAT_H

#include "hexl/hexl.hpp"

#include "math/hal/intnat-hexl/ubintnathexl.h"
#include "math/hal/vector.h"

#include "utils/inttypes.h"
#include "utils/serializable.h"
#include "utils/blockAllocator/xvector.h"

#include <algorithm>
#include <initializer_list>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

/**
 * @namespace intnathexl
 * The namespace of intnathexl
 */
namespace intnathexl {

// Forward declare class and give it an alias for the expected type
template <typename IntType>
class NativeVectorT;
using NativeVector = NativeVectorT<NativeInteger>;

/**
 * @brief The class for representing vectors of native integers.
 */

#if 0  // allocator that reports bytes used.
template <class Tp>
struct NAlloc {
    typedef Tp value_type;
    NAlloc() = default;
    template <class T> NAlloc(const NAlloc<T>&) {}
    Tp* allocate(std::size_t n) {
        n *= sizeof(Tp);
        return static_cast<Tp*>(::operator new(n));
    }
    void deallocate(Tp* p, std::size_t n) {
        std::cout << "deallocating " << n*sizeof*p << " bytes\n";
        ::operator delete(p);
    }
};
template <class T, class U>
bool operator==(const NAlloc<T>&, const NAlloc<U>&) { return true; }
template <class T, class U>
bool operator!=(const NAlloc<T>&, const NAlloc<U>&) { return false; }
#endif

#if 0  // allocator that reports bytes used.
template <class Tp>
struct NAlloc {
    typedef Tp value_type;
    NAlloc() = default;
    template <class T> NAlloc(const NAlloc<T>&) {}
    Tp* allocate(std::size_t n) {
        n *= sizeof(Tp);
        std::cout << "allocating   " << n << " bytes\n";
        return static_cast<Tp*>(::operator new(n));
    }
    void deallocate(Tp* p, std::size_t n) {
        std::cout << "deallocating " << n*sizeof*p << " bytes\n";
        ::operator delete(p);
    }
};
template <class T, class U>
bool operator==(const NAlloc<T>&, const NAlloc<U>&) { return true; }
template <class T, class U>
bool operator!=(const NAlloc<T>&, const NAlloc<U>&) { return false; }
#endif

template <class IntegerType>
class NativeVectorT final : public lbcrypto::BigVectorInterface<NativeVectorT<IntegerType>, IntegerType>,
                            public lbcrypto::Serializable {
private:
    intel::hexl::AlignedVector64<IntegerType> m_data{};
    IntegerType m_modulus{0};

    // function to check if the index is a valid index.
    bool IndexCheck(size_t length) const {
        return length < m_data.size();
    }

public:
    using BasicInt = typename IntegerType::Integer;

    constexpr NativeVectorT() = default;

    static constexpr NativeVectorT Single(const IntegerType& val, const IntegerType& modulus) noexcept {
        return NativeVectorT(1, modulus, val);
    }

    explicit constexpr NativeVectorT(usint length) noexcept : m_data(length) {}

    constexpr NativeVectorT(usint length, const IntegerType& modulus) noexcept : m_data(length), m_modulus{modulus} {}

    constexpr NativeVectorT(usint length, const IntegerType& modulus, const IntegerType& val) noexcept
        : m_data(length, val.Mod(modulus)), m_modulus{modulus} {}

    constexpr NativeVectorT(const NativeVectorT& v) noexcept : m_data{v.m_data}, m_modulus{v.m_modulus} {}

    constexpr NativeVectorT(NativeVectorT&& v) noexcept
        : m_data{std::move(v.m_data)}, m_modulus{std::move(v.m_modulus)} {}

    NativeVectorT(usint length, const IntegerType& modulus, std::initializer_list<std::string> rhs) noexcept;

    NativeVectorT(usint length, const IntegerType& modulus, std::initializer_list<uint64_t> rhs) noexcept;

    NativeVectorT& operator=(const NativeVectorT& rhs) noexcept {
        m_modulus = rhs.m_modulus;
        if (m_data.size() >= rhs.m_data.size()) {
            std::copy(rhs.m_data.begin(), rhs.m_data.end(), m_data.begin());
            if (m_data.size() > rhs.m_data.size())
                m_data.resize(rhs.m_data.size());
            return *this;
        }
        m_data = rhs.m_data;
        return *this;
    }

    NativeVectorT& operator=(NativeVectorT&& rhs) noexcept {
        m_modulus = std::move(rhs.m_modulus);
        m_data    = std::move(rhs.m_data);
        return *this;
    }

    NativeVectorT& operator=(std::initializer_list<std::string> rhs) noexcept;

    NativeVectorT& operator=(std::initializer_list<uint64_t> rhs) noexcept;

    constexpr NativeVectorT& operator=(uint64_t val) {
        std::fill(m_data.begin(), m_data.end(), 0);
        m_data.at(0) = val;
        return *this;
    }

    IntegerType& at(size_t i) {
        if (!NativeVectorT::IndexCheck(i))
            OPENFHE_THROW("NativeVectorT index out of range");
        return m_data[i];
    }

    const IntegerType& at(size_t i) const {
        if (!NativeVectorT::IndexCheck(i))
            OPENFHE_THROW("NativeVectorT index out of range");
        return m_data[i];
    }

    IntegerType& operator[](size_t idx) {
        return m_data[idx];
    }

    const IntegerType& operator[](size_t idx) const {
        return m_data[idx];
    }

    void SetModulus(const IntegerType& value) {
        if (value.GetMSB() > MAX_MODULUS_SIZE)
            OPENFHE_THROW("NativeVectorT supports only modulus size <=  " + std::to_string(MAX_MODULUS_SIZE) + " bits");
        m_modulus.m_value = value.m_value;
    }

    void SwitchModulus(const IntegerType& value);
    void LazySwitchModulus(const IntegerType& value);

    NativeVectorT& MultAccEqNoCheck(const NativeVectorT& V, const IntegerType& I);

    const IntegerType& GetModulus() const {
        return m_modulus;
    }

    size_t GetLength() const {
        return m_data.size();
    }

    NativeVectorT Mod(const IntegerType& modulus) const;
    NativeVectorT& ModEq(const IntegerType& modulus);

    NativeVectorT ModAdd(const IntegerType& b) const;
    NativeVectorT& ModAddEq(const IntegerType& b);

    NativeVectorT ModAddAtIndex(size_t i, const IntegerType& b) const;
    NativeVectorT& ModAddAtIndexEq(size_t i, const IntegerType& b);

    NativeVectorT ModAdd(const NativeVectorT& b) const;
    NativeVectorT& ModAddEq(const NativeVectorT& b);
    NativeVectorT& ModAddNoCheckEq(const NativeVectorT& b) {
        if constexpr (HEXL_ADD_ENABLE && std::is_same_v<IntegerType, NativeInteger>) {
            uint64_t* op1       = reinterpret_cast<uint64_t*>(&m_data[0]);
            const uint64_t* op2 = reinterpret_cast<const uint64_t*>(&b.m_data[0]);
            intel::hexl::EltwiseAddMod(op1, op1, op2, m_data.size(), m_modulus.m_value);
        }
        else {
            size_t size{m_data.size()};
            auto mv{m_modulus};
            for (size_t i = 0; i < size; ++i)
                m_data[i].ModAddFastEq(b[i], mv);
        }
        return *this;
    }

    NativeVectorT ModSub(const IntegerType& b) const;
    NativeVectorT& ModSubEq(const IntegerType& b);

    NativeVectorT ModSub(const NativeVectorT& b) const;
    NativeVectorT& ModSubEq(const NativeVectorT& b);

    NativeVectorT ModMul(const IntegerType& b) const;
    NativeVectorT& ModMulEq(const IntegerType& b);

    NativeVectorT ModMul(const NativeVectorT& b) const;
    NativeVectorT& ModMulEq(const NativeVectorT& b);
    NativeVectorT& ModMulNoCheckEq(const NativeVectorT& b) {
        if constexpr (HEXL_MUL_ENABLE && std::is_same_v<IntegerType, NativeInteger>) {
            uint64_t* op1       = reinterpret_cast<uint64_t*>(&m_data[0]);
            const uint64_t* op2 = reinterpret_cast<const uint64_t*>(&b[0]);
            intel::hexl::EltwiseMultMod(op1, op1, op2, m_data.size(), m_modulus.m_value, 1);
        }
        else {
            size_t size{m_data.size()};
            auto mv{m_modulus};
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
    NativeVectorT MultWithOutMod(const NativeVectorT& b) const;

    NativeVectorT ModExp(const IntegerType& b) const;
    NativeVectorT& ModExpEq(const IntegerType& b);

    NativeVectorT ModInverse() const {
        size_t size{m_data.size()};
        auto mv{m_modulus};
        NativeVectorT ans(size, mv);
        for (size_t i{0}; i < size; ++i)
            ans[i] = m_data[i].ModInverse(mv);
        return ans;
    }

    NativeVectorT& ModInverseEq() {
        size_t size{m_data.size()};
        auto mv{m_modulus};
        for (size_t i{0}; i < size; ++i)
            m_data[i] = m_data[i].ModInverse(mv);
        return *this;
    }

    NativeVectorT ModByTwo() const;
    NativeVectorT& ModByTwoEq();

    NativeVectorT MultiplyAndRound(const IntegerType& p, const IntegerType& q) const;
    NativeVectorT& MultiplyAndRoundEq(const IntegerType& p, const IntegerType& q);

    NativeVectorT DivideAndRound(const IntegerType& q) const;
    NativeVectorT& DivideAndRoundEq(const IntegerType& q);

    NativeVectorT GetDigitAtIndexForBase(usint index, usint base) const;

    template <class IntegerType_c>
    friend std::ostream& operator<<(std::ostream& os, const NativeVectorT<IntegerType_c>& ptr_obj) {
        auto len = ptr_obj.m_data.size();
        os << "[";
        for (usint i = 0; i < len; i++) {
            os << ptr_obj.m_data[i];
            os << ((i == (len - 1)) ? "]" : " ");
        }
        os << " modulus: " << ptr_obj.m_modulus;
        return os;
    }

    template <class Archive>
    typename std::enable_if<!cereal::traits::is_text_archive<Archive>::value, void>::type save(
        Archive& ar, std::uint32_t const version) const {
        ::cereal::size_type size = m_data.size();
        ar(size);
        if (size > 0) {
            ar(::cereal::binary_data(m_data.data(), size * sizeof(IntegerType)));
        }
        ar(m_modulus);
    }

    template <class Archive>
    typename std::enable_if<cereal::traits::is_text_archive<Archive>::value, void>::type save(
        Archive& ar, std::uint32_t const version) const {
        ar(::cereal::make_nvp("v", m_data));
        ar(::cereal::make_nvp("m", m_modulus));
    }

    template <class Archive>
    typename std::enable_if<!cereal::traits::is_text_archive<Archive>::value, void>::type load(
        Archive& ar, std::uint32_t const version) {
        if (version > SerializedVersion()) {
            OPENFHE_THROW("serialized object version " + std::to_string(version) +
                          " is from a later version of the library");
        }
        ::cereal::size_type size;
        ar(size);
        m_data.resize(size);
        if (size > 0) {
            auto* data = reinterpret_cast<IntegerType*>(malloc(size * sizeof(IntegerType)));
            ar(::cereal::binary_data(data, size * sizeof(IntegerType)));
            for (::cereal::size_type i = 0; i < size; i++) {
                m_data[i] = data[i];
            }
            free(data);
        }
        ar(m_modulus);
    }

    template <class Archive>
    typename std::enable_if<cereal::traits::is_text_archive<Archive>::value, void>::type load(
        Archive& ar, std::uint32_t const version) {
        if (version > SerializedVersion()) {
            OPENFHE_THROW("serialized object version " + std::to_string(version) +
                          " is from a later version of the library");
        }
        ar(::cereal::make_nvp("v", m_data));
        ar(::cereal::make_nvp("m", m_modulus));
    }

    std::string SerializedObjectName() const override {
        return "NativeVectorT";
    }

    static uint32_t SerializedVersion() {
        return 1;
    }
};

}  // namespace intnathexl

namespace cereal {

//! Serialization for vector of NativeInteger

template <class Archive, class A>
inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, std::vector<intnathexl::NativeIntegerT<uint64_t>, A> const& vec) {
    ar(make_size_tag(static_cast<cereal::size_type>(vec.size())));  // number of elements
    for (const auto& v : vec) {
        ar(v.ConvertToInt());
    }
}

#if defined(HAVE_INT128)
template <class Archive, class A>
inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar,
                                      std::vector<intnathexl::NativeIntegerT<unsigned __int128>, A> const& vec) {
    ar(make_size_tag(static_cast<cereal::size_type>(vec.size())));  // number of elements
    constexpr unsigned __int128 mask = (static_cast<unsigned __int128>(1) << 64) - 1;
    for (const auto& v : vec) {
        uint64_t vec[2];
        unsigned __int128 int128 = v.ConvertToInt();
        vec[0]                   = int128 & mask;  // least significant word
        vec[1]                   = int128 >> 64;   // most significant word
        ar(vec);
    }
}
#endif

//! Deserialization for vector of NativeInteger

template <class Archive, class A>
inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, std::vector<intnathexl::NativeIntegerT<uint64_t>, A>& vec) {
    cereal::size_type size;
    ar(make_size_tag(size));
    vec.resize(static_cast<size_t>(size));
    for (auto& v : vec) {
        uint64_t b;
        ar(b);
        v = b;
    }
}

#if defined(HAVE_INT128)
template <class Archive, class A>
inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, std::vector<intnathexl::NativeIntegerT<unsigned __int128>, A>& vec) {
    cereal::size_type size;
    ar(make_size_tag(size));
    vec.resize(static_cast<size_t>(size));
    for (auto& v : vec) {
        uint64_t vec[2];
        ar(vec);
        v = vec[1];  // most significant word
        v <<= 64;
        v += vec[0];  // least significant word
    }
}
#endif
}  // namespace cereal

#endif  // LBCRYPTO_MATH_HAL_INTNATHEXL_MUBINTVECNAT_H
