#!/bin/bash

# 12thread4
cd 12thread4/ && make x64_release && cd .. && cp 12thread4/bin/sort_matching_threaded.amd64.Release ./thread4 && exit 0

exit 1
