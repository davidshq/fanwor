all:
	$(MAKE) -C src

clean:
	$(RM) out/fanwor
	$(MAKE) -C src clean
