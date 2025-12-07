""" Get stat info from stats """
import sys
import operator
import optparse
import csv
import shutil
import os.path


# MEM info
CHANNELS=2
RANKS=2
BANKS=8
SUFFIX='_p4'

# Bench & configuration info
#BenchList = [ 'transformer', 't5-base', 'bert-large', 'gpt2', 'mix1', 'mix2', 'mix3', 'mix4' ]
#ConfigList = [ 'hynix_params_dimmonly', 'hynix_params_aiconly' ]

BenchList = [ 'gpt2' ]
ConfigList = [ 'hynix_params_hetero_d1a1', 'hynix_params_hetero_d1a3', 'hynix_params_hetero_d1a7', 'hynix_params_hetero_d1a15', 'hynix_params_hetero_d1a31' ]

def get_dir_list( ):
    dir_list=[]
    for c in ConfigList:
        for b in BenchList:
            dir_name="../"+c+SUFFIX+"."+b+SUFFIX
            dir_list.append(dir_name)

    return dir_list

# Definition of data extraction function
def run( ):
    dir_list = get_dir_list( )
    csv_out = open('statout.csv', 'w')
    csv_writer = csv.writer(csv_out)

    for d in dir_list:
        core_gstats_path = d + '/stats_macsim/general.stat.out'
        core_mstats_path = d + '/stats_macsim/memory.stat.out'
        cxl_stats_path = ''
        mem_stats_path = ''
        cxlmem_stats_path = ''
        if d.find('dimmonly')!=-1:
            mem_stats_path = d + '/stats_macsim/pcmcsim_example.out'
        elif d.find('aiconly')!=-1:
            cxl_stats_path = d + '/stats_cxlmem/io.stat.out'
            cxlmem_stats_path = d + '/stats_cxlmem/pcmcsim_example.out'
        elif d.find('hetero')!=-1:
            cxl_stats_path = d + '/stats_cxlmem/io.stat.out'
            mem_stats_path = d + '/stats_macsim/pcmcsim_example.out'
            cxlmem_stats_path = d + '/stats_cxlmem/pcmcsim_example.out'
        else:
            print("Invalid configuration name may be included")
            sys.exit( )

        if os.path.exists(core_gstats_path) == True:
            core_gstats = open(core_gstats_path, 'r')
        if os.path.exists(core_mstats_path) == True:
            core_mstats = open(core_mstats_path, 'r')
        if os.path.exists(cxl_stats_path) == True:
            cxl_stats = open(cxl_stats_path, 'r')
        if os.path.exists(mem_stats_path) == True:
            mem_stats = open(mem_stats_path, 'r')
        if os.path.exists(cxlmem_stats_path) == True:
            cxlmem_stats = open(cxlmem_stats_path, 'r')

#        print(core_gstats_path)
#        print(core_mstats_path)
#        print(cxl_stats_path)
#        print(mem_stats_path)
#        print(cxlmem_stats_path)
#        print("\n")

        # Blank stats list 
        stats_list = []

        # Get IPC
        ipc = 0.
        insts = 0
        cycle = 0
        while True:
            line = core_gstats.readline( )
            seg_sp = line.split( )
            if len(seg_sp)==0:
                continue

            if seg_sp[0]=='INST_COUNT_TOT':
                insts = float(seg_sp[1])

            if seg_sp[0]=='CYC_COUNT_TOT':
                cycle = float(seg_sp[1])
                break

        ipc = insts/cycle
        core_gstats.close( )

        # Get memory latency
        cxl_ratio = 0.
        mem_rdlat = 0.
        mem_wrlat = 0.
        mem_rdlat_cxl = 0.
        mem_wrlat_cxl = 0.
        llc_hit = 0.
        llc_miss = 0.
        tlb_hit = 0.
        tlb_miss = 0.
        tlb_miss_rate = 0.
        pt_hit = 0.
        pt_miss = 0.
        pt_fault_rate = 0.
        while True:
            line = core_mstats.readline( )
            seg_sp = line.split( )
            if len(seg_sp)==0:
                continue

            if seg_sp[0]=='BME_REQ_RATIO':
                cxl_ratio = float(seg_sp[2])
            if seg_sp[0]=='AVG_BME_LATENCY' and seg_sp[2]!='NaN':
                mem_lat_cxl = float(seg_sp[2])
            if seg_sp[0]=='AVG_BME_RD_LATENCY' and seg_sp[2]!='NaN':
                mem_rdlat_cxl = float(seg_sp[2])
            if seg_sp[0]=='AVG_BME_WR_LATENCY' and seg_sp[2]!='NaN':
                mem_wrlat_cxl = float(seg_sp[2])
            if seg_sp[0]=='AVG_DIMM_LATENCY' and seg_sp[2]!='NaN':
                mem_lat = float(seg_sp[2])
            if seg_sp[0]=='AVG_DIMM_RD_LATENCY' and seg_sp[2]!='NaN':
                mem_rdlat = float(seg_sp[2])
            if seg_sp[0]=='AVG_DIMM_WR_LATENCY' and seg_sp[2]!='NaN':
                mem_wrlat = float(seg_sp[2])
            if seg_sp[0]=='LLC_HIT_CPU':
                llc_hit = float(seg_sp[2])
            if seg_sp[0]=='LLC_MISS_CPU':
                llc_miss = float(seg_sp[2])
            if seg_sp[0]=='TLB_HIT':
                tlb_hit = float(seg_sp[1])
            if seg_sp[0]=='TLB_MISS':
                tlb_miss = float(seg_sp[1])
            if seg_sp[0]=='PAGETABLE_HIT':
                pt_hit = float(seg_sp[1])
            if seg_sp[0]=='PAGETABLE_MISS':
                pt_miss = float(seg_sp[1])
                break

        llc_miss_rate = llc_miss / (llc_hit+llc_miss)
        tlb_miss_rate = tlb_miss / (tlb_hit+tlb_miss)
        pt_fault_rate = pt_miss / (pt_hit+pt_miss)
        mem_avg_lat = (1-cxl_ratio)*mem_lat+cxl_raio*mem_lat_cxl;

        core_mstats.close( )

        # Get CXL latency breakdown 
        cxl_tot_lat = 0.
        rc2tx = 0.
        rx2rc = 0.
        dev2tx = 0.
        rx2dev = 0.
        inmem_lat = 0.
        while True:
            if os.path.exists(cxl_stats_path) == False:
                break
            line = cxl_stats.readline( )
            seg_sp = line.split( )
            if len(seg_sp)==0:
                continue
            
            if seg_sp[0]=='AVG_TOTAL_LATENCY':
                cxl_tot_lat = float(seg_sp[2])
            if seg_sp[0]=='AVG_BME_LATENCY':
                inmem_lat = float(seg_sp[2])
            if seg_sp[0]=='AVG_RCTX_LATENCY':
                rc2tx = float(seg_sp[2])
            if seg_sp[0]=='AVG_RCRX_LATENCY':
                rx2rc = float(seg_sp[2])
            if seg_sp[0]=='AVG_DEVTX_LATENCY':
                dev2tx = float(seg_sp[2])
            if seg_sp[0]=='AVG_DEVRX_LATENCY':
                rx2dev = float(seg_sp[2])
                break

        if os.path.exists(cxl_stats_path) == True and cxl_stats.closed == False:
            cxl_stats.close( )

        # Get power breakdown (DIMM)
        E_tot = 0.
        E_stnby = 0.
        E_ref = 0.
        E_act = 0.
        E_rdwr = 0.
        while True:
            if os.path.exists(mem_stats_path) == False:
                break
            line = mem_stats.readline( ) 
            if not line:
                break
            if len(seg_sp)==0:
                continue

            seg_sp = line.split( )
            if len(seg_sp) <= 1:
                continue
            seg_pt = seg_sp[0].split('.')
            if len(seg_pt) < 2:
                continue

            for cidx in range(CHANNELS):
                if seg_pt[1]!='ucmde['+str(cidx)+']':
                    continue
                for ridx in range(RANKS):
                    if seg_pt[2]!='rank['+str(ridx)+']':
                        continue
                    if seg_pt[3]=='E_total[nJ]':
                        E_tot += float(seg_sp[1])
                    if seg_pt[3]=='E_stnby[nJ]':
                        E_stnby += float(seg_sp[1])
                    if seg_pt[3]=='E_refresh[nJ]':
                        E_ref += float(seg_sp[1])
                    if seg_pt[3]=='E_act[nJ]':
                        E_act += float(seg_sp[1])
                    if seg_pt[3]=='E_rd[nJ]' or seg_pt[3]=='E_wr[nJ]':
                        E_rdwr += float(seg_sp[1])

        mem_stats.close( )

        # Get power breakdown (BME)
        E_tot_cxl = 0.
        E_stnby_cxl = 0.
        E_ref_cxl = 0.
        E_act_cxl = 0.
        E_rdwr_cxl = 0.
        while True:
            if os.path.exists(cxlmem_stats_path) == False:
                break
            line = cxlmem_stats.readline( ) 
            if not line:
                break
            if len(seg_sp)==0:
                continue

            seg_sp = line.split( )
            if len(seg_sp) <= 1:
                continue
            seg_pt = seg_sp[0].split('.')
            if len(seg_pt) < 2:
                continue

            for cidx in range(CHANNELS):
                if seg_pt[1]!='ucmde['+str(cidx)+']':
                    continue
                for ridx in range(RANKS):
                    if seg_pt[2]!='rank['+str(ridx)+']':
                        continue
                    if seg_pt[3]=='E_total[nJ]':
                        E_tot_cxl += float(seg_sp[1])
                    if seg_pt[3]=='E_stnby[nJ]':
                        E_stnby_cxl += float(seg_sp[1])
                    if seg_pt[3]=='E_refresh[nJ]':
                        E_ref += float(seg_sp[1])
                    if seg_pt[3]=='E_act[nJ]':
                        E_act_cxl += float(seg_sp[1])
                    if seg_pt[3]=='E_rd[nJ]' or seg_pt[3]=='E_wr[nJ]':
                        E_rdwr_cxl += float(seg_sp[1])

        cxlmem_stats.close( )

        # Record all stats in desired order
        stats_list.append(insts)
        stats_list.append(cycle)
        stats_list.append(ipc)

        stats_list.append(llc_hit)
        stats_list.append(llc_miss)
        stats_list.append(llc_miss_rate)
        stats_list.append(tlb_hit)
        stats_list.append(tlb_miss)
        stats_list.append(tlb_miss_rate)
        stats_list.append(pt_hit)
        stats_list.append(pt_miss)
        stats_list.append(pt_fault_rate)
        stats_list.append(cxl_ratio)
        stats_list.append(mem_lat)
        stats_list.append(mem_lat_cxl)
        stats_list.append(mem_avg_lat)
        stats_list.append(mem_rdlat)
        stats_list.append(mem_wrlat)
        stats_list.append(mem_rdlat_cxl)
        stats_list.append(mem_wrlat_cxl)

        stats_list.append(rc2tx)
        stats_list.append(rx2dev)
        stats_list.append(inmem_lat)
        stats_list.append(dev2tx)
        stats_list.append(rx2rc)
        stats_list.append(cxl_tot_lat)

        stats_list.append(E_stnby)
        stats_list.append(E_ref)
        stats_list.append(E_act)
        stats_list.append(E_rdwr)
        stats_list.append(E_tot)

        stats_list.append(E_stnby_cxl)
        stats_list.append(E_ref_cxl)
        stats_list.append(E_act_cxl)
        stats_list.append(E_rdwr_cxl)
        stats_list.append(E_tot_cxl)

        print (stats_list)
        csv_writer.writerow(stats_list)

    csv_out.close( )
    print('Processed file #: %d' % len(dir_list))

if __name__ == "__main__":
    run( )

    print('===Data extraction Done===')

