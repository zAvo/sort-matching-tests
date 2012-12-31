#!/bin/bash

# opencl last because of possible problems with libs
./compile_correctness.sh && ./compile_datatype.sh && ./compile_default.sh && ./compile_lowmem.sh && ./compile_superset.sh && ./compile_thread.sh && ./compile_thdim.sh && ./compile_defdim.sh && ./compile_lowdim.sh && ./compile_default4.sh && ./compile_lowmem4.sh && ./compile_thread4.sh && ./compile_superset0.sh && ./compile_default0.sh && ./compile_lowmem0.sh && ./compile_lowbig.sh && ./compile_opencl.sh && exit 0

exit 1
