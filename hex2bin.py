import sys
import binascii
if sys.argv[1].endswith(".hex"):
    outname = sys.argv[1][:-4]+".bin"
else:
    outname = sys.argv[1]+".bin"
    
crc32 = 0
    
with open(sys.argv[1], "r") as inf:
    with open(outname, "wb") as outf:
        for line in inf:
            if line.startswith('>'):
                parts = line[1:].split()
                expected = int(parts[2],16)
                sum = 0
                data = parts[1]
                for i in range(0,32,2):
                    datum = int(data[i:i+2],16)
                    binary = bytes((datum,))
                    crc32 = binascii.crc32(binary, crc32)
                    outf.write(binary)
                    sum += datum
                if (sum & 0xFFFF) != expected:
                    print("Error at " + parts[0])
                    
print("CRC-32: %08x\n" % crc32)                    