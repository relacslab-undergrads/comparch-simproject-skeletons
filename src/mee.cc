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
 * File         : mee.cc
 * Author       : Hyokeun Lee, RELACS Research Group
 * Date         : 11/22/2025
 * Description  : Memory encryption engine (MEE)
 *********************************************************************************************/

#include "assert_macros.h"
#include "debug_macros.h"

#include "macsim.h"
#include "dram_ctrl.h"

#include "mee.h"
#include "encryption.h"
#include "counter_mode.h"
#include "integrity_tree.h"
#include "bonsai_mt.h"
#include "toc_mt.h"
#include "data_mac.h"

#include "all_knobs.h"
#include "statistics.h"

#define DEBUG_MEE(args...) _DEBUG(*m_simBase->m_knobs->KNOB_DEBUG_MEE, ##args)

mem_encrypt_engine_c::mem_encrypt_engine_c(macsim_c* simBase): 
    m_simBase(simBase) {
  // sanity check of modes
  mee_mode = KNOB(KNOB_MEE_MODE)->getValue();
  if (mee_mode!="ENCRYPT_ONLY" &&
      mee_mode!="ENCRYPT_DMAC" &&
      mee_mode!="ENCRYPT_TREE") {
    fprintf(stderr, "Unknown mode! Options: ENCRYPT_{ONLY, DMAC, TREE}");
    exit(1);
  }
}

void
mem_encrypt_engine_c::init(int id) {
  m_ctrl = DRAM_CTRL[id];
  m_id = id;

  // TODO: data region starts at 0 and ends with MEMORY_SIZE knob

  // TODO: instatiate encryption, data mac, and tree objects, where
  // encryption & tree are generated with {encrypt, integrity_tree}_factory_c
}

void
mem_encrypt_engine_c::trigger(mem_req_list& req_list, mem_req_ptr trig_req) {
  DEBUG_MEE("MEE[%d] is triggered by req <0x%llx, %s> [%p]\n", m_id,
            trig_req->m_addr, is_read(trig_req)? "RD":"WR", trig_req);

  ASSERT(!trig_req->m_is_metadata);
  trig_req->m_is_trigger_req = true;

  // TODO: generate metadata fetch requests (B/W burden modeling)

  // TODO: record generated requests in tracking list 
}

void
mem_encrypt_engine_c::proc_resp_meta(mem_req_list& req_list, 
                                     mem_req_ptr meta_resp) {
  DEBUG_MEE("MEE[%d] Metadata fetch response <0x%llx> of request "
          "<0x%llx, %s> has arrived [%p - %p]\n", m_id ,
          meta_resp->m_addr, meta_resp->m_parent->m_addr, 
          is_read(meta_resp->m_parent)? "RD":"WR",
          meta_resp->m_parent, meta_resp);
  ASSERT(meta_resp->m_is_metadata && is_read(meta_resp));

  // TODO: process metadata fetch response: 
  // check whether triggered one has returned, 
  // generate metadata update reqs (B/W burden modeling)...
}

void
mem_encrypt_engine_c::check_host_resp_ready(mem_req_ptr mem_req) {
  assert(!mem_req->m_is_metadata && is_read(mem_req));
  ASSERT(mem_req->m_is_trigger_req);

  // set response flag
  mem_req->m_is_trigger_resp = true;

  // TODO: check whether host is ready or not
  // if ready insert to tmp_output_buffer (latency modeling)
  // if not ready pend until metadata fetch is completed
}

void
mem_encrypt_engine_c::set_ctrl(dram_c* _ctrl) {
  m_ctrl = _ctrl;
}

Counter
mem_encrypt_engine_c::get_cpath_latency() {
  Counter ret = ULLONG_MAX;
  if (mee_mode == "ENCRYPT_ONLY") 
    ret = m_encrypt->get_latency();
  else if (mee_mode == "ENCRYPT_DMAC")
    ret = m_encrypt->get_latency() + m_dmac->get_latency();
  else if (mee_mode == "ENCRYPT_TREE")
    ret = m_encrypt->get_latency() + m_tree->get_latency();

  ASSERT(ret < ULLONG_MAX);
  return ret;
}

void
mem_encrypt_engine_c::free_metawr(mem_req_ptr meta_wr) {
  ASSERT(is_read(meta_wr) == false);
  delete meta_wr;
}

uint64_t floor_log2(uint64_t x) {
//  ASSERT(x > 0);

  uint64_t ux = x;
  int y = 0;
  if (x & 0xffffffff00000000ULL) {
    y += 32;
    ux >>= 32;
  }

  if (ux & 0xf0ULL) {
    y += 4;
    ux >>= 4;
  } else if (ux & 0xcULL) {
    y += 2;
    ux >>= 2;
  } else if (ux & 0x2ULL) {
    y += 1;
  }

  return y;
}

uint64_t ceil_log2(const uint64_t& x) {
  if (x==1)
    return 0;

  return (floor_log2(x - 1) + 1);
}

bool is_read(mem_req_ptr mem_req) {
  bool ret = true;

  // MRT_STORE is a read at controller as it needs RMW at cache
  if (mem_req->m_type == MRT_WB)  
    ret = false;

  return ret;
}

