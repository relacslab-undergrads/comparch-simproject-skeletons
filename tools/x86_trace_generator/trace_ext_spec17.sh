#!/bin/sh

# Copyright (c) 2020 The Regents of the University of California.
# SPDX-License-Identifier: BSD 3-Clause

# This file is the script that runs on the gem5 guest. This reads a file from the host via m5 readfile
# which contains the workload and the size to run. Then it resets the stats before running the workload.
# Finally, it exits the simulation after running the workload, then it copies out the result file to be checked.

PIN_HOME="/hdd1/hynix-cxl/pin-3.13-98189-g60a6ef199-gcc-linux"
SPEC_HOME="/hdd1/workload_srcs/SPECCPU/cpu2017"
TGEN_HOME="/hdd1/hynix-cxl/macsim-cxl/tools/x86_trace_generator"
cd $SPEC_HOME
source ./shrc

size="ref"
NUM_INST=5000000000

for i in {1..23..1}
do
    RUN_PATH=
    if [ $size = "ref" ]; then
        RUN_PATH="run_base_refrate_mytest-m64.0000"
    elif [ $size = "test" ]; then
        RUN_PATH="run_base_test_mytest-m64.0000"
    elif [ $size = "train" ]; then
        RUN_PATH="run_base_train_mytest-m64.0000"
        echo "Currently sole-run mode does not support train arguments"
    fi

    ARGS=
    IFILE=
    OFILE_PREFIX=
    if [ $i -eq 1 ]; then
        workload="500.perlbench_r"
        if [ $size == "ref" ]; then
            ARGS="-I./lib diffmail.pl 4 800 10 17 19 300"
            OFILE_PREFIX="diffmail.4.800.10.17.19.300"
        elif [ $size == "test" ]; then
            ARGS="-I. -I./lib test.pl"
            OFILE_PREFIX="perl_test"
        fi
    elif [ $i -eq 2 ]; then
        continue
        workload="502.gcc_r"
        if [ $size == "ref" ]; then
            ARGS="ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s"
            OFILE_PREFIX="ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2"
        elif [ $size == "test" ]; then
            ARGS="t1.c -O3 -finline-limit=50000 -o t1.opts-O3_-finline-limit_50000.s"
            OFILE_PREFIX="t1.opts-O3_-finline-limit_50000"
        fi
    elif [ $i -eq 3 ]; then
        continue
        workload="503.bwaves_r"
        ARGS="bwaves_1"
        IFILE="bwaves_1.in"
        OFILE_PREFIX="bwaves_1"
    elif [ $i -eq 4 ]; then
        workload="505.mcf_r"
        ARGS="inp.in"
        OFILE_PREFIX="inp"
    elif [ $i -eq 5 ]; then
        workload="507.cactuBSSN_r"
        if [ $size == "ref" ]; then
            ARGS="spec_ref.par"
            OFILE_PREFIX="spec_ref"
        elif [ $size == "test" ]; then
            ARGS="spec_test.par"
           OFILE_PREFIX="spec_test" 
        fi
    elif [ $i -eq 6 ]; then
        workload="508.namd_r"
        if [ $size == "ref" ]; then
            ARGS="--input apoa1.input --output apoa1.ref.output --iterations 65"
            OFILE_PREFIX="namd_apoa1_iter65"
        elif [ $size == "test" ]; then
            ARGS="--input apoa1.input --output apoa1.test.output --iterations 1" 
            OFILE_PREFIX="namd_apoa1_iter1"
        fi
    elif [ $i -eq 7 ]; then
        workload="510.parest_r"
        if [ $size == "ref" ]; then
            ARGS="ref.prm"
            OFILE_PREFIX="parest_ref"
        elif [ $size == "test" ]; then
            ARGS="test.prm"
            OFILE_PREFIX="parest_test"
        fi
    elif [ $i -eq 8 ]; then
        workload="511.povray_r"
        if [ $size == "ref" ]; then
            ARGS="SPEC-benchmark-ref.ini"
            OFILE_PREFIX="SPEC-benchmark-ref"
        elif [ $size == "test" ]; then
            ARGS="SPEC-benchmark-test.ini"
            OFILE_PREFIX="SPEC-benchmark-test"
        fi
    elif [ $i -eq 9 ]; then
        workload="519.lbm_r"
        if [ $size == "ref" ]; then
            ARGS="3000 reference.dat 0 0 100_100_130_ldc.of"
            OFILE_PREFIX="lbm_ldc"
        elif [ $size == "test" ]; then
            ARGS="20 reference.dat 0 0 100_100_130_cf_a.of"
            OFILE_PREFIX="lbm_cf_a"
        fi
    elif [ $i -eq 10 ]; then
        workload="520.omnetpp_r"
        ARGS="-c General -r 0"
        OFILE_PREFIX="omentpp.General-0"
    elif [ $i -eq 11 ]; then
        continue
        workload="521.wrf_r"
        ARGS="" 
        OFILE_PREFIX="wrf"
    elif [ $i -eq 12 ]; then
        workload="523.xalancbmk_r"
        if [ $size == "ref" ]; then
            ARGS="-v t5.xml xalanc.xsl"
            OFILE_PREFIX="ref-t5"
        elif [ $size == "test" ]; then
            ARGS="-v test.xml xalanc.xsl"
            OFILE_PREFIX="test-test"
        fi
    elif [ $i -eq 13 ]; then
        workload="525.x264_r"
        if [ $size == "ref" ]; then
            ARGS="--pass 1 --stats x264_stats.log --bitrate 1000 --frames 1000 -o BuckBunny_New.264 BuckBunny.yuv 1280x720"
            OFILE_PREFIX="run_000-1000_x264_pass1"
        elif [ $size == "test" ]; then
            ARGS="--dumpyuv 50 --frames 156 -o BuckBunny_New.264 BuckBunny.yuv 1280x720" 
            OFILE_PREFIX="run_000-156_x264_pass1"
        fi
    elif [ $i -eq 14 ]; then
        workload="526.blender_r"
        if [ $size == "ref" ]; then
            ARGS="sh3_no_char.blend --render-output sh3_no_char_ --threads 1 -b -F RAWTGA -s 849 -e 849 -a"
            OFILE_PREFIX="sh3_no_char.849.spec"
        elif [ $size == "test" ]; then
            ARGS="cube.blend --render-output cube_ --threads 1 -b -F RAWTGA -s 1 -e 1 -a"
           OFILE_PREFIX="cube.1.spec" 
        fi
    elif [ $i -eq 15 ]; then
        continue
        workload="527.cam4_r"
        ARGS=""
        OFILE_PREFIX="cam4"
    elif [ $i -eq 16 ]; then
        workload="531.deepsjeng_r"
        if [ $size == "ref" ]; then
            ARGS="ref.txt"
            OFILE_PREFIX="deepsjeng_ref"
        elif [ $size == "test" ]; then
            ARGS="test.txt"
            OFILE_PREFIX="deepsjeng_test"
        fi
    elif [ $i -eq 17 ]; then
        workload="538.imagick_r"
        if [ $size == "ref" ]; then
            ARGS="-limit disk 0 refrate_input.tga -edge 41 -resample 181% -emboss 31 -colorspace YUV -mean-shift 19x19+15% -resize 30% refrate_output.tga"
            OFILE_PREFIX="refrate_convert"
        elif [ $size == "test" ]; then
            ARGS="-limit disk 0 test_input.tga -shear 25 -resize 640x480 -negate -alpha Off test_output.tga" 
            OFILE_PREFIX="test_convert"
        fi
    elif [ $i -eq 18 ]; then
        workload="541.leela_r"
        if [ $size == "ref" ]; then
            ARGS="ref.sgf"
            OFILE_PREFIX="leela_ref"
        elif [ $size == "test" ]; then
            ARGS="test.sgf"
            OFILE_PREFIX="leela_test"
        fi
    elif [ $i -eq 19 ]; then
        workload="544.nab_r"
        if [ $size == "ref" ]; then
            ARGS="1am0 1122214447 122"
            OFILE_PREFIX="1am0"
        elif [ $size == "test" ]; then
            ARGS="hkrdenq 1930344093 1000" 
            OFILE_PREFIX="hkrdenq"
        fi
    elif [ $i -eq 20 ]; then
        continue
        workload="548.exchange2_r"
        if [ $size == "ref" ]; then
            ARGS="6"
            OFILE_PREFIX="exchange2_6"
        elif [ $size == "test" ]; then
            ARGS="0"
            OFILE_PREFIX="exchange2_0"
        fi
    elif [ $i -eq 21 ]; then
        continue
        workload="549.fotonik3d_r"
        ARGS=""
        OFILE_PREFIX="fotonik3d"
    elif [ $i -eq 22 ]; then
        continue
        workload="554.roms_r"
        ARGS=""
        if [ $size == "ref" ]; then
            IFILE="ocean_benchmark2.in.x"
            OFILE_PREFIX="ocean_benchmark2"
        elif [ $size == "test" ]; then
            IFILE="ocean_benchmark0.in.x"
            OFILE_PREFIX="ocean_benchmark0"
        fi
    else
        workload="557.xz_r"
        if [ $size == "ref" ]; then
            ARGS="cpu2006docs.tar.xz 250 055ce243071129412e9dd0b3b69a21654033a9b723d874b2015c774fac1553d9713be561ca86f74e4f16f22e664fc17a79f30caa5ad2c04fbc447549c2810fae 23047774 23513385 6e"
            OFILE_PREFIX="cpu2006docs.tar-250-6e"
    #            ARGS="input.combined.xz 250 a841f68f38572a49d86226b7ff5baeb31bd19dc637a922a972b2e6d1257a890f6a544ecab967c313e370478c74f760eb229d4eef8a8d2836d233d3e9dd1430bf 40401484 41217675 7"
    #            OFILE_PREFIX="input.combined.tar-250-7"
        elif [ $size == "test" ]; then
            ARGS="cpu2006docs.tar.xz 4 055ce243071129412e9dd0b3b69a21654033a9b723d874b2015c774fac1553d9713be561ca86f74e4f16f22e664fc17a79f30caa5ad2c04fbc447549c2810fae 1548636 1555348 0" 
            OFILE_PREFIX="cpu2006docs.tar-4-0"
        fi
    fi

    RUN_PATH=$SPEC_HOME"/benchspec/CPU/"$workload"/run/"$RUN_PATH
    cd $RUN_PATH
    pwd
    workload=$(printf $workload | sed 's/^[0-9.]*//')

    if [ -z $IFILE ]; then
        echo "$PIN_HOME/pin -t $TGEN_HOME/obj-intel64/trace_generator.so -tracename $TGEN_HOME/trace_spec17_$workload -max $NUM_INST -manual 1 -- ./$workload $ARGS > $OFILE_PREFIX.out 2> $OFILE_PREFIX.err"
#        $PIN_HOME"/pin" -t $TGEN_HOME"/obj-intel64/trace_generator.so" -tracename $TGEN_HOME"/trace_spec17_"$workload -max $NUM_INST -manual 1 -- ./$workload $ARGS > $OFILE_PREFIX".out" 2> $OFILE_PREFIX".err"
        $PIN_HOME"/pin" -t $TGEN_HOME"/obj-intel64/trace_generator.so" -tracename $TGEN_HOME"/trace_spec17_"$workload -max $NUM_INST -manual 1 -- ./$workload $ARGS
    else
        echo "$PIN_HOME/pin -t $TGEN_HOME/obj-intel64/trace_generator.so -tracename $TGEN_HOME/trace_spec17_$workload -max $NUM_INST -manual 1 -- ./$workload $ARGS < $IFILE > $OFILE_PREFIX.out 2> $OFILE_PREFIX.err"
        $PIN_HOME"/pin" -t $TGEN_HOME"/obj-intel64/trace_generator.so" -tracename $TGEN_HOME"/trace_spec17_"$workload -max $NUM_INST -manual 1 -- ./$workload $ARGS < $IFILE > $OFILE_PREFIX".out" 2> $OFILE_PREFIX".err"
    fi
done
