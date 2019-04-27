.PHONY: clean


T1=main
TARGETS= $(T1)

FLAGS= -std=std99 -Wall -Werror

all: $(TARGETS)


$(T1): $(T1).c
	gcc $< -o $@ $(FLAGS) -std=gnu99


clean:
	rm $(TARGETS)

