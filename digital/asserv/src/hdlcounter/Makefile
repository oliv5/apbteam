EXEC = test_input_latch test_noise_filter test_updown_counter \
       test_quad_decoder test_counter_top

test_input_latch_SOURCES = test_input_latch.v input_latch.v
test_noise_filter_SOURCES = test_noise_filter.v noise_filter.v
test_updown_counter_SOURCES = test_updown_counter.v updown_counter.v
test_quad_decoder_SOURCES = test_quad_decoder.v quad_decoder_div4.v \
			    quad_decoder_full.v
test_counter_top_SOURCES = test_counter_top.v counter_top.v input_latch.v \
			   noise_filter.v quad_decoder_div4.v \
			   quad_decoder_full.v

all: $(EXEC:%=%.vcd)

%.vcd: %
	vvp $<

define EXEC_TEMPLATE
$1: $$($1_SOURCES) common.v
	iverilog -Wall -o $$@ $$($1_SOURCES)
endef
$(foreach exec,$(EXEC),$(eval $(call EXEC_TEMPLATE,$(exec))))

clean:
	rm -f $(EXEC) $(EXEC:%=%.vcd)
