default:
	compile_cpp

compile_cpp:
	rm exe/*
	g++ -std=c++17 src/label_prop.cpp -o exe/label_prop
	g++ -std=c++17 src/main_on_cluster.cpp -o exe/gen_dg
	g++ -std=c++17 src/main_notebooks.cpp -o exe/gen_dg_nb

run_6dfb:
	exe/gen_dg 15801 12 1 7 <datasets/6dfb_mat >datasets/fb_dg_eps
	exe/gen_dg 15801 12 2 7 <datasets/6dfb_mat >datasets/fb_dg_knn
	exe/gen_dg 15801 12 3 <datasets/6dfb_mat >datasets/fb_dg_rng
	exe/gen_dg 15801 12 4 <datasets/6dfb_mat >datasets/fb_dg_gabr
	exe/gen_dg 15801 12 5 <datasets/6dfb_mat >datasets/fb_dg_inf
