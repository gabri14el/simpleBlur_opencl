### Makefile ###
#g++ blur.cpp -o blur -lOpenCL `pkg-config --cflags --libs opencv` -std=c++11 

CFLAGS=-o -lOpenCL $(shell pkg-config --cflags opencv) 
LDFLAGS=$(shell pkg-config --libs opencv)
CC=gcc

SOURCES=$(wildcard *.cpp)
EXECUTABLES=$(patsubst %.cpp,%,$(SOURCES))

all: $(EXECUTABLES)