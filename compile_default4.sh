#!/bin/bash

# 10default4
cd 10default4/ && make x64_release && cd .. && cp 10default4/bin/sort_matching_standard.amd64.Release ./default4 && exit 0

exit 1
