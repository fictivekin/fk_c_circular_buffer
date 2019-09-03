#! /usr/bin/env bash

afl-fuzz -i fuzz/input -o fuzz/findings -x fuzz/keywords.dict ./fuzz/fuzz_driver -m 900