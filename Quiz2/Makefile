
run:
	@for i in 1 3 4; do \
    	gcc -o ee23b035_quiz2_q$$i ee23b035_quiz2_q$$i.c -lm; \
		echo -n $$i: ; \
		eval ./ee23b035_quiz2_q$$i $(file); \
		tail -n 1 ee23b035_quiz2_q$${i}_output.txt; \
	done;
