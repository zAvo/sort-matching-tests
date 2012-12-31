#!/bin/bash

# 13superset0
cd 13superset0/ && make x64_release && cd .. && cp 13superset0/bin/sort_matching_standard.amd64.Release ./superset0 && exit 0

exit 1
