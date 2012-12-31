#!/bin/bash

# 05opencl
cd 05opencl/ && make x64_release && cd .. && cp 05opencl/bin/* ./ && mv ./sort_matching_opencl.amd64.Release ./opencl && exit 0

exit 1
