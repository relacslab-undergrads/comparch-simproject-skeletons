# Notes

## CXL stats
- ```TURN_LATENCY```
  - DIMM : Request inserted ~ callback
  - CXL : Request inserted into PCIe -> memory access (writes) -> request returned from PCIe (reads)
  - Currently, using ```m_core_cycle[req->m_core_id]```

- ```AVG_LATENCY```
  - Time from ```init_new_req``` to ```free_req```
  - Currently using ```m_cycle``` from ```memory.cc```
  - CME request should show greater values due to neglecting cache hits for their requests

## TODO
- Code cleanup : remove useless datastructures in dram.cc (like m_cme_free_list)
