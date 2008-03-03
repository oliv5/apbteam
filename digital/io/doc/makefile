DOT = mainFsm.dot loadBall.dot getSamplesFsm.dot stdContenerFsm.dot

PNG = $(DOT:%.dot=%.png)

all: $(PNG)

%.png: %.dot
	dot -Tpng -o $@ $<

clean:
	rm -f $(PNG)
