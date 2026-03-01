FILES = main.c logics.c utils.c zobrist.c
EXEC = prog
CFLAGS = -O3 -march=native -fomit-frame-pointer -g
CFLAGS_PERF = -O3 -march=native -flto -fomit-frame-pointer -funroll-loops -ffast-math 

main : $(FILES)
	gcc $(CFLAGS) $(FILES) -o $(EXEC)

perf : $(FILES)
	gcc $(CFLAGS_PERF) $(FILES) -o $(EXEC)

no_opti : $(FILES)
	gcc $(FILES) -o $(EXEC)
	
header_only : main_header_only.c impl.c
	gcc $(CFLAGS_PERF)  main_header_only.c impl.c -o ho

debug : $(FILES)
	gcc -g -O0 -Wall -Wextra $(FILES) -o $(EXEC)

