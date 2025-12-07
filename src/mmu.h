/*
Copyright (c) <2012>, <Georgia Institute of Technology> All rights reserved.

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
 * File         : mmu.h
 * Author       : HPArch Research Group
 * Date         : 1/30/2019
 * Modified     : 12/11/2022 (++CAPP, process-wise PTW, assume no data shring)
 * Description  : Memory Management Unit
 *********************************************************************************************/

#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

#include <cstdlib>
#include <cmath>
#include <map>
#include <unordered_map>
#include <memory>
#include <utility>
#include <tuple>
#include <set>
#include <unordered_set>
#include <random>

#include "macsim.h"
#include "uop.h"
#include "tlb.h"

class MMU  // Memory Management Unit
{
private:
  class PageDescriptor
  {
  public:
    PageDescriptor(Addr _frame_number) : frame_number(_frame_number) { }
    PageDescriptor() { }
    Addr frame_number;
  };

  class FaultInfo
  {
  public:
    Addr page_number;
    int pid;

    FaultInfo(): page_number(-1), pid(-1) { }
    FaultInfo(Addr _pn, int _pid): page_number(_pn), pid(_pid) { }

    bool operator==(const MMU::FaultInfo &rhs) const {
      if (page_number == rhs.page_number &&
          pid == rhs.pid)
        return true;
      return false;
    }

    bool operator>(const MMU::FaultInfo &rhs) const {
      if (page_number>rhs.page_number)
        return true;
      return false;
    }

    bool operator<(const MMU::FaultInfo &rhs) const {
      if (page_number<rhs.page_number)
        return true;
      return false;
    }
  };

  class hasher_FaultInfo {
  public:
    size_t operator() (const MMU::FaultInfo &key) const {
      return key.page_number ^ key.pid;
    }
  };
  
  typedef FaultInfo WalkInfo;
  typedef hasher_FaultInfo hasher_WalkInfo;

  class ReplacementUnit
  {
  private:
    struct Entry {
      Addr page_number;
      int pid;
      Entry *prev;
      Entry *next;
    };

    class EntryIndex {
    public:
      Addr page_number;
      int pid;

      EntryIndex(): page_number(-1), pid(-1) { }
      EntryIndex(Addr _pn, int _pid): page_number(_pn), pid(_pid) { }

      bool operator==(const MMU::ReplacementUnit::EntryIndex &rhs) const {
        if (page_number == rhs.page_number &&
            pid == rhs.pid)
          return true;
        return false;
      }
    };

    class hasher_EntryIndex {
    public:
      size_t operator() (const MMU::ReplacementUnit::EntryIndex &key) const {
        return key.page_number ^ key.pid;
      }
    };

    void detach(Entry *node) {
      node->prev->next = node->next;
      node->next->prev = node->prev;
    }

    void attach(Entry *node) {
      node->next = m_head->next;
      node->prev = m_head;
      m_head->next = node;
      node->next->prev = node;
    }

    unordered_map<EntryIndex, Entry *, hasher_EntryIndex> m_table;
    vector<Entry *> m_free_entries;
    Entry *m_head;
    Entry *m_tail;
    Entry *m_entries;

    long m_max_entries;
    macsim_c *m_simBase;

  public:
    ReplacementUnit(macsim_c *simBase, long max_entries)
      : m_simBase(simBase), m_max_entries(max_entries) {
      m_entries = new Entry[m_max_entries];
      for (long i = 0; i < m_max_entries; ++i) {
        m_free_entries.push_back(m_entries + i);
        m_entries[i].page_number = -1;
        m_entries[i].pid = -1;
      }

      m_head = new Entry;
      m_tail = new Entry;

      m_head->prev = NULL;
      m_head->next = m_tail;
      m_tail->next = NULL;
      m_tail->prev = m_head;
    }

    ~ReplacementUnit() {
      delete m_head;
      delete m_tail;
      delete[] m_entries;
    }

    void insert(Addr page_number, int pid=-1);
    void update(Addr page_number, int pid=-1);
    pair<Addr, int> getVictim();
  };

public:
  MMU() {
  }
  ~MMU() {
  }

  void initialize(macsim_c *simBase);
  void finalize();
  void run_a_cycle(bool);

  bool translate(uop_c *cur_uop);
  void handle_page_faults();
  int get_pid(uop_c *cur_uop);

private:
  void do_page_table_walks(uop_c *cur_uop);

  void begin_batch_processing();
  bool do_batch_processing();

  Addr allocate_frame();

  Addr get_page_number(Addr addr) {
    return addr >> m_offset_bits;
  }
  Addr get_page_offset(Addr addr) {
    return addr & (m_page_size - 1);
  }

  macsim_c *m_simBase;
  Counter m_cycle;
  bool m_nonswap;

  Addr m_frame_to_allocate;
  Addr m_frame_dimm;

  unordered_map<int, unordered_map<Addr, PageDescriptor>> m_page_table;
  long m_page_size;
  long m_memory_size;
  long m_offset_bits;
  long m_free_frames_remaining;
  vector<bool> m_free_frames;

  unique_ptr<TLB> m_TLB;
  unique_ptr<ReplacementUnit> m_replacement_unit;

  long m_walk_latency;
  long m_fault_latency;
  long m_eviction_latency;

  // PTW info to be processed
  // queue_cycle: e.g., cycle t - page A, B, C
  // queue_walk: e.g., page A - uop 3, 4, 5, 6
  map<Counter, list<WalkInfo>> m_walk_queue_cycle;
  unordered_map<WalkInfo, list<uop_c *>, hasher_WalkInfo> m_walk_queue_page;

  list<uop_c *> m_retry_queue;
  list<uop_c *> m_fault_retry_queue;

  long m_fault_buffer_size;

  // faults to be processed
  unordered_set<FaultInfo, hasher_FaultInfo> m_fault_buffer;
  unordered_map<FaultInfo, list<uop_c *>, hasher_FaultInfo> m_fault_uops;

  // faults under processing
  list<FaultInfo> m_fault_buffer_processing;
  unordered_map<FaultInfo, list<uop_c *>, hasher_FaultInfo> m_fault_uops_processing;

  bool m_batch_processing;
  bool m_batch_processing_first_transfer_started;
  long m_batch_processing_overhead;
  Counter m_batch_processing_start_cycle;
  Counter m_batch_processing_transfer_start_cycle;
  Counter m_batch_processing_next_event_cycle;

  unordered_set<Addr> m_unique_pages;
};

#endif  // MMU_H_INCLUDED
