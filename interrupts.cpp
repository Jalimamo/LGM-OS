//
// Created by lichtgott on 06.07.25.
//

#include "interrupts.h"


void printf(char* str);
void printNumber(uint32_t number);


InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];
InterruptManager* InterruptManager::ActiveInterruptManager = nullptr;


void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t interrupt,
    uint16_t codeSegmentSelectorOffset,
    void (*handler) (),
    uint8_t DescriptorPrivilegeLevel,
    uint8_t DescriptorType
) {
    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interrupt].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interrupt].handlerAddressHighBits = ((uint32_t)handler >> 16) & 0xFFFF;
    interruptDescriptorTable[interrupt].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interrupt].access = IDT_DESC_PRESENT | DescriptorType | (DescriptorPrivilegeLevel & 3) << 5;
    interruptDescriptorTable[interrupt].reserved = 0;
}

InterruptManager::InterruptManager(GlobalDescriptorTable* gdt)
    :   picMasterCommand(0x20),
        picMasterData(0x21),
        picSlaveCommand(0xA0),
        picSlaveData(0xA1)
{


    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for(uint16_t i = 0; i < 256; i++) {
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

    SetInterruptDescriptorTableEntry(0x20,CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x21,CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    picMasterData.Write(0x20);
    picSlaveData.Write(0x28);

    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);

    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);

    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);


    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt));
}

InterruptManager::~InterruptManager() {
}

void InterruptManager::Activate()
{
    if (ActiveInterruptManager != nullptr)
        ActiveInterruptManager->Deactivate();
    ActiveInterruptManager = this;
    asm("sti");
}

void InterruptManager::Deactivate()
{
    if (ActiveInterruptManager == this)
    {
        ActiveInterruptManager = nullptr;
        asm("cli");
    }
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp) {
    if (ActiveInterruptManager != nullptr)
    {
        return ActiveInterruptManager->ObjectOrientedHandleInterrupt(interruptNumber, esp);
    }
    printf(" INTERRUPT");

    return esp;
}

uint32_t InterruptManager::ObjectOrientedHandleInterrupt(uint8_t interruptNumber, uint32_t esp)
{
    if (interruptNumber != 0x20)
    {
        printf("Interrupt");
    }

    if (0x20 <= interruptNumber && interruptNumber < 0x30)
    {
        picMasterCommand.Write(0x20);
        if (0x28 <= interruptNumber && interruptNumber < 0x30)
            picMasterData.Write(0x20);
    }

    return esp;
}