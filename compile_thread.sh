#!/bin/bash

# 06thread
cd 06thread/ && make x64_release && cd .. && cp 06thread/bin/sort_matching_threaded.amd64.Release ./thread && exit 0

exit 1
