maxlinks:= 1000
pagelimit:= 10
threads:= 4
maxfilesize:= 1024
timeout:= 5
all_targets:= compile run clean

all: ${all_targets}

compile:
	@echo "Compiling file..."
	g++ -std=c++14 main.cpp -o _beqa \
		-w -Wall -Werror -Wextra \
		-lcurl -lpthread -O3
	@echo "Successfully compiled."


run:
	mkdir temp -p
	@echo "Running..."
	./_beqa $(maxlinks) $(pagelimit) $(threads) $(maxfilesize) $(timeout)
	@echo "Successfully runned."


clean:
	@rm -rf _beqa
	@rm -rf temp
	@rm -rf logs.txt
	@echo "All cleaned."