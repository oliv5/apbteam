PAGES=trace.txt

HTML=$(PAGES:%.txt=%.html)

%.html: %.txt
	rst2html $< > $@

all : $(HTML)

clean:
	rm $(HTML)
