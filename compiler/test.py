import os  
path = "test_files"
files=[]
for f in os.listdir(path):
    files.append(f) 

correct_files=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.c'):
        correct_files.append(f)


which="ast"
cfgFiles=[]
for f in correct_files:
    command = "make -f Makefile.cfg FILE=" + f+ " >/dev/null";
    os.system(command);

for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.cfg'):
        cfgFiles.append(path + "/" +f)
print("Done generating cfg files\n");

for f in cfgFiles:
    print("Testing file  " + f ); 
    command = "./cfg-test -icode -d " + f + " > expected "
    os.system(command) 
    command = "./cfglp64 -icode -d "  + f + " > generated  "
    os.system(command)
    os.system("diff expected generated");

error_files=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.ecfg'):
        error_files.append(path + "/" +f)

for f in error_files:
    print("Testing file  " + f ); 
    command = "./cfg-test -icode -d  " + f + " > expected " 
    os.system(command) 
    command = "./cfglp64 -icode -d  " + f + " > generated " 
    os.system(command)
    #os.system("diff -b expected generated");
