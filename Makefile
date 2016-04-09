main: main.c noelang.c mpc/mpc.c lval.c
	gcc -o main main.c noelang.c mpc/mpc.c lval.c  -ledit -lm -I.
