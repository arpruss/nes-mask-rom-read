import sys

if sys.argv[1].endswith(".hex"):
    outname = sys.argv[1][:-4]+".bin"
else:
    outname = sys.argv[1]+".bin"
    
with open(sys.argv[1], "r") as inf:
    with open(outname, "wb") as outf:
        for line in inf:
            parts = line.split()
            if len(parts) == 3:
                expected = int(parts[2],16)
                sum = 0
                data = parts[1]
                for i in range(0,32,2):
                    datum = int(data[i:i+2],16)
                    outf.write(bytes((datum,)))
                    sum += datum
                if (sum & 0xFFFF) != expected:
                    print("Error at " + parts[0])