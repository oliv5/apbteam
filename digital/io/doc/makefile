DOT = mainFsm.dot chargeBall.dot getSamplesFsm.dot chuteFsm.dot

PNG = $(DOT:%.dot=%.png)

all: $(PNG)

%.png: %.dot
	dot -Tpng -o $@ $<

clean:
	rm -f $(PNG)
