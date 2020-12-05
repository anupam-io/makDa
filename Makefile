maxlinks:= 100
pagelimit:= 100
threads:= 50
rankerFlag:= -sp
all_targets:= compile run clean

all: ${all_targets}

compile:
	@echo "Compiling file..."
	g++ -std=c++14 main.cpp -o _crawler -lcurl -lpthread -w

run:
	mkdir temp -p
	@echo "Running..."
	./_crawler $(maxlinks) $(pagelimit) $(threads)

clean:
	rm -rf _crawler
	rm -rf temp
	rm -rf logs.txt
	@echo "All cleaned."