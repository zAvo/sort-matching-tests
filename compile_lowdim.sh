#!/bin/bash

# 09lowdim
cd 09lowdim/ && make x64_release && cd .. && cp 09lowdim/bin/sort_matching_standard.amd64.Release ./lowdim && exit 0

exit 1
