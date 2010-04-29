EXEC = test_top

vpath %.v ../../../asserv/src/hdlcounter

test_top_SOURCES = test_top.v top.v input_latch.v quad_decoder_div4.v

all: $(EXEC:%=%.vcd)

%.vcd: %
	vvp $<

define EXEC_TEMPLATE
$1: $$($1_SOURCES)
	iverilog -Wall -o $$@ $$^
endef
$(foreach exec,$(EXEC),$(eval $(call EXEC_TEMPLATE,$(exec))))

clean:
	rm -f $(EXEC) $(EXEC:%=%.vcd)
