#! /bin/bash

######################################################################################
#
# Project: EE1103Assignments
# License: MIT
# Author: Keshav Saravanan - EE23B035
#
# 1. Use ping to generate a data file with 5000 records.
# 2. Use awk, with output redirection to extract the relevant numeric time value (7th column).
# 3. Use awk to calculate the mean and stdev of the time stamps.
#
######################################################################################

# Generate 5000 lines of data and write it to data.txt
ping -c 5000 -i 0.002 8.8.8.8 > data.txt

# Remove the first and last four lines of the file as they are not meaningful data and overwrite data.txt
tail -n +2 data.txt  | head -n -4 > dataconcise.txt

# Split each record by spaces and '=' and write the 10th field - the time taken - to tmp.txt
awk -F [[:space:]=] '{print $10}' dataconcise.txt > tmp.txt

# Calculate the mean and stddev and print it to stdout.
awk 'BEGIN {sum = 0; sum_x_square = 0} {sum += $1; sum_x_square += $1 * $1} END {print "Mean = " sum / NR "ms"; print "Standard Deviation = " (sum_x_square / NR) - (sum / NR) ^ 2 "ms"}' tmp.txt
