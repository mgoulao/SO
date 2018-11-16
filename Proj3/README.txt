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
|   +-- Makefile
+-- CircuitRouter-SeqSolver
|   +-- CircuitRouter-SeqSolver.c
|   +-- coordinate.c
|   +-- coordinate.h
|   +-- grid.c
|   +-- grid.h
|   +-- maze.c
|   +-- maze.h
|   +-- router.h
|   +-- Makefile
+-- CircuitRouter-SimpleShell
|   +-- CircuitRouter-SimpleShell.c
|   +-- CircuitRouter-SimpleShell.h
|   +-- Makefile
+-- inputs
|   +-- generate.py
|   +-- *.txt (ficheiros inputs)
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
+-- results
|   +-- *.csv (ficheiros com analise do speed-up)
+-- Makefile
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

* Numero de cores: 2 (4 Threads)
* Clock Rate: 2.50GHz
* Model: 78 - Intel(R) Core(TM) i7-6500U CPU

-Sistema Operativo:

* 4.15.0-20-generic #21-Ubuntu SMP Tue Apr 24 06:16:15 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux

