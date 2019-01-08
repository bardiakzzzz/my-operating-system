
void printf(char* str)
{
    static unsigned short* VideoMemory = (unsigned short*)0xb8000; //a place to show string on screen by graphic card

    for(int i = 0; str[i] != '\0'; ++i)
        VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i]; // 2 bytes az once to avoid overwriting to high byte
}



typedef void (*constructor)();       //for work with global and static parameters
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}



extern "C" void kernelMain(const void* multiboot_structure, unsigned int /*multiboot_magic*/) //extern for not change name of function
{
    printf("Hello World!!!");

    while(1);
}
