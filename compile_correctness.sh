#!/bin/bash

# 00correctness
cd 00correctness/default && make x64_release && cd ../lowmem && make x64_release && cd ../.. && cp 00correctness/default/bin/sort_matching_standard.amd64.Release ./correctness_default && cp 00correctness/lowmem/bin/sort_matching_standard.amd64.Release ./correctness_lowmem && exit 0

exit 1
