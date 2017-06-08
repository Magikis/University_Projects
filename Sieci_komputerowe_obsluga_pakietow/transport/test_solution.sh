#!/bin/bash
PORT="40001"
echo $PORT
/usr/bin/time -f "\t%E real,\t%U user,\t%S sys,\t%M mem"  ./correct_solution/transport-faster $PORT res $1
# ./correct_solution/transport-faster $PORT res $1
echo "transport-slower finished"
make
/usr/bin/time -f "\t%E real,\t%U user,\t%S sys,\t%M mem" ./main $PORT result $1
# ./main result $1
echo "main finished"
diff --brief result res
# diff <(xxd res) <(xxd result)
#wc -c res
#wc -c result
rm result res
