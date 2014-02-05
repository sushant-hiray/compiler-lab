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



cfgFiles=[]
for f in correct_files:
    command = "make -f Makefile.cfg FILE=" + f+ " >/dev/null";
    os.system(command);
    cfgFiles.append(path+"/"+f+"s306.cfg"); 

print("Done generating cfg files\n");

for f in cfgFiles:
    print("Testing file  " + f ); 
    command = "./cfg-test -ast -d " + f + " > expected " 
    os.system(command) 
    command = "./cfglp -ast -d " + f + " > generated " 
    os.system(command)
    os.system("diff -bB expected generated");


error_files=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.ecfg'):
        error_files.append(path + "/" +f)

for f in error_files:
    print("Testing file  " + f ); 
    command = "./cfg-test -ast  " + f + " > expected " 
    os.system(command) 
    command = "./cfglp -ast " + f + " > generated " 
    os.system(command)
    os.system("diff -bB expected generated");






