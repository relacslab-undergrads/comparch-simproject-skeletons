/*
Copyright (c) <2025>, <RELACS> All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions
and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.

Neither the name of the <Georgia Institue of Technology> nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

/**********************************************************************************************
 * File         : encryption.cc
 * Author       : Hyokeun Lee, RELACS Research Group
 * Date         : 11/22/2025
 * Description  : MEE's encryption module factory
 *********************************************************************************************/
#include "assert_macros.h"
#include "debug_macros.h"

#include "macsim.h"
#include "encryption.h"
#include "counter_mode.h"

#include "all_knobs.h"

encryption_c::encryption_c(macsim_c* simBase, int _id): 
    m_simBase(simBase), m_id(_id) {
  m_page_sz = *KNOB(KNOB_PAGE_SIZE);
  m_line_sz = *KNOB(KNOB_LLC_LINE_SIZE);
  m_latency = *KNOB(KNOB_AES_LATENCY);

  if (m_page_sz < m_line_sz) {
    fprintf(stderr, "Page size must be larger than cacheline size\n");
    exit(1);
  }
}

bool
encryption_c::has_metadata() {
  return m_has_metadata;
}

Addr
encryption_c::get_size() {
  return m_size;
}

Counter
encryption_c::get_latency() {
  return m_latency;
}

int 
encryption_c::gen_displaced_req_id(int id) {
  return ((1 << 29) + id);
}

encryption_c*
encrypt_factory_c::create(macsim_c* simBase, int _id, 
        std::string encrypt_style, Addr data_region_st, 
        Addr data_region_ed, Addr _offset) {
  encryption_c* ret = NULL;

  if (encrypt_style == "counter-mode") {
    ret = new counter_mode_c(simBase, _id, data_region_st, 
                             data_region_ed, _offset); 
  } else if (encrypt_style == "counterless") {
    fprintf(stderr, "Under construction\n");
    exit(1);
  } else {
    fprintf(stderr, "Unknown encryption scheme: %s\n", encrypt_style.c_str());
    exit(1);
  }

  return ret;
}
