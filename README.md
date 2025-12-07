# Macsim 
## Quick Start (For class project)
1. Download code and traces
```
git clone [url-to-code] macsim
gdown --id 1c_O7L3191u7ddNTrqv-pNidWxbGnEZV1
tar -zxvf mascim_traces.tar.gz
cp macsim_traces/trace_file_list macsim/bin/
```

2. Setup Macsim
```
cd macsim/scripts
./knobgen.pl
./statgen.pl
cd ..
```

3. Build & Run
```
./build.py -j{# of cores} // For more details, please refer to macsim/INSTALL
cd bin/
./macsim params.in trace_file_list .
```


## Introduction (From original repo)

* MacSim is a heterogeneous architecture timing model simulator that is
  developed from Georgia Institute of Technology.
* It simulates x86, ARM64, NVIDIA PTX and Intel GEN GPU instructions and 
  can be configured as either a trace driven or execution-drive cycle 
  level simulator. It models detailed mico-architectural behaviors, 
  including pipeline stages,  multi-threading, and memory systems.
* MacSim is capable of simulating a variety of architectures, such as Intel's
  Sandy Bridge, Skylake (both CPUs and GPUs) and NVIDIA's Fermi. 
  It can simulate homogeneous ISA multicore simulations as well as 
  heterogeneous ISA multicore simulations. It also supports asymmetric 
  multicore configurations (small cores + medium cores + big cores) and 
  SMT or MT architectures as well.
* Currently interconnection network model (based on IRIS) and power model (based
  on McPAT) are connected.
* MacSim is also one of the components of SST, so muiltiple MacSim simulatore
  can run concurrently.
* The project has been supported by Intel, NSF, Sandia National Lab.

## Note

* Stats of macsim will be stored in $(macsim)/stats-macsim; 

* If you're interested in the Intel's integrated GPU model in MacSim, please 
  refer to [intel_gpu](https://github.com/gthparch/macsim/tree/intel_gpu) branch.

* We've developed a power model for GPU architecture using McPAT. Please refer
  to the following paper for more detailed
  information. [Power Modeling for GPU Architecture using McPAT](http://www.cercs.gatech.edu/tech-reports/tr2013/git-cercs-13-10.pdf)
  Modeling for GPU Architecture using McPAT.pdf) by Jieun Lim, Nagesh
  B. Lakshminarayana, Hyesoon Kim, William Song, Sudhakar Yalamanchili, Wonyong
  Sung, from Transactions on Design Automation of Electronic Systems (TODAES)
  Vol. 19, No. 3.
* We've characterised the performance of Intel's integrated GPUs using MacSim. 
  Please refer to the following paper for more detailed information. [Performance Characterisation and Simulation of Intel's Integrated GPU Architecture (ISPASS'18)](http://comparch.gatech.edu/hparch/papers/gera_ispass18.pdf)

## People

* Prof. Hyesoon Kim (Project Leader) at Georgia Tech 
Hparch research group 
(http://hparch.gatech.edu/people.hparch) 

## Tutorial

* We had a tutorial in HPCA-2012. Please visit [here](http://comparch.gatech.edu/hparch/OcelotMacsim_tutorial.html) for the slides.
* We had a tutorial in ISCA-2012, Please visit [here](http://comparch.gatech.edu/hparch/isca12_gt.html) for the slides.

