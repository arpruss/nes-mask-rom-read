import sys

try:
    prgName = sys.argv[1]
    prgSize = int(sys.argv[2])
    chrName = sys.argv[3]
    chrSize = int(sys.argv[4])
    outName = sys.argv[5]
    if len(sys.argv)>=7:
        mapper = int(sys.argv[6])
    else:
        mapper = 0
    if len(sys.argv)>=8:
        flag6low = int(sys.argv[7])
    else:
        flag6low = 0
    if len(sys.argv)>=9:
        flag7low = int(sys.argv[8])
    else:
        flag7low = 0
except:
    print("python3 nesjoiner.py prgfilename.bin prgsize chrfilename.bin chrsize outputfilename.nes [mapper [flag6 low nybble [flag7 low nybble]]]")
    print("  sizes should be in kb")
    sys.exit(1)
    
    
with open(outName,"wb") as out:
        out.write(b'NES\x1A')
        out.write(bytes([prgSize//16,chrSize//8,flag6low | ((mapper&0xF)<<4),flag7low | ((mapper>>4)<<4)]))
        out.write(bytes(8))
        
        with open(prgName, "rb") as prg:
            data = prg.read()
            if prgSize*1024 != len(data):
                print("prg should be %d bytes but is %d bytes" % (prgSize*1024,len(data)))
                sys.exit(2)
            out.write(data)
            
        with open(chrName, "rb") as chr:
            data = chr.read()
            if chrSize*1024 != len(data):
                print("chr should be %d bytes but is %d bytes" % (chrSize*1024,len(data)))
                sys.exit(2)
            out.write(data)
