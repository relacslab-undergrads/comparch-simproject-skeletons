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
 * File         : integrity_tree.h
 * Author       : Hyokeun Lee, RELACS Research Group
 * Date         : 11/22/2025
 * Description  : MEE's Integrity tree module (including data MAC) factory
 *********************************************************************************************/
#ifndef _MEE_INTEGRITY_TREE_HH_ 
#define _MEE_INTEGRITY_TREE_HH_

#include "global_types.h"
#include "global_defs.h"

#include "memreq_info.h"

class macsim_c;
class data_mac_c;

class integrity_tree_c
{
  public:
    integrity_tree_c(macsim_c* simBase, int _id, Addr data_region_st, 
            Addr data_region_ed, Addr _offset);

    virtual void gen_metadata_rd(mem_req_list& req_list, mem_req_ptr req) = 0;
    virtual void gen_metadata_wr(mem_req_list& req_list, mem_req_ptr req) = 0;

    Addr get_size();
    Addr get_total_size();

    Counter get_latency();

  protected:
    /**
     * Generate 64B leaf index by using counter index
     * @param paddr: physical address of normal data
     */
    virtual Addr get_leaf_idx(Addr paddr) = 0;

    /**
     * Generate parent node's index using child node's index
     * @param nd_idx: a child node index
     */
    virtual Addr get_parent_idx(Addr nd_idx) = 0;

    /**
     * Generate displaced request ID
     * @param id: ID of host request 
     */
    int gen_displaced_req_id(int id);

    macsim_c* m_simBase;
    int m_id;

    data_mac_c* m_dmac;     // data should also be protected

    Addr m_base_addr;       // base address for tree nodes
    Addr m_size;            // size of all tree nodes

    Addr m_page_sz;
    Addr m_line_sz;

    Addr m_data_per_leaf;   // data (bytes) covered by a leaf
    Addr m_num_full_leaf;   // # leaves to span a complete tree 
    Addr m_num_lv;          // tree height
    Addr m_num_intermed;    // number of nodes except for leaves and root
    uint32_t m_degree;      // degree of tree (assume 8-ary)

    Counter m_latency;      // latency of MAC sourced from knob

    uint64_t power_of_8(uint64_t n);
};

class tree_factory_c
{
  public:
    tree_factory_c() {}
    ~tree_factory_c() {}

    static integrity_tree_c* create(macsim_c* simBase, int _id, 
            std::string mt_style, Addr data_region_st, 
            Addr data_region_ed, Addr _offset);
};

#endif
