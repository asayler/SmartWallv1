# Andy Sayler
# SmartWall Project
# Report
# makefile
# Created 4/30/11
#
# Change Log:
# 04/30/11 - Created
# ---

.PHONY : all
all : SDReportv1.pdf

SDReportv1.pdf : SDReportv1.tex
	pdflatex SDReportv1.tex
	pdflatex SDReportv1.tex
	pdflatex SDReportv1.tex

.PHONY : clean
clean :
	rm -f SDReportv1.pdf
	rm -f *.log
	rm -f *.toc
	rm -f *.aux
	rm -f *~