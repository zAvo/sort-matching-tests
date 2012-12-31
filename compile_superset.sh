#!/bin/bash

# 04superset
cd 04superset/ && make x64_release && cd .. && cp 04superset/bin/sort_matching_standard.amd64.Release ./superset && exit 0

exit 1
