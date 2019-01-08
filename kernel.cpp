#include "types.h"
#include "gdt.h"
#include "interrupts.h"
void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000; //a place to show string on screen by graphic card
    static uint8_t x=0,y=0;

    for(int i = 0; str[i] != '\0'; ++i){
      ///  VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i]; // 2 bytes az once to avoid overwriting to high byte
      switch(str[i])  /// use it because in perivous way we could not use printf more than one time
        {
            case '\n':   //goes to the next line
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25) //it is not a good idea
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }

}



typedef void (*constructor)();       //for work with global and static parameters
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}



extern "C" void kernelMain(const void* multiboot_structure, uint32_t/*multiboot_magic*/) //extern for not change name of function
{
    printf("Hello World\n");
    printf("hi\n");
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(0x20, &gdt);   //IRQ_BASE = 0x20
    interrupts.Activate();
    while(1);
}
