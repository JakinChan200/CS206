# use flag '-g' to only generate tests
# use flag '-r' to only run tests
# use flag '-u' to only run on universe.txt
# use flag '-t' to only run test_suites

# pass prog_name as argument
# https://stackoverflow.com/questions/18568706/check-number-of-arguments-passed-to-a-bash-script
if [ "$#" -lt 1 ]; then
	echo "Usage: ./test_suites.sh <prog_name>"
	exit
fi

# get parameters and save prog_name and flags
flags=''
for i in $@; do
	if [[ $i = '-'* ]]; then
		flags+=$i' '
	else
		prog_name=$i
	fi
done
flags=${flags%' '}
if [[ $flags != *'-g'* && $flags != *'-r'* ]]; then
	flags+=' -g -r'
fi
if [[ $flags = *'-g'* && $flags = *'-r'* ]]; then
	rm -rf test_suites/ results/
fi

# https://ioflood.com/blog/bash-if-string-contains/
if [[ $prog_name != *'/'* ]]; then
    cd benchmarks/$prog_name
else
    cd $prog_name
	prog_name=${prog_name#*/}
fi

# https://ioflood.com/blog/bash-if-string-contains/
# commands are different for each program
if [[ $prog_name = 'totinfo' || $prog_name = 'replace' ]]; then
    cmd='gcc -Wno-return-type -g -o '$prog_name' '$prog_name'.c -lm'
else
    cmd='gcc -Wno-return-type -g -o '$prog_name' '$prog_name'.c'
fi


# copy-paste all needed files to target directory
cp ../../parse.cpp ../../generate_tests.sh ../../data_collect.cpp ../../run_tests.sh ../../clean.sh ./

./clean.sh

# generate_tests
if [[ $flags = *'-g'* && $flags != *'-u'* ]]; then
	./generate_tests.sh $prog_name $cmd
fi

# run_tests
if [[ $flags = *'-r'* ]]; then
	if [[ $flags != *'-t'* ]]; then
	./run_tests.sh $prog_name universe.txt $cmd
	./clean.sh
	fi
	if [[ $flags != *'-u'* ]]; then
	for ts in test_suites/*; do
		./run_tests.sh $prog_name $ts $cmd
		./clean.sh
	done
	fi
fi

# clean up the evidence
rm parse data_collect
rm parse.cpp generate_tests.sh data_collect.cpp run_tests.sh clean.sh
cd ..