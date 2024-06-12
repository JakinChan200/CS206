# gcc --coverage tcas.c -c
# gcc --coverage tcas.o
# ./a.out
# gcov tcas -m --json-format

# function to get gcov info on a file
# https://linuxize.com/post/bash-functions/
test_prog () {
    local p=$1
    local file=".c"
    local cmd=$2
    # gcc --coverage -Wno-return-type -g -o tcas tcas.c
    $cmd --coverage -w      # compile command   # -w suppresses warnings
    input="universe.txt"
    mkdir json/
    
    # https://stackoverflow.com/questions/2578116/how-can-i-test-if-line-is-empty-in-shell-script
    n=1
    while IFS=$'\n' read -r line
    do
        args=""
        file=""

        # echo $line
        for i in $line; do                      # script can't handle < file.txt, so parse it beforehand
            if [[ $i = *'/'* ]]; then
                file="$i"
            elif [[ $i != '<'* ]]; then
                args+=$i' '
            fi
        done
        file=${file#'<'*}

        if [ "$line" != "" ]; then
            echo $n $line
            if [[ $file = "" ]]; then
                ./$p $args
            else
                ./$p $args < $file
            fi
            gcov $p.c -m -b --json-format
            # gzip -dk $1.gcov.json.gz 
            # rm $1.gcov.json.gz
            # mv $1.gcov.json json/$1$n.json
            gzip -dk *.gz 
            rm *.gz
            mv *.json json/$1$n.json
            rm $1.gcda
            let "n++"
        fi
    done < "$input"

    rm $1.gcno
    # rm $1
}

# pass prog_name as argument
# https://stackoverflow.com/questions/18568706/check-number-of-arguments-passed-to-a-bash-script
if [ "$#" -lt 2 ]; then
    echo "Usage: ./generate_tests.sh prog_name cmd..."
    exit
fi
# $ ./generate_tests.sh pzn pzn_test.txt
prog_name=$1
test_name="universe.txt"
cmd=$@
cmd=${cmd#*' '}
# echo "$cmd"

# get gcov info on correct program, put in json folder and number
test_prog $prog_name "$cmd"

#parse resulting json
g++ parse.cpp -o parse

if [ ! -d test_suites/ ]; then
    mkdir test_suites/      # put all test suites in here
fi
./parse $prog_name $test_name           # generate the 6 different kinds of test suites

rm -rf json/
echo "Finished generate_tests"
echo