#!/bin/bash

# 07thdim
cd 07thdim/ && make x64_release && cd .. && cp 07thdim/bin/sort_matching_threaded.amd64.Release ./thdim && exit 0

exit 1
