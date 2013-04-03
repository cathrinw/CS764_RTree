#!/bin/bash
python averageResults.py lin.out.2 6 > l.lin.out ; mv l.lin.out lin.out.2
python averageResults.py lin.out 6 > l.lin.out ; mv l.lin.out lin.out
python averageResults.py quad.out.2 6 > l.lin.out ; mv l.lin.out quad.out.2
python averageResults.py quad.out 6 > l.lin.out ; mv l.lin.out quad.out
python averageResults.py linNum.out 3 > l.lin.out ; mv l.lin.out linNum.out
python averageResults.py quadNum.out 3 > l.lin.out ; mv l.lin.out quadNum.out
python averageResults.py ex.out.2 5 > l.lin.out ; mv l.lin.out ex.out.2
python averageResults.py ex.out 5 > l.lin.out ; mv l.lin.out ex.out
