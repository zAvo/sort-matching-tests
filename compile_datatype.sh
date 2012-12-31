#!/bin/bash

# 01datatype
cd 01datatype/int32/ && make x64_release && cd ../int64 && make x64_release && cd ../float && make x64_release && cd ../double && make x64_release && cd ../.. && cp 01datatype/int32/bin/sort_matching_standard.amd64.Release ./datatype_int32 && cp 01datatype/int64/bin/sort_matching_standard.amd64.Release ./datatype_int64 && cp 01datatype/float/bin/sort_matching_standard.amd64.Release ./datatype_float && cp 01datatype/double/bin/sort_matching_standard.amd64.Release ./datatype_double && exit 0

exit 1
