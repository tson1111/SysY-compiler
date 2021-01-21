all:
	flex -o lex.yy.cpp eeyore.l
	bison -d -o eeyore.tab.cpp eeyore.y
	g++ *.cpp -Wno-register -O2 -lm -std=c++17 -o compiler
test: test/test.sy
	./compiler -S -e test/test.sy
clean:
	rm -rf lex.yy.cpp eeyore.tab.* compiler
submit: clean
	git add .
	git commit -m "blabla"
	sudo git push
	
