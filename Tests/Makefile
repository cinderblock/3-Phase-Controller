
all: compile run

compile:
	g++ predictor.cpp ../Predictor.cpp -I ../AVR++ -I ./ -std=c++11 -g `csv_parser-config --cxxflags --libs`

run:
	./a.out


