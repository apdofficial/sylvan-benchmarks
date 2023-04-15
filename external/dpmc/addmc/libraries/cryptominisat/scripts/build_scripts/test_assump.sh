#/usr/bin/bash

if [[ 1 -eq 1 ]]; then
    rm -f out dr x
    rm -f assump.*
    ./tests/cnf-utils/sgen4 -sat -n 80 -s 10  > out
    ./cryptominisat5 out  dr --verb 0
    echo "-1" > assump.ass
    ./cryptominisat5 out  --assump assump.ass  dr --verb 0
    ./tests/drat-trim/drat-trim out dr -A assump.ass -l x -B
    cp out out_more
    grep -v "d" x | grep -v "^0$" | sed "s/ 0 .*/ 0/" >> out_more
    out=$(./cryptominisat5 out_more --verb 0)
    echo $out
fi



if [[ 1 -eq 1 ]]; then
    rm -f out dr x
    rm -f assump.*
    ./tests/cnf-utils/cnf-fuzz-biere 10 > out
    ./cryptominisat5 out  dr --verb 0
    cat >assump.ass <<EOL
1
2
-3
-4
5
6
-7
-8
9
-10
-11
12
13
-14
EOL
    ./cryptominisat5 out  --assump assump.ass  dr --verb 0
    ./tests/drat-trim/drat-trim out dr -A assump.ass -l x -B -O 3
    cp out out_more
    grep -v "d" x | grep -v "^0$" | sed "s/ 0 .*/ 0/" >> out_more
    out=$(./cryptominisat5 out_more --verb 0)
    echo "$out"
fi


# TODO: print decisions for final SAT
# TODO: add them as assumptions (maybe unit clauses?)
# TODO: add extra clause that bans this particular solution -a V -b V -c....
