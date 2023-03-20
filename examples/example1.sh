#! /bin/sh
# g++ -o test.app test.cpp
../szd -o test.app.new test.app test1.zip
diff test.app.new test.app.new.mwe