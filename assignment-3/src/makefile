.PHONY: clean all
#c-flags
CC = g++
#algos
FIFO=-DFIFO
SJF=-DSJF
SRTF=-DSRTF
RR=-DRR
#size of time slice
SIZE=5
#creating initial folder binary and doc
BIN_LOC = ./bin
DOC_LOC = ./doc
#creating folder if they don't exist
$(BIN_LOC):
	mkdir -p $(BIN_LOC)
$(DOC_LOC):
	mkdir -p $(DOC_LOC)
.$(DOC_LOC):
	mkdir -p .$(DOC_LOC)
#make and run all target
all: $(BIN_LOC) $(DOC_LOC) build_single_cpu build_double_cpu run_single_cpu run_double_cpu
#building binary for single cpu
build_single_cpu: $(BIN_LOC) scheduler.cpp single_fifo single_sjf single_srtf single_rr
single_fifo: scheduler.cpp 
	$(CC) $(FIFO) $< -o $(BIN_LOC)/$@
single_sjf: scheduler.cpp
	$(CC) $(SJF) $< -o $(BIN_LOC)/$@
single_srtf: scheduler.cpp
	$(CC) $(SRTF) $< -o $(BIN_LOC)/$@
single_rr: scheduler.cpp
	$(CC) $(RR) $< -o $(BIN_LOC)/$@
#building binary for double cpu
build_double_cpu: $(BIN_LOC) scheduler_p2.cpp double_fifo double_sjf double_srtf double_rr
double_fifo: scheduler_p2.cpp
	$(CC) $(FIFO) $< -o $(BIN_LOC)/$@
double_sjf: scheduler_p2.cpp
	$(CC) $(SJF) $< -o $(BIN_LOC)/$@
double_srtf: scheduler_p2.cpp
	$(CC) $(SRTF) $< -o $(BIN_LOC)/$@
double_rr: scheduler_p2.cpp
	$(CC) $(RR) $< -o $(BIN_LOC)/$@
#run all binary
run_single_cpu: $(DOC_LOC) run_single_fifo run_single_sjf run_single_srtf run_single_rr
#run binary for single fifo
run_single_fifo: $(BIN_LOC)/single_fifo
	./$< process1.dat >> $(DOC_LOC)/$@1.txt
	./$< process2.dat >> $(DOC_LOC)/$@2.txt
	./$< process3.dat >> $(DOC_LOC)/$@3.txt
run_single_sjf: $(BIN_LOC)/single_sjf
	./$< process1.dat >> $(DOC_LOC)/$@1.txt
	./$< process2.dat >> $(DOC_LOC)/$@2.txt
	./$< process3.dat >> $(DOC_LOC)/$@3.txt
run_single_srtf: $(BIN_LOC)/single_srtf
	./$< process1.dat >> $(DOC_LOC)/$@1.txt
	./$< process2.dat >> $(DOC_LOC)/$@2.txt
	./$< process3.dat >> $(DOC_LOC)/$@3.txt
run_single_rr: $(BIN_LOC)/single_rr
	./$< process1.dat $(SIZE) >> $(DOC_LOC)/$@1.txt
	./$< process2.dat $(SIZE) >> $(DOC_LOC)/$@2.txt
	./$< process3.dat $(SIZE) >> $(DOC_LOC)/$@3.txt
#run all binary
run_double_cpu: $(DOC_LOC) run_double_fifo run_double_sjf run_double_srtf run_double_rr
#run binary for double fifo
run_double_fifo: $(BIN_LOC)/double_fifo
	./$< process1.dat >> $(DOC_LOC)/$@1.txt
	./$< process2.dat >> $(DOC_LOC)/$@2.txt
	./$< process3.dat >> $(DOC_LOC)/$@3.txt
run_double_sjf: $(BIN_LOC)/double_sjf
	./$< process1.dat >> $(DOC_LOC)/$@1.txt
	./$< process2.dat >> $(DOC_LOC)/$@2.txt
	./$< process3.dat >> $(DOC_LOC)/$@3.txt
run_double_srtf: $(BIN_LOC)/double_srtf
	./$< process1.dat >> $(DOC_LOC)/$@1.txt
	./$< process2.dat >> $(DOC_LOC)/$@2.txt
	./$< process3.dat >> $(DOC_LOC)/$@3.txt
run_double_rr: $(BIN_LOC)/double_rr
	./$< process1.dat $(SIZE) >> $(DOC_LOC)/$@1.txt
	./$< process2.dat $(SIZE) >> $(DOC_LOC)/$@2.txt
	./$< process3.dat $(SIZE) >> $(DOC_LOC)/$@3.txt

clean:
	rm -r $(BIN_LOC)
	rm -r $(DOC_LOC)