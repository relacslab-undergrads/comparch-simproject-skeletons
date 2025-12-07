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
 * File         : mee.h
 * Author       : Hyokeun Lee, RELACS Research Group
 * Date         : 11/22/2025
 * Description  : Memory encryption engine (MEE)
 *********************************************************************************************/
#ifndef _MEE_HH_ 
#define _MEE_HH_

#include "global_types.h"
#include "global_defs.h"

#include "memreq_info.h"

class macsim_c;
class encryption_c;
class integrity_tree_c;
class data_mac_c;
class dram_c;

class mem_encrypt_engine_c
{
  public:
    mem_encrypt_engine_c(macsim_c* simBase);

    /**
     * Initialize the engine's components
     */
    void init(int id);

    /**
     * Trigger metadata requester generation 
     *
     * @param req_list Return request list for metadata fetch
     * @param trig_req The request that triggers fetch       
     */
    void trigger(mem_req_list &req_list, mem_req_ptr trig_req);

    /**
     * Process fetched metadata to:
     * 1) check response readiness of original host request
     * 2) generate possible metadata update requests
     *
     * @param req_list  Return request list for metadata update
     * @param meta_resp Responded metadata to be processed 
     */
    void proc_resp_meta(mem_req_list &req_list, mem_req_ptr meta_resp);

    /**
     * Check readiness upon the response of host request
     *
     * @param mem_req The host read request just returned
     */
    void check_host_resp_ready(mem_req_ptr mem_req);

    /**
     * Set the pointer to dram controller
     */
    void set_ctrl(dram_c* _ctrl);

    /**
     * Return critical path latency for processing this packet
     */
    Counter get_cpath_latency();

    /**
     * Push host request's resp to response queue
     */
    void push_host_resp(mem_req_ptr mem_req);

    /**
     * Free (=delete) metadata write request
     */
    void free_metawr(mem_req_ptr metawr_req);

  protected:
    /* Engine components generating metadata requests */
    encryption_c*     m_encrypt; // encrypt metadata request generator
    integrity_tree_c* m_tree;    // integrity tree metadata request generator
    data_mac_c*       m_dmac;    // data MAC request generator

    meta_map meta_track_list; // tracking list for metadata reqs and a host req

    /* Parent components */
    macsim_c* m_simBase;
    
    dram_c* m_ctrl; // parent controller
    int m_id;       // controller channel ID

    std::string mee_mode; // modes of MEE
};

// Utility functions
uint64_t floor_log2(uint64_t x); 
uint64_t ceil_log2(const uint64_t& x); 

bool is_read(mem_req_ptr mem_req); 

#endif

