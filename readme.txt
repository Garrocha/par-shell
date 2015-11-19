========================================
par-shell
Sistemas Operativos 2015/16

Tiago Gonçalves
Mariana Vicente
Patricia Santos
========================================

v1
-Ficheiros base do projecto
-Inicio da função main (já lê input)
====================
v2
-alguns problemas no list.h (as vezes nao reconhece as funções)
====================
v2.1
-list.h e Makefile corrigidos
-On par-shell.c -> execv corrigido.
-Macros WIFEXITED e WEXITSTATUS implementadas
-Correcao de alguns prints.
====================
v2.2
- no cmd exit -> verifica valor de retorno do wait
                 verifica se um processo foi terminado por 1 sinal
- no lst_print-> so imprime processos que terminaram correctamente
---- versao final p/ entrega 09.10.15 ----
====================
v2.3
-comentarios alterados
-funcao rm_process
====================
v 3.0
- add mutexes
- add semaphores
- enhanced comments
---- final version for submision 30.10.15 ----
====================
v 4.0
- add pthread_cond functions
- lst_print - now prins execution time
- minor changes on update_terminated_process
- added conditional values (falta tirar semaforos e cenas do ficheiro)
v 4.1
- removed semaphores (.c and .h still present)
- added File functions
- reads and writes to a log file.
- Todo: wrap all exit functions into 1.
---- final version for submission 13.11.15 ----
v 4.2 alinea extra
- prints (and reads) a 4th line in an iteration with the end date of the process.
====================
v 5.0
- added mutexLock and mutexUnlock
- added cleanUp
- added valMAXPAR (fixed "more" cmd)
