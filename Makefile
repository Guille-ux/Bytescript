all:
	$(MAKE) -C stdlib_src
	$(MAKE) -C src

o_o:
	$(MAKE) -C stdlib_src o_o
	$(MAKE) -C src o_o

o_one:
	$(MAKE) -C stdlib_src o_one
	$(MAKE) -C src o_one

o_two:
	$(MAKE) -C stdlib_src o_two
	$(MAKE) -C src o_two

o_three:
	$(MAKE) -C stdlib_src o_three
	$(MAKE) -C src o_three

byte:
	$(MAKE) -C stdlib_src byte
	$(MAKE) -C src byte

debug:
	$(MAKE) -C stdlib_src debug
	$(MAKE) -C src debug

clean:
	rm -rf build
	rm -rf dist