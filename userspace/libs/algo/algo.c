unsigned int CyclicRedundancyCheck32(unsigned char *Data)
{
    static unsigned int Table[256];
    unsigned int Byte, CRC;

    if (Table[1] == 0)
    {
        unsigned int Mask;
        for (Byte = 0; Byte <= 255; Byte++)
        {
            CRC = Byte;
            for (int j = 7; j >= 0; j--)
            {
                Mask = -(CRC & 1);
                CRC = (CRC >> 1) ^ (0xEDB88320 & Mask);
            }
            Table[Byte] = CRC;
        }
    }

    int i = 0;
    CRC = 0xFFFFFFFF;
    while ((Byte = Data[i]) != 0)
    {
        CRC = (CRC >> 8) ^ Table[(CRC ^ Byte) & 0xFF];
        i = i + 1;
    }
    return ~CRC;
}
