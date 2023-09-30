#! /bin/bash

######################################################################################
#
# Project: EE1103Assignments
# License: MIT
# Author: Keshav Saravanan - EE23B035
#
# 1. Use ping to generate a data file with 5000 records.
#
######################################################################################

# Generate 5000 lines of data and write it to data.txt
ping -c 5000 -i 0.002 8.8.8.8 > data.txt
