#pragma once

#include <types.h>

/* https://wiki.osdev.org/Floppy_disk */
// TODO: current issues are that how tf i will read on separated floppy drive?

namespace Floppy
{
    enum FloppyRegisters
    {
        STATUS_REGISTER_A = 0x3F0,
        STATUS_REGISTER_B = 0x3F1,
        DIGITAL_OUTPUT_REGISTER = 0x3F2,
        TAPE_DRIVE_REGISTER = 0x3F3,
        MAIN_STATUS_REGISTER = 0x3F4,
        DATARATE_SELECT_REGISTER = 0x3F4,
        DATA_FIFO = 0x3F5,
        DIGITAL_INPUT_REGISTER = 0x3F7,
        CONFIGURATION_CONTROL_REGISTER = 0x3F7
    };

    enum FloppyCommands
    {
        READ_TRACK = 2,
        SPECIFY = 3,
        SENSE_DRIVE_STATUS = 4,
        WRITE_DATA = 5,
        READ_DATA = 6,
        RECALIBRATE = 7,
        SENSE_INTERRUPT = 8,
        WRITE_DELETED_DATA = 9,
        READ_ID = 10,
        READ_DELETED_DATA = 12,
        FORMAT_TRACK = 13,
        DUMPREG = 14,
        SEEK = 15,
        VERSION = 16,
        SCAN_EQUAL = 17,
        PERPENDICULAR_MODE = 18,
        CONFIGURE = 19,
        _LOCK = 20,
        VERIFY = 22,
        SCAN_LOW_OR_EQUAL = 25,
        SCAN_HIGH_OR_EQUAL = 29
    };

    struct FloppyDrive
    {
        string Name;
        virtual uint8_t read(uint8_t Sector, uint16_t SectorCount, uint8_t *Buffer)
        {
            fixme("Unimplmented function");
            Buffer = 0;
            UNUSED(Buffer);
            return false;
        }

        virtual void write(uint8_t Sector, uint16_t SectorCount, uint8_t *Buffer)
        {
            fixme("Unimplmented function");
            Buffer = 0;
            UNUSED(Buffer);
        }
    };

    class FloppyDriver
    {
    public:
        FloppyDrive Drives[2];
        FloppyDriver();
        ~FloppyDriver();

    private:
        string ReturnFloppyType(int i);
        void SendByte_FDC(char b);
        unsigned char RecByte_FDC();
        void Recal();
        void SeekTrackSide(unsigned char track, unsigned char side);
        void MotorOn();
        void MotorOff();
        void ResetFloppy();
        void WaitForIRQ();
    };
}
