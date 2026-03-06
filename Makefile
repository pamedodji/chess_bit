FILES = main.c logics.c utils.c zobrist.c pieces/knight.c pieces/bishop.c pieces/rook.c pieces/pawn.c pieces/king.c pieces/queen.c moves/moves.c kingstate.c
OBJ = $(FILES:.c=.o)
EXEC = prog
CFLAGS = -O3 -march=native -fomit-frame-pointer -g -fopenmp
CFLAGS_PERF = -O3 -march=native -flto -fomit-frame-pointer -funroll-loops -ffast-math -fopenmp


main : $(FILES)
	gcc $(CFLAGS) $(FILES) -o $(EXEC)

obj : $(OBJ)
	gcc $(CFLAGS_PERF) $(OBJ) -o $(EXEC) 

obj_debug : $(OBJ)
	gcc $(CFLAGS) $(OBJ) -o $(EXEC)

perf : $(FILES)
	gcc $(CFLAGS_PERF) $(FILES) -o $(EXEC) 

no_opti : $(FILES)
	gcc -mbmi2 $(FILES) -o $(EXEC)
	
header_only : main_header_only.c impl.c
	gcc $(CFLAGS_PERF)  main_header_only.c impl.c -o ho

debug : $(FILES)
	gcc -g -O0 -Wall -Wextra -$(FILES) -o $(EXEC)

clean:
	rm *.o pieces/*.o moves/*.o
