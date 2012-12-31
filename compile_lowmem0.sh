#!/bin/bash

# 15lowmem0
cd 15lowmem0/ && make x64_release && cd .. && cp 15lowmem0/bin/sort_matching_standard.amd64.Release ./lowmem0 && exit 0

exit 1
