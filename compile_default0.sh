#!/bin/bash

# 14default0
cd 14default0/ && make x64_release && cd .. && cp 14default0/bin/sort_matching_standard.amd64.Release ./default0 && exit 0

exit 1
