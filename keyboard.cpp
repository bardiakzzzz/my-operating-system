
#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(manager, 0x21),        //keyboard interrupt is 0x21
dataport(0x60),
commandport(0x64)
{
    while(commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xae); // activate interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);
    dataport.Write(0xf4);
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(char*);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();
    static bool Shift = false;
    if(key < 0x80)
    {
        switch(key)
        {
            case 0x02: printf("1"); break;
            case 0x03: printf("2"); break;
            case 0x04: printf("3"); break;
            case 0x05: printf("4"); break;
            case 0x06: printf("5"); break;
            case 0x07: printf("6"); break;
            case 0x08: printf("7"); break;
            case 0x09: printf("8"); break;
            case 0x0A: printf("9"); break;
            case 0x0B: printf("0"); break;

            case 0x10: if(Shift) printf("Q");else printf("q"); break;
            case 0x11: if(Shift) printf("W");else printf("w"); break;
            case 0x12: if(Shift) printf("E");else printf("e"); break;
            case 0x13: if(Shift) printf("R");else printf("r"); break;
            case 0x14: if(Shift) printf("T");else printf("t"); break;
            case 0x15: if(Shift) printf("Y");else printf("y"); break;
            case 0x16: if(Shift) printf("U");else printf("u"); break;
            case 0x17: if(Shift) printf("I");else printf("i"); break;
            case 0x18: if(Shift) printf("O");else printf("o"); break;
            case 0x19: if(Shift) printf("P");else printf("p"); break;


            case 0x1E: if(Shift) printf("A");else printf("a"); break;
            case 0x1F: if(Shift) printf("S");else printf("s"); break;
            case 0x20: if(Shift) printf("D");else printf("d"); break;
            case 0x21: if(Shift) printf("F");else printf("f"); break;
            case 0x22: if(Shift) printf("G");else printf("g"); break;
            case 0x23: if(Shift) printf("H");else printf("h"); break;
            case 0x24: if(Shift) printf("J");else printf("j"); break;
            case 0x25: if(Shift) printf("K");else printf("k"); break;
            case 0x26: if(Shift) printf("L");else printf("l"); break;

            case 0x2C: if(Shift) printf("Z");else printf("z"); break;
            case 0x2D: if(Shift) printf("X");else printf("x"); break;
            case 0x2E: if(Shift) printf("C");else printf("c"); break;
            case 0x2F: if(Shift) printf("V");else printf("v"); break;
            case 0x30: if(Shift) printf("B");else printf("b"); break;
            case 0x31: if(Shift) printf("N");else printf("n"); break;
            case 0x32: if(Shift) printf("M");else printf("m"); break;
            case 0x33: printf(","); break;
            case 0x34: printf("."); break;
            case 0x35: printf("-"); break;

            case 0x1C: printf("\n"); break;
            case 0x39: printf(" "); break;
            case 0x0f: printf("    ");break;
            case 0x2A: case 0x36 :if(Shift) Shift =  false; else Shift = true;break;


            default:
            {
                char* foo = "KEYBOARD 0x00 ";
                char* hex = "0123456789ABCDEF";
                foo[11] = hex[(key >> 4) & 0xF];
                foo[12] = hex[key & 0xF];
                printf(foo);
                break;
            }
        }
    }
    return esp;
}
