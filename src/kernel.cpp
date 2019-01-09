#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

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
void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}




class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(char c)
    {
        if(c == '\t'){
          printf("    ");
          return;
        }
        char* foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
public:

    MouseToConsole()

    {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);
    }

    virtual void OnMouseMove(int xoffset, int yoffset)
    {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);

        x += xoffset;
        if(x >= 80) x = 79;
        if(x < 0) x = 0;
        y += yoffset;
        if(y >= 25) y = 24;
        if(y < 0) y = 0;

        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);
    }

};





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
    printf("Welcome to my OS\n");
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(0x20, &gdt);   //IRQ_BASE = 0x20
    // KeyboardDriver keyboard(&interrupts);
    // MouseDriver mouse(&interrupts);
    printf("Initializing Hardware, Stage 1 : Initializing GDT\n");

   DriverManager drvManager;

       PrintfKeyboardEventHandler kbhandler;
       KeyboardDriver keyboard(&interrupts, &kbhandler);
       drvManager.AddDriver(&keyboard);

       MouseToConsole mousehandler;
       MouseDriver mouse(&interrupts, &mousehandler);
       drvManager.AddDriver(&mouse);

       PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager);



   printf("Initializing Hardware, Stage 2 : Activate drivers\n");
       drvManager.ActivateAll();

   printf("Initializing Hardware, Stage 3 : Activate interrupts\n");
    interrupts.Activate();
    while(1);
}
