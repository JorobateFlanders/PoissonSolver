paralelo:
	gcc -fopenmp poisson_paralelo.c -o POpar.out
	./POpar.out

secuencial:
	gcc poisson_secuencial.c -o POseq.out
	./POseq.out
