all: tree

tree:
	g++ -std=c++11 ./tree_v_submission.cpp -o ./tree

clean:
	rm -f ./tree
