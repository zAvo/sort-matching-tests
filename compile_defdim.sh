#!/bin/bash

# 08defdim
cd 08defdim/ && make x64_release && cd .. && cp 08defdim/bin/sort_matching_standard.amd64.Release ./defdim && exit 0

exit 1
