#!/bin/bash

# 05opencl last because of eventual problems (and slowness)
./correctness.sh && ./datatype.sh && ./default.sh && ./lowmem.sh && ./superset.sh && ./thread.sh && ./thdim.sh && ./thdim200.sh && ./defdim.sh && ./defdim200.sh && ./lowdim.sh &&./lowdim200.sh && ./default4.sh && ./lowmem4.sh && ./thread4.sh && ./superset0.sh && ./default0.sh && ./lowmem0.sh && ./lowbig.sh && ./opencl.sh && exit 0

exit 1
