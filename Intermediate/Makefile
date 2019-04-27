.PHONY: clean


T1= hexthrow
T2= syscall_read
T3= large_file_size
T4= file_properties
T5= directory_properties
TARGETS= $(T1) $(T2) $(T3) $(T4) $(T5)

FLAGS= -std=c99 -Wall -Werror
all: $(TARGETS)


$(T1): $(T1).c
	gcc $< -o $@ $(FLAGS)

$(T2): $(T2).c
	gcc $< -o $@ $(FLAGS)

$(T3): $(T3).c
	gcc $< -o $@ $(FLAGS)

$(T4): $(T4).c
	gcc $< -o $@ $(FLAGS)

$(T5): $(T5).c
	gcc $< -o $@ $(FLAGS) -std=gnu99

clean:
	rm $(T1) $(T2) $(T3) $(T4) $(T5)
