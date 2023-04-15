#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (C) 2018 Mate Soos
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 2
# of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.


import random
import sys
import optparse

parser = optparse.OptionParser()
parser.add_option("--seed", "-s", metavar="SEED", dest="seed", type=int,
                  help="random seed value")
(options, args) = parser.parse_args()
if options.seed is not None:
    random.seed(options.seed)

num_matrixes = random.randint(3, 5)
numvars = random.randint(500, 1000)
numunits = random.randint(0, 25)
numlongs = random.randint(200, 300 )
numcls = numunits + numlongs

xorclsizes = []
for i in range(random.randint(50, max(50, int(numvars/2) ))):
    thissize = random.randint(4, 7)
    xorclsizes.append(thissize)
    numcls += (1 << (thissize - 1))*num_matrixes

print("p cnf %d %d" % (numvars, numcls))

# longcls
for i in range(numlongs):
    for i2 in range(random.randint(2, 5)):
        lit = random.randint(1, numvars)
        if random.randint(0, 1) == 1:
            lit = -1 * lit
        sys.stdout.write("%d " % lit)

    print("0")

# units
for i in range(numunits):
    lit = random.randint(1, numvars)
    if random.randint(0, 1) == 1:
        lit = -1 * lit
    print("%d 0" % lit)

# xors
per_matrix_vars = int(numvars/num_matrixes)
def add_xors(matrix_num) :
    vars_from = 1+per_matrix_vars*matrix_num
    vars_to = per_matrix_vars*matrix_num+per_matrix_vars
    # print(matrix_num, " ", vars_from, " ", vars_to, " ", per_matrix_vars, " numvars: " , numvars)
    assert vars_from < vars_to
    assert vars_to <= numvars

    for thisxorsize in xorclsizes:
        varlist = []

        # create varlist
        for a in range(thisxorsize):
            var = random.randint(vars_from, vars_to)
            while var in varlist:
                var = random.randint(vars_from, vars_to)

            # flip randomly
            if random.randint(0, 1) == 1:
                var = -1 * var

            varlist.append(var)

        # polarity of the XOR
        polarity = random.randint(0, 1)

        for i2 in range(1 << len(varlist)):
            # number of inversions is right, use it
            if bin(i2).count("1") % 2 == polarity:
                at = 0
                for var in varlist:
                    lit = var

                    # calculate inversion
                    invert = (i2 >> at) & 1 == 1
                    # if polarity :
                    #    invert = not invert

                    # create lit
                    if invert:
                        lit = -1 * var

                    # print lit
                    sys.stdout.write("%d " % lit)
                    at += 1

                # end of clause
                print("0")

for m in range(num_matrixes):
    add_xors(m)


