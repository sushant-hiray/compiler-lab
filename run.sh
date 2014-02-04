
./cfglp test_files/test.cs306.cfg -tokens -d > token.dump
./cfglp test_files/test.cs306.cfg -ast -d > ast.dump
./cfglp test_files/test.cs306.cfg -eval -d > eval.dump

./cfg-test test_files/test.cs306.cfg -tokens -d > token.ref
./cfg-test test_files/test.cs306.cfg -ast -d > ast.ref
./cfg-test test_files/test.cs306.cfg -eval -d > eval.ref

