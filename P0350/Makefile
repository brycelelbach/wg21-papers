NAME := $(shell grep 'newcommand.wgDocumentNumber' main.tex|cut -f2 -d'{'|cut -f1 -d'}')
DRAFT := $(NAME)_draft
TEXINPUTS := ..:.:
PDFLATEX := TEXINPUTS=$(TEXINPUTS) flock . lualatex --shell-escape --halt-on-error --file-line-error --interaction nonstopmode
BIBER := biber --input-directory .. --output-directory .
MAKEINDEX := makeindex
SHELL := /bin/bash
EXTRA_DEPS := $(wildcard *.sty) .dummy.tex
CREATEMD5 := md5sum `find . -regex '.*\.\(aux\|bbl\|toc\|tex\|ind\)'` > aux-md5sums
CHECKMD5  := md5sum --quiet -c aux-md5sums

all: final mobile

.dummy.tex:
	touch $@

help:
	@echo "all"
	@echo "devel"
	@echo "draft-loop"
	@echo "final-loop"
	@echo "final-figure<number>-loop"
	@echo "draft"
	@echo "final"
	@echo "clean"
	@echo "autocommit"

TEX_INPUTS   := $(shell egrep '^[^%]*\\in(put|clude){.*?}' main.tex|sed 's@^.*{\(.*\)}.*$$@\1.tex@')
TEX_INPUTS2  := $(shell test -n "$(TEX_INPUTS)" && egrep '^[^%]*\\in(put|clude){.*?}' $(TEX_INPUTS) .dummy.tex|sed -e 's@^.*{\([^.]*\)}.*$$@\1.tex@' -e 's@^.*{\(.*\)}.*$$@\1@') $(TEX_INPUTS)
TEX_INPUTS   := $(shell test -n "$(TEX_INPUTS2)" && egrep '^[^%]*\\in(put|clude){.*?}' $(TEX_INPUTS2) .dummy.tex|sed -e 's@^.*{\([^.]*\)}.*$$@\1.tex@' -e 's@^.*{\(.*\)}.*$$@\1@') $(TEX_INPUTS2) $(TEX_INPUTS) main.tex
TEX_LISTINGS := $(shell test -n "$(TEX_INPUTS)" && awk -F '{|}' -- '/lstinputlisting\[[^\]]*$$/ { getline; while($$0 !~ /\] *{.*\..*}$$/) getline; sub(/^.*\]/, ""); print } /lstinputlisting\[.*\] *{.*}/ { print }' $(TEX_INPUTS) |sed 's=.*\(\[[^]]*\]\)\?{\([^}]*\)}.*=\2='| sort -u)
TEX_PLOTDATA := $(shell grep '\\addplot.* file' $(TEX_INPUTS)|sed 's=^.*\\addplot.* file *{\(.*\)}.*$$=\1=')
GRAPHICS := $(shell grep 'includegraphics' main.tex $(TEX_INPUTS)|grep -v newcommand|sed -e 's=.*\\includegraphics[^{]*{\([^}]*\)}.*=\1=' |sort -u|while read f; do for d in $(subst :, ,$(TEXINPUTS)); do test -f $$d/$$f && echo $$d/$$f && break; done; done)
ALL_DEPS := $(TEX_INPUTS) $(TEX_LISTINGS) $(TEX_PLOTDATA) $(GRAPHICS) $(EXTRA_DEPS)

devel: final-loop

draft: $(DRAFT).pdf
final: $(NAME).pdf
mobile: $(NAME)_mobile.pdf

upload: $(NAME).pdf
	@echo -n "Uploading $< ."
	@while ! timeout 4s scp -B $< lxpool.gsi.de:web-docs/datapar.pdf; do echo -n .; done
	@echo .

loop-internal:
	@while true; do \
	  while true; do \
	    printf '\ekLaTeX waiting: $(loopinternaltarget)\e\\'; \
	    if ! $(MAKE) -q $(loopinternaltarget).pdf>/dev/null; then \
	      printf '\ekLaTeX build: $(loopinternaltarget)\e\\'; \
	      ls -lt $(ALL_DEPS) | head -n1 > .stamp; \
	      nice ionice -c idle $(MAKE) $(loopinternaltarget).pdf || break; \
	    fi; \
	    sleep 1s; \
	  done; \
	  printf '\ekfailed\e\\'; \
	  kdialog --passivepopup "LaTeX ($(NAME)) failed" 1; \
	  echo -e '\a'; \
	  ls -lt $(ALL_DEPS) $(MORE_DEPS) | head -n1 > .stamp.new; \
	  while diff -q .stamp .stamp.new; do \
	    sleep 1s; \
	    ls -lt $(ALL_DEPS) $(MORE_DEPS) | head -n1 > .stamp.new; \
	  done; \
	done

draft-loop:
	$(MAKE) loopinternaltarget=$(DRAFT) MORE_DEPS=../draft.tex loop-internal

final-loop:
	$(MAKE) loopinternaltarget=$(NAME) MORE_DEPS=../final.tex loop-internal

%-loop:
	$(MAKE) loopinternaltarget=$(NAME)_$* MORE_DEPS=../$*.tex loop-internal

final-figure%-loop:
	@while true; do $(MAKE) -q final-figure$*.pdf || $(MAKE) final-figure$*.pdf && sleep 1s; done

clean:
	rm -f $(NAME).pdf $(DRAFT).pdf *.aux *.bbl *.blg *.brf *.lof *.log *.lol *.lot *.out *.toc */*.aux *.auxlock aux-md5sums *.bcf *.run.xml .stamp .stamp.new

ALL_DEPS=$(TEX_INPUTS) $(TEX_LISTINGS) $(EXTRA_DEPS) $(TEX_PLOTDATA)

$(DRAFT).pdf: ../draft.tex $(ALL_DEPS)
	$(PDFLATEX) ../draft
	@test -s draft.pdf && mv draft.pdf $@
	$(BIBER) draft || true

$(NAME).pdf: ../final.tex $(ALL_DEPS)
	@$(CREATEMD5)
	$(PDFLATEX) ../final
	@test -s final.pdf && mv final.pdf $@
	$(BIBER) final || true
	@while ! $(CHECKMD5); do \
		$(CREATEMD5); \
		$(PDFLATEX) ../final; \
		test -s final.pdf && mv final.pdf $@; \
	done

$(NAME)_%.pdf: ../%.tex $(ALL_DEPS)
	@$(CREATEMD5)
	$(PDFLATEX) ../$*
	@test -s $*.pdf && mv $*.pdf $@
	test -f $*.bcf && $(BIBER) $* || true
	test -f $*.ind && $(MAKEINDEX) $* || true
	while ! $(CHECKMD5); do \
		$(CREATEMD5); \
		$(PDFLATEX) ../$*; \
		test -s $*.pdf && mv $*.pdf $@; \
		test -f $*.ind && $(MAKEINDEX) $*; \
	done

final-figure%.pdf: final.tex $(ALL_DEPS)
	$(PDFLATEX) --halt-on-error --interaction=nonstopmode --jobname "final-figure$*" "\def\tikzexternalrealjob{final}\input{final}"

autocommit:
	@printf '\ekautocommit\e\\'
	@while true; do \
	  git commit -am "auto: `git status --porcelain|grep -v '^??'|cut -c4-|paste -s -d' '`" && \
	  printf '\ekcommitted\e\\' && \
	  git push && \
	  printf '\ekcommitted & pushed\e\\'; \
	  sleep 5s; \
	  printf '\ekautocommit\e\\'; \
	  sleep 55s; \
	done

.PHONY: clean
