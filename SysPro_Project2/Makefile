CC = gcc
CFLAGS  = -g


all: jobExecutor workers

jobExecutor:  jobExecutor.o jobExecFunctions.o
	$(CC) $(CFLAGS) -o jobExecutor jobExecutor.o jobExecFunctions.o

workers: workers.o WorkerFunctions.o
	$(CC) $(CFLAGS) -o workers workers.o WorkerFunctions.o

jobExecutor.o:  jobExecutor.c header.h
	$(CC) $(CFLAGS) -c jobExecutor.c

jobExecFunctions.o: jobExecFunctions.c jobheader.h
	$(CC) $(CFLAGS) -c jobExecFunctions.c

workers.o: workers.c WorkerFunctions.c header.h
	$(CC) $(CFLAGS) -c workers.c WorkerFunctions.c

clean: 
	$(RM) all *.o *~
	rm -f jobExecutor workers
