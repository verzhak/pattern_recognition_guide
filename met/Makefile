
OUTPUT_DIR = build
SRC = tmet.tex
SRC_AUX = tmet.aux

debug: clean

	mkdir -p $(OUTPUT_DIR)
	pdflatex -output-directory $(OUTPUT_DIR) $(SRC)

	make prepare

release: clean

	mkdir -p $(OUTPUT_DIR)
	pdflatex -output-directory $(OUTPUT_DIR) $(SRC) 
	bibtex $(OUTPUT_DIR)/$(SRC_AUX) 
	pdflatex -output-directory $(OUTPUT_DIR) $(SRC) 
	pdflatex -output-directory $(OUTPUT_DIR) $(SRC) 

	make prepare

prepare:

	echo -e "\
from pyPdf import PdfFileWriter, PdfFileReader \n\
\n\
o = PdfFileWriter() \n\
i = PdfFileReader(file(\"build/tmet.pdf\", \"rb\")) \n\
\n\
i.getNumPages() \n\
\n\
for x in range(0, i.getNumPages() - 1): o.addPage(i.getPage(x)) \n\
\n\
s = file(\"build/met.pdf\", \"wb\") \n\
o.write(s) \n\
s.close()" | python2 

clean:

	rm -f $(OUTPUT_DIR)/*

distclean:

	rm -Rf $(OUTPUT_DIR)

run: clean debug

	evince $(OUTPUT_DIR)/met.pdf

release_run: clean release

	evince $(OUTPUT_DIR)/met.pdf

