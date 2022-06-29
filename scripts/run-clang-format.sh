#!/bin/sh

clang-format -i `find . -name "*.cpp"`
clang-format -i `find . -name "*.h"`
