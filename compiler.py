# Example of compiler for custom virtual core with custom instructions 
# Authors: Bilal Taalbi & Maxime Rouhier
# Last update : 08/12/2021
import binascii
import sys
import struct
dict_opcode =    {"AND":0,
                 "ORR":1,
                 "EOR":2,
                 "ADD":3,
                 "ADC":4,
                 "CMP":5,
                 "SUB":6,
                 "SBC":7,
                 "MOV":8,
                 "LSH":9,
                 "RSH":10}


dict_register =  {"r0":0,
                  "r1":1,
                  "r2":2,
                  "r3":3,
                  "r4":4,
                  "r5":5,
                  "r6":6,
                  "r7":7,
                  "r8":8,
                  "r9":9,
                  "r10":10,
                  "r11":11,
                  "r12":12,
                  "r13":13,
                  "r14":14,
                  "r15":15}

dict_branch = {"B":8,
               "BEQ":9,
               "BNE":10,
               "BLE":11,
               "BGE":12,
               "BL":13,
               "BG":14,}

def instr_handler_op(iv,dest_r,s_op,f_op,op,ivf):
    binary = 0
    binary = iv << 0 | dest_r << 8 | s_op << 12 | f_op << 16 | op << 20 | ivf << 24
    print(bin(binary))
    print(binary)
    return struct.pack('<I',binary)

def instr_handler_branch(offset,bcc):
    binary = offset << 0 | bcc << 28
    print(bin(binary))
    print(binary)
    return struct.pack('<I',binary)

def main(filename):
    file = open(filename,"r")
    tab = file.readlines()
    file.close()
    for i in range(len(tab)):
        tab[i] = tab[i].rstrip("\n")
    binary_file = open("binary","wb")
    for i in range(len(tab)):
        s = tab[i].replace(",","").split()
        if s[0].upper() in dict_opcode:
            print(s)
            op = dict_opcode[s[0].upper()]
            if op == 0 or op == 1 or op == 2 or op ==3 or op ==4 or op ==6 or op ==7 or op ==9 or op ==10:
                if not(s[3].isnumeric()):
                    instr = instr_handler_op(0,dict_register[s[1]],dict_register[s[3]],dict_register[s[2]],op,0)
                else:
                    instr = instr_handler_op(int(s[3]),dict_register[s[1]],0,dict_register[s[2]],op,1)
            elif op == 5:
                if not(s[2].isnumeric()):
                    instr = instr_handler_op(0,0,dict_register[s[2]],dict_register[s[1]],op,0)
                else:
                    instr = instr_handler_op(int(s[2]),0,0,dict_register[s[1]],op,1)
            elif op == 8:
                if not(s[2].isnumeric()):
                    instr = instr_handler_op(0,dict_register[s[1]],dict_register[s[2]],0,op,0)
                else:
                    instr = instr_handler_op(int(s[2]),dict_register[s[1]],0,0,op,1)
        elif s[0].upper() in dict_branch:
            instr = instr_handler_branch(int(s[1]), dict_branch[s[0]])
        binary_file.write(instr)
    binary_file.close()

main(sys.argv[1])
