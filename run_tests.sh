# https://sentry.io/answers/determine-whether-a-directory-exists-in-bash/
if [ ! -d results/ ]; then
	mkdir results/
	mkdir results/by_case/
fi
# for input in *_test.txt test_suites/*_test.txt; do

# pass prog_name as argument
# https://stackoverflow.com/questions/18568706/check-number-of-arguments-passed-to-a-bash-script
if [ "$#" -lt 3 ]; then
    echo "Usage: ./run_tests.sh prog_name prog_test.txt cmd..."
    exit
fi
input=$2
echo "Running tests using $input"
prog_name=$1
cmd=$@
cmd=${cmd#*.txt' '}			# uses the fact that test_cases use .txt

# get the correct outputs first
n=1
$cmd -w			# compile command
echo "Getting correct outputs..."
while IFS= read -r line;
do
	# echo $n
	
	args=''
	file=''
	for i in $line; do                      # script can't handle < file.txt, so parse it beforehand
		if [[ $i = *'/'* ]]; then
			file=$i
			file=${file#'<'*}
		elif [[ $i != '<'* ]]; then
			args+=$i' '
		fi
	done
	file=${file#'<'*}

	if [ "$line" != "" ]; then
		# echo "./$prog_name $line"
		if [[ $file = "" ]]; then
			./$prog_name $args > "$n.txt"
		else
			# ./$prog_name $args < $file
			./$prog_name $args < $file > "$n.txt" || true
		fi
	fi
	let "n++"
	# if [ $n -ge 10 ]; then
	# 	break
	# fi
done < "$input"

# for all faulty cases, generate outputs
for d in v*/;
do
	cd $d
	echo "Running tests on $d"
	n=1
	$cmd -w		# compile command
	while IFS= read -r line
	do
		args=""
        file=""
        for i in $line; do                      # script can't handle < file.txt, so parse it beforehand
            if [[ $i = *'/'* ]]; then
                file=$i
            elif [[ $i != '<'* ]]; then
                args+=$i' '
            fi
        done
		file=${file#'<'*}
		if [[ $file != "" ]]; then
			file='../'$file
		fi

		if [ "$line" != "" ]; then
			# echo "./$prog_name $line"
			if [[ $file = "" ]]; then
				./$prog_name $args &> "$n.txt"
			else
				./$prog_name $args < $file &> "$n.txt" || true
			fi
		fi
		let "n++"
	done < "../$input"
	rm $prog_name
	cd ..
done

# aggregate .txt files
g++ data_collect.cpp -o data_collect	# scrape all v#/ directories for comparisons between output files
./data_collect $input

echo "Finished run_tests"
echo