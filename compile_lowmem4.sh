#!/bin/bash

# 11lowmem4
cd 11lowmem4/ && make x64_release && cd .. && cp 11lowmem4/bin/sort_matching_standard.amd64.Release ./lowmem4 && exit 0

exit 1
