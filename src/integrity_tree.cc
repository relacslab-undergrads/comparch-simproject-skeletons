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
 * File         : integrity_tree.cc
 * Author       : Hyokeun Lee, RELACS Research Group
 * Date         : 11/22/2025
 * Description  : MEE's Integrity tree module (including data MAC) factory
 *********************************************************************************************/
#include "assert_macros.h"
#include "debug_macros.h"

#include "macsim.h"
#include "integrity_tree.h"
#include "bonsai_mt.h"
#include "toc_mt.h"
#include "data_mac.h"

#include "all_knobs.h"

integrity_tree_c::integrity_tree_c(macsim_c* simBase, int _id, 
        Addr data_region_st, Addr data_region_ed, Addr _offset): 
    m_simBase(simBase), m_id(_id) {
  m_page_sz = *KNOB(KNOB_PAGE_SIZE);
  m_line_sz = *KNOB(KNOB_LLC_LINE_SIZE);
  
  // in addition to tree, still need data MAC for protecting data
  m_dmac = new data_mac_c(m_simBase, m_id, data_region_st, 
                          data_region_ed, _offset);

  if (m_page_sz < m_line_sz) {
    fprintf(stderr, "Page size must be larger than cacheline size\n");
    exit(1);
  }
}

Addr
integrity_tree_c::get_size() {
  return m_size;
}

Addr
integrity_tree_c::get_total_size() {
  return (m_size + m_dmac->get_size());
}

Counter
integrity_tree_c::get_latency() {
  return m_latency;
}

uint64_t
integrity_tree_c::power_of_8(uint64_t n) {
  ASSERT(m_degree == 8);

  if (n == 0)
    return 1;
  return (m_degree << (3 * (n-1)));
}

int 
integrity_tree_c::gen_displaced_req_id(int id) {
  return ((3 << 29) + id);
}

integrity_tree_c*
tree_factory_c::create(macsim_c* simBase, int _id, std::string mt_style,
        Addr data_region_st, Addr data_region_ed, Addr _offset) {
  integrity_tree_c* ret = NULL;

  if (mt_style == "Bonsai") {
    ret = new bonsai_mt_c(simBase, _id, data_region_st, 
                          data_region_ed, _offset);
  } else if (mt_style == "ToC") {
    fprintf(stderr, "Under construction\n");
    exit(1);
  } else {
    fprintf(stderr, "Unknown tree scheme: %s\n", mt_style.c_str());
    exit(1);
  }

  return ret;
}
