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
 * File         : encryption.h
 * Author       : Hyokeun Lee, RELACS Research Group
 * Date         : 11/22/2025
 * Description  : MEE's encryption module factory
 *********************************************************************************************/
#ifndef _MEE_ENCRYPTION_HH_ 
#define _MEE_ENCRYPTION_HH_

#include "global_types.h"
#include "global_defs.h"

#include "memreq_info.h"

class macsim_c;

class encryption_c
{
  public:
    encryption_c(macsim_c* simBase, int _id);

    virtual void gen_metadata_rd(mem_req_list& req_list, mem_req_ptr req) {};
    virtual void gen_metadata_wr(mem_req_list& req_list, mem_req_ptr req) {};

    bool has_metadata();
    Addr get_size();
    Counter get_latency();

  protected:
    /**
     * Generate a 64B metadata block index
     * @param paddr: physical address of normal data 
     */
    virtual Addr get_metadata_bidx(Addr paddr) { return 0; }

    /**
     * Generate displaced request ID
     * @param id: ID of host request 
     */
    int gen_displaced_req_id(int id);

    macsim_c* m_simBase;
    int m_id;

    Addr m_base_addr;       // base address for encrypt metadata
    Addr m_size;            // size of all metadata

    Addr m_page_sz;
    Addr m_line_sz;

    bool m_has_metadata;    // true for counter-mode

    Counter m_latency;      // latency of encryption sourced from knob
};

class encrypt_factory_c
{
  public:
    encrypt_factory_c() {}
    ~encrypt_factory_c() {}

    static encryption_c* create(macsim_c* simBase, int _id, 
            std::string encrypt_style, Addr data_region_st, 
            Addr data_region_ed, Addr _offset);
};

#endif
