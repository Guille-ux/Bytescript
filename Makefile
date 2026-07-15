all:
	$(MAKE) -C stdlib
	$(MAKE) -C src

o_o:
	$(MAKE) -C stdlib o_o
	$(MAKE) -C src o_o

o_one:
	$(MAKE) -C stdlib o_one
	$(MAKE) -C src o_one

o_two:
	$(MAKE) -C stdlib o_two
	$(MAKE) -C src o_two

o_three:
	$(MAKE) -C stdlib o_three
	$(MAKE) -C src o_three

byte:
	$(MAKE) -C stdlib byte
	$(MAKE) -C src byte

debug:
	$(MAKE) -C stdlib debug
	$(MAKE) -C src debug

clean:
	rm -rf build
	rm -rf dist