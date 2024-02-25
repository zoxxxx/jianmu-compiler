#!/bin/bash
ulimit -s 1024000000
project_dir=$(realpath ../)
sylib_dir=$(realpath "$project_dir"/src/sylib)
output_dir=output
suffix=sy
timeout=10

LOG=log.txt

usage() {
	cat <<JIANMU
Usage: $0 [path-to-testcases] [type]
path-to-testcases: './testcases' or '../testcases_general' or 'self made cases'
type: 'debug', 'test', or 'll', where 'debug' will output .ll and .s files, 'test' will only test, and 'll' will test .ll files
JIANMU
	exit 0
}

check_return_value() {
	rv=$1
	expected_rv=$2
	fail_msg=$3
	detail=$4
	if [ "$rv" -eq "$expected_rv" ]; then
		return 0
	else
		printf "\033[1;31m%s: \033[0m%s\n" "$fail_msg" "$detail"
		return 1
	fi
}

# check arguments
[ $# -lt 2 ] && usage
if [ "$2" == "debug" ]; then
	debug_mode=true
	ll_mode=false
elif [ "$2" == "test" ]; then
	debug_mode=false
	ll_mode=false
elif [ "$2" == "ll" ]; then
	debug_mode=false
	ll_mode=true
else
	usage
fi

test_dir=$1
testcases=$(ls "$test_dir"/*."$suffix" | sort -V)
check_return_value $? 0 "PATH" "unable to access to '$test_dir'" || exit 1

# hide stderr in the script
# exec 2>/dev/null

mkdir -p $output_dir

truncate -s 0 $LOG

if [ $debug_mode = false ] && [ $ll_mode = false ]; then
	exec 3>/dev/null 4>&1 5>&2 1>&3 2>&3
else
	exec 3>&1
fi

if [ $debug_mode = false ] && [ $ll_mode = false ]; then
	exec 1>&4 2>&5
fi

echo "[info] Start testing, using testcase dir: $test_dir"
# asm or ll
for case in $testcases; do
	echo "==========$case==========" >>$LOG
	case_base_name=$(basename -s .$suffix "$case")
	std_out_file=$test_dir/$case_base_name.out
	in_file=$test_dir/$case_base_name.in
	asm_file=$output_dir/$case_base_name.s
	exe_file=$output_dir/$case_base_name
	out_file=$output_dir/$case_base_name.out
	ll_file=$output_dir/$case_base_name.ll

	echo -n "$case_base_name..."
	# if debug or ll mode on, generate .ll also
	if [ $debug_mode = true ] || [ $ll_mode = true ]; then
		timeout $timeout bash -c "cminusfc -emit-llvm $case -o $ll_file" >>$LOG 2>&1
	fi

	# Skip asm and executable generation if in ll mode
	if [ $ll_mode = false ]; then
		# cminusfc compile to .s
		timeout $timeout bash -c "cminusfc -S $case -o $asm_file" >>$LOG 2>&1
		check_return_value $? 0 "CE" "cminusfc compiler error" || continue

		# gcc compile asm to executable
		loongarch64-unknown-linux-gnu-gcc -static \
			"$asm_file" "$sylib_dir"/sylib.c -o "$exe_file" \
			>>$LOG
		check_return_value $? 0 "CE" "gcc compiler error" || continue

		# qemu run
		if [ -e "$in_file" ]; then
			exec_cmd="qemu-loongarch64 $exe_file >$out_file <$in_file"
		else
			exec_cmd="qemu-loongarch64 $exe_file >$out_file"
		fi
	else
		# For ll mode, use llc and clang to compile and run .ll file
		llc "$ll_file" -filetype=obj -o "$output_dir/$case_base_name.o" >>$LOG 2>&1
		check_return_value $? 0 "CE" "llc compiler error" || continue
		clang "$output_dir/$case_base_name.o" -o "$exe_file" -lsylib>>$LOG 2>&1
		check_return_value $? 0 "CE" "clang linker error" || continue

		# Run the compiled executable
		if [ -e "$in_file" ]; then
			exec_cmd="$exe_file >$out_file <$in_file"
		else
			exec_cmd="$exe_file >$out_file"
		fi
	fi

	if [ $? -eq 124 ]; then
		echo "Compilation Time Limit Exceeded (TLE)" >> $LOG
		printf "\033[1;31mCompilation TLE\033[0m\n"
		continue
	fi

	timeout $timeout bash -c "$exec_cmd"
	ret=$?

	if [ $ret -eq 124 ]; then
		echo "TLE" >> "$out_file"
		printf "\033[1;31mTLE\033[0m\n"
		continue
	fi

	# remove trailing null byte in the end line
	sed -i "\$s/\x00*$//" "$out_file"
	# append return value
	# Check if the last character of the output file is not a newline
	if [ -s "$out_file" ]; then
		# Check if the last character of the output file is not a newline
		if [ "$(tail -c1 "$out_file" | wc -l)" -eq 0 ]; then
			echo "" >> "$out_file"  
		fi
	fi




	echo $ret >>"$out_file"

	# compare output
	diff --strip-trailing-cr "$std_out_file" "$out_file" -y >>$LOG
	check_return_value $? 0 "WA" "output differ, check $std_out_file and $out_file" || continue

	# ok
	printf "\033[1;32mOK\033[0m\n"
done
