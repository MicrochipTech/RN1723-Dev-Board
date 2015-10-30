#!/usr/bin/env python

from argparse import ArgumentParser

__version__ = "1.0"

def main():

    parser = ArgumentParser(description="CA Cert to C Header file converter",
                            epilog="Copyright (C) 2015 Microchip Technology Inc.",
                            )

    parser.add_argument("--version", action="version", version="%(prog)s "  + __version__)
    parser.add_argument("file", help="CA Cert file to convert")
    parser.add_argument("outfile", help="Converted CA Cert output file")

    args = parser.parse_args()

    with open(args.file, "rb") as fd:
        data = bytearray(fd.read())

    out_file = open(args.outfile, 'w')
    
    data_out = ""
    for part in range(0, len(data), 8):
        if data_out:
            data_out += ",\n"
        data_out += ', '.join(["0x%02x" % b for b in data[part: part + 8]])

    out_file.write("/* file auto-generated from %s by bin2h.py */\n" % args.file)
    out_file.write("#include <GenericTypeDefs.h>\n\n")
    out_file.write("size_t caCert_len = %d;\n\n" % (len(data)))
    out_file.write("unsigned char caCert[%d] = {\n%s\n};\n\n" % (len(data), data_out))
 
    print("/* file auto-generated from %s by bin2h.py  */\n" % args.file)   
    print("#include <GenericTypeDefs.h>\n\n")
    print("size_t caCert_len = %d;\n\n" % (len(data)))
    print("unsigned char caCert[%d] = {\n%s\n};\n\n" % (len(data), data_out))

if __name__ == "__main__":
    main()