import os

FILECOUNT = 10
data_size = [1]
curr = 1024*64

for i in range(1,FILECOUNT+1):
    curr = i*curr
    data_size.append(curr)

str1 = ""
for i in range(1024):
    str1 += 'a';

for i in range(1,FILECOUNT+1):
    with open(os.path.join("site_root/data",str(i)+".txt"), "w") as f:
        for j in range(i*FILECOUNT):
            f.write(str1)


