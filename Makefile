CC 				:= g++
CC_FLAGS 	:= -w -Wall -Werror -Wextra -lcurl -lpthread -O3
CC_V 			:= -std=c++14

BUILD := makda_build

maxlinks		:= 100
pagelimit		:= 5
threads			:= 10
maxfilesize	:= 100
timeout			:= 5
restore_data:= 0
save_data		:= 0

all_targets:= compile run clean

all: ${all_targets}

compile:
	@echo "Compiling file..."
	$(CC) main.cpp -o $(BUILD) $(CC_V) $(CC_FLAGS)
	@echo "Successfully compiled."


run:
	mkdir -p temp
	@echo "Running..."
	./$(BUILD) \
		$(maxlinks) $(pagelimit) $(threads) \
		$(maxfilesize) $(timeout) \
		$(save_data) $(restore_data)
	@echo "Successfully runned."


clean:
	@rm -rf $(BUILD)
	@rm -rf temp
	@rm -rf logs.txt
	@echo "All cleaned."