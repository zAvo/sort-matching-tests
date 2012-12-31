#!/bin/bash

# 16lowbig
cd 16lowbig/ && make x64_release && cd .. && cp 16lowbig/bin/sort_matching_standard.amd64.Release ./lowbig && exit 0

exit 1
