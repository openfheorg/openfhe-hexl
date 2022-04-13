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
  Test file for checking if the interface class is implemented correctly Intel HEXL specific DCRT Polynomial Object
 */

 //==================================================================================
 // This file is included only if WITH_INTEL_HEXL is set to ON in CMakeLists.txt
 //==================================================================================
#ifdef WITH_INTEL_HEXL

#include <fstream>
#include <memory>

#include "hexl/hexl.hpp"

#include "math/hal.h"
#include "lattice/hal/hexl/hexldcrtpoly.h"
#include "utils/debug.h"

using std::shared_ptr;
using std::string;

namespace lbcrypto {

// used for CKKS rescaling
template<typename VecType>
void HexlDCRTPoly<VecType>::DropLastElementAndScale(
   const std::vector<NativeInteger> &QlQlInvModqlDivqlModq,
   const std::vector<NativeInteger> &QlQlInvModqlDivqlModqPrecon,
   const std::vector<NativeInteger> &qlInvModq,
   const std::vector<NativeInteger> &qlInvModqPrecon) {
 usint sizeQl = this->m_vectors.size();

 // last tower that will be dropped
 PolyType lastPoly(this->m_vectors[sizeQl - 1]);

 // drop the last tower
 this->DropLastElement();

 lastPoly.SetFormat(Format::COEFFICIENT);
 HexlDCRTPoly extra(this->m_params, COEFFICIENT, true);

#pragma omp parallel for
 for (usint i = 0; i < extra.m_vectors.size(); i++) {
   auto temp = lastPoly;
   // SwitchModulus is HEXL-accelerated
   temp.SwitchModulus(this->m_vectors[i].GetModulus(),
                      this->m_vectors[i].GetRootOfUnity());
   extra.m_vectors[i] = (temp *= QlQlInvModqlDivqlModq[i]);
 } // omp threaded loop

 if (this->GetFormat() == Format::EVALUATION)
   extra.SetFormat(Format::EVALUATION);

 usint ringDim = this->GetRingDimension();
 for (usint i = 0; i < this->m_vectors.size(); i++) {
   const NativeInteger &qi = this->m_vectors[i].GetModulus();
   PolyType &m_veci = this->m_vectors[i];
   const PolyType &extra_m_veci = extra.m_vectors[i];
   const auto multOp = qlInvModq[i];
   uint64_t *op1 = reinterpret_cast<uint64_t *>(&m_veci[0]);
   uint64_t op2 = multOp.ConvertToInt();
   const uint64_t *op3 = reinterpret_cast<const uint64_t *>(&extra_m_veci[0]);
   intel::hexl::EltwiseFMAMod(op1, op1, op2, op3, ringDim, qi.ConvertToInt(),
                              1);
 }

 this->SetFormat(Format::EVALUATION);
} // DCRTPolyImpl<VecType>::DropLastElementAndScale



// used for BGVrns modulus switching
template <typename VecType>
void HexlDCRTPoly<VecType>::ModReduce(
    const NativeInteger &t, const std::vector<NativeInteger> &tModqPrecon,
    const NativeInteger &negtInvModq, const NativeInteger &negtInvModqPrecon,
    const std::vector<NativeInteger> &qlInvModq,
    const std::vector<NativeInteger> &qlInvModqPrecon) {
  usint sizeQl = this->m_vectors.size();

  // last tower that will be dropped
  PolyType delta(this->m_vectors[sizeQl - 1]);

  // Pull tower to be dropped in COEFFICIENT FORMAT
  delta.SetFormat(Format::COEFFICIENT);

  this->DropLastElement();

  // loop invariants, regardless of Format
  usint ringDim = this->GetRingDimension();
  const auto multOp = t;
  uint64_t t_u64 = multOp.ConvertToInt();

  if (this->GetFormat() == Format::EVALUATION) {
    HexlDCRTPoly extra(this->m_params, COEFFICIENT, true);

    delta *= negtInvModq;

    #pragma omp parallel for
    for (usint i = 0; i < this->m_vectors.size(); i++) {
      auto temp = delta;
      // SwitchModulus is HEXL-accelerated
      temp.SwitchModulus(this->m_vectors[i].GetModulus(),
                         this->m_vectors[i].GetRootOfUnity());
      extra.m_vectors[i] = temp;
    }

    extra.SetFormat(Format::EVALUATION);

    // XXX - jbates - #pragma omp parallel for
    for (usint i = 0; i < this->m_vectors.size(); i++) {

      //  Portable code:  extra.m_vectors[i] *= t;
      //                  this->m_vectors[i] += extra.m_vectors[i];
      {
        // first cooking of the params
        PolyType &e_veci        = extra.m_vectors[i];
        const NativeInteger &qi = extra.m_vectors[i].GetModulus();
        PolyType &m_veci        = this->m_vectors[i];

        // second cooking of the params
        const uint64_t *op1     = reinterpret_cast<const uint64_t *>(&e_veci[0]);
        uint64_t        op2     = t_u64;
        uint64_t       *op3     = reinterpret_cast<uint64_t *>(&m_veci[0]);
        uint64_t        modulus = qi.ConvertToInt();
        
        intel::hexl::EltwiseFMAMod(op3, op1, op2, op3, ringDim, modulus, 1);
      }
    }

    #pragma omp parallel for
    for (usint i = 0; i < this->m_vectors.size(); i++) {
      this->m_vectors[i] *= qlInvModq[i];
    }

  } else {

    delta *= negtInvModq;

    // XXX - jbates - #pragma omp parallel for
    for (usint i = 0; i < this->m_vectors.size(); i++) {
      auto temp = delta;
      // SwitchModulus is HEXL-accelerated
      temp.SwitchModulus(this->m_vectors[i].GetModulus(),
                         this->m_vectors[i].GetRootOfUnity());

      //  Portable code: this->m_vectors[i] += (temp *= t);
      {
        // first cooking of the params
        const NativeInteger &qi = this->m_vectors[i].GetModulus();
        PolyType &m_veci        = this->m_vectors[i];

        // second cooking of the params
        const uint64_t *op1     = reinterpret_cast<const uint64_t *>(&temp[0]);
        uint64_t        op2     = t_u64;
        uint64_t       *op3     = reinterpret_cast<uint64_t *>(&m_veci[0]);
        uint64_t        modulus = qi.ConvertToInt();

        intel::hexl::EltwiseFMAMod(op3, op1, op2, op3, ringDim, modulus, 1);
      }
    }

    #pragma omp parallel for
    for (usint i = 0; i < this->m_vectors.size(); i++) {
      this->m_vectors[i] *= qlInvModq[i];
    }
  }
}


template class HexlDCRTPoly<M2Vector>;
template class HexlDCRTPoly<M4Vector>;
#ifdef WITH_NTL
template class HexlDCRTPoly<M6Vector>;
#endif

} // namespace lbcrypto

#endif // WITH_INTEL_HEXL
