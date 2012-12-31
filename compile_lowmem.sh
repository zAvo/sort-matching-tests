#!/bin/bash

# 03lowmem
cd 03lowmem/ && make x64_release && cd .. && cp 03lowmem/bin/sort_matching_standard.amd64.Release ./lowmem && exit 0

exit 1
