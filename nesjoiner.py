import sys

try:
    prgName = sys.argv[1]
    chrName = sys.argv[2]
    outName = sys.argv[3]
    if len(sys.argv)>=5:
        mapper = int(sys.argv[4])
    else:
        mapper = 0
    if len(sys.argv)>=6:
        flag6low = int(sys.argv[5])
    else:
        flag6low = 0
    if len(sys.argv)>=7:
        flag7low = int(sys.argv[6])
    else:
        flag7low = 0
except:
    print("python3 nesjoiner.py prgfilename.bin chrfilename.bin|- outputfilename.nes [mapper [flag6-low-nybble [flag7-low-nybble]]]")
    print("  sizes should be in kb")
    sys.exit(1)
        
with open(outName,"wb") as out:
        with open(prgName, "rb") as prg:
            prgData = prg.read()
            if len(prgData) % 1024 != 0:
                print("prg should be a multiple of 1024 bytes but is %d bytes" % (len(prgData)))
                sys.exit(2)
                
        if chrName == "-":
            chrData = []
        else:
            with open(chrName, "rb") as chr:
                chrData = chr.read()
                if len(prgData) % 1024 != 0:
                    print("chr should be a multiple of 1024 bytes but is %d bytes" % (len(chrData)))
                    sys.exit(2)
                    
        prgSize = len(prgData) // 1024
        chrSize = len(chrData) // 1024
            
        out.write(b'NES\x1A')
        out.write(bytes([prgSize//16,chrSize//8,flag6low | ((mapper&0xF)<<4),flag7low | ((mapper>>4)<<4)]))
        out.write(bytes(8))
        out.write(prgData)
        out.write(chrData)
        