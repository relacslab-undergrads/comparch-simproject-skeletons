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
 * File         : data_mac.cc
 * Author       : Hyokeun Lee, RELACS Research Group
 * Date         : 11/22/2025
 * Description  : MEE's data MAC module
 *********************************************************************************************/
#include "assert_macros.h"
#include "debug_macros.h"

#include "macsim.h"
#include "mee.h"
#include "data_mac.h"

#include "all_knobs.h"
#include "statistics.h"

#define DEBUG_MEE(args...) _DEBUG(*m_simBase->m_knobs->KNOB_DEBUG_MEE, ##args)

data_mac_c::data_mac_c(macsim_c* simBase, int _id, Addr data_region_st,
        Addr data_region_ed, Addr _offset): m_simBase(simBase), m_id(_id) {
  m_page_sz = *KNOB(KNOB_PAGE_SIZE);
  m_line_sz = *KNOB(KNOB_LLC_LINE_SIZE);
  m_latency = *KNOB(KNOB_AES_LATENCY);

  // TODO: set up all necessary member variables
}

void
data_mac_c::gen_metadata_rd(mem_req_list& req_list, mem_req_ptr mem_req) {
  ASSERT(!mem_req->m_is_metadata);
  const Addr line_offset_bits = ceil_log2(m_line_sz);

  // TODO: generate metadata read requst and push to list
  // NOTE: host req's m_id is displaced to assign generated req's m_id
}

void
data_mac_c::gen_metadata_wr(mem_req_list& req_list, mem_req_ptr mem_req) {
  ASSERT(!mem_req->m_is_metadata);
  const Addr line_offset_bits = ceil_log2(m_line_sz);

  // TODO: generate metadata write requst and push to list
  // NOTE: host req's m_id is displaced to assign generated req's m_id
}

Addr
data_mac_c::get_size() {
  return m_size;
}

Counter
data_mac_c::get_latency() {
  return m_latency;
}

Addr
data_mac_c::get_mac_bidx(Addr paddr) {
  //TODO: correct the folllowing return val as block index formula
  return 0;
}

int 
data_mac_c::gen_displaced_req_id(int id) {
  return ((2 << 29) + id);
}

