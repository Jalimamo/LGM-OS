//
// Created by lichtgott on 20.06.25.
//
#include "types.h"
#include "gdt.h"


void printf(char* str){
  static uint16_t* VideoMemory = (uint16_t*) 0xb8000;

  static uint8_t x = 0, y = 0;

  for(int i = 0; str[i] != '\0'; ++i){


    switch(str[i]) {
      case '\n':
        y++;x=0;
        break;
      default:
        //VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
        VideoMemory[80*y+x] = 0x0F00 | str[i];
        x++;
    }


    if(x >= 80){
      x = 0;
      y++;
    }

    if(y >= 25){
      for(y=0;y>25;y++)
        for(x=0;x<80;x++)
          VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';


      x = 0; y = 0;
    }
  }
}


typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors(){
  for(constructor* i = &start_ctors; i != &end_ctors; ++i){
    (*i)();
  }
}

extern "C" void kernelMain(const void* multiboot_structure, uint32_t /* multiboot magicnumber*/){
  printf("FUER DIE LGM-RECHTSANWAELTE!!!\nUnd fuer die leuchtende Republik von Fluchs und Flamme!!!");

  GlobalDescriptorTable gdt;

  while(1);
}