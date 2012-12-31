#!/bin/bash

# 02default
cd 02default/ && make x64_release && cd .. && cp 02default/bin/sort_matching_standard.amd64.Release ./default && exit 0

exit 1
