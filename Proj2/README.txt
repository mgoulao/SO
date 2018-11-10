#Estrutura das diretorias

.
+-- CircuitRouter-ParSolver
|   +-- CircuitRouter-ParSolver.c
|   +-- coordinate.c
|   +-- coordinate.h
|   +-- grid.c
|   +-- grid.h
|   +-- maze.c
|   +-- maze.h
|   +-- router.h
+-- CircuitRouter-SeqSolver
|   +-- CircuitRouter-SeqSolver.c
|   +-- coordinate.c
|   +-- coordinate.h
|   +-- grid.c
|   +-- grid.h
|   +-- maze.c
|   +-- maze.h
|   +-- router.h
+-- CircuitRouter-SimpleShell
|   +-- CircuitRouter-SimpleShell.c
|   +-- CircuitRouter-SimpleShell.h
+-- inputs
|   +-- generate.py
|   +-- random-x128-y128-z5-n128.txt
|   +-- random-x32-y32-z3-n64.txt
|   +-- random-x48-y48-z3-n64.txt
|   +-- random-x64-y64-z3-n64.txt
|   +-- random-x128-y128-z3-n128.txt
|   +-- random-x256-y256-z3-n256.txt
|   +-- random-x32-y32-z3-n96.txt
|   +-- random-x512-y512-z7-n512.txt
|   +-- random-x128-y128-z3-n64.txt
|   +-- random-x256-y256-z5-n256.txt
|   +-- random-x48-y48-z3-n48.txt
|   +-- random-x64-y64-z3-n48.txt
+-- lib
|   +-- commandlinereader.c
|   +-- commandlinereader.h
|   +-- list.c
|   +-- list.h
|   +-- list.o
|   +-- pair.c
|   +-- pair.h
|   +-- pair.o
|   +-- queue.c
|   +-- queue.h
|   +-- queue.o
|   +-- timer.h
|   +-- types.h
|   +-- utility.h
|   +-- vector.c
|   +-- vector.h
|   +-- vector.o
+-- ./doTest.sh
+-- README.txt

#Compilar

make

#Executar

-Pelo doTest.sh

./doTest.sh numberThreads inputFile

-Linha de comandos

cd CircuitRouter-ParSolver
./CircuitRouter-ParSolver -t numberThreads inputFile


#Carateristicas

-CPU:

* Numero de cores: 4
* Clock Rate: 500.024MHz
* Model: 78 - Intel(R) Core(TM) i7-6500U CPU @ 2.50GHz

-Sistema Operativo:

* 4.15.0-20-generic #21-Ubuntu SMP Tue Apr 24 06:16:15 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux

