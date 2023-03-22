#! /bin/sh
g++ -I.. -o example2.app test.cpp
zip -r example2.zip test.cpp example1.sh example2.sh
../szd -o example2.app.new example2.app example2.zip
#diff example2.app.new example2.mwe
