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


#which="ast"
cfgFiles=[]
for f in correct_files:
    command = "make -f Makefile32.cfg FILE=" + f+ " >/dev/null";
    os.system(command);
    cfgFiles.append(path+"/"+f+"s306.cfg"); 

print("Done generating cfg files\n");

for f in cfgFiles:
    print("Testing file  " + f ); 
    command = "./cfg-test32 -eval -d " + f + " > expected "
    os.system(command) 
    command = "./cfglp -eval -d "  + f + " > generated  "
    os.system(command)
    os.system("diff -bB expected generated");

error_files=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.ecfg'):
        error_files.append(path + "/" +f)

for f in error_files:
    print("Testing file  " + f ); 
    command = "./cfg-test32 -eval -d  " + f + " 2> expected " 
    os.system(command) 
    command = "./cfglp -eval -d  " + f + " 2> generated " 
    os.system(command)
    os.system("diff -b expected generated");
