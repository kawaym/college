cFiles=./*.c

pyFile=./*.py

mkdir results

echo "running matrices with $1 elements"

python3 $pyFile $1

for file in $cFiles;
do
	echo "compiling archive $file"
	for i in 0 1 2 3;
	do
		gcc -O$i -march='native' -msse4.2 $file -o $file.O$i
	done
done

runFiles=./dgemm*.c.*

for file in $runFiles;
do
	$file $1
	echo ""
done
