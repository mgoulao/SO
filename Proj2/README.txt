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
|   +-- grid.c
|   +-- maze.c
|   +-- router.c
+-- CircuitRouter-SimpleShell
|   +-- CircuitRouter-SimpleShell.c
|   +-- CircuitRouter-SimpleShell.h
+-- inputs
|   +-- *.txt
+-- lib
|   +-- *.c
+-- ./doTest.sh
+-- README.txt

#Compilar

make

#Executar

-Pelo doTest.sh

./doTest.sh

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

