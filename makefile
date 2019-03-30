default:
	rm exe/*
	g++ -std=c++17 src/label_prop.cpp -o exe/label_prop
	g++ -std=c++17 src/gen_data_graphs.cpp -o exe/gen_data_graphs
