    // void *imgg = (void *)syscall_FileOpen((char *)"/system/wallpapers/fennec.tga");
    // if (imgg == nullptr)
    // {
    //     syscall_dbg(0x3F8, (char *)"[LoadImage] Could not open file.\n");
    //     return;
    // }
    // syscall_dbg(0x3F8, (char *)"[LoadImage] Image opened.\n");

    // uint64_t sizeeeee = syscall_FileSize(imgg);

    // void *imgdata = malloc(sizeeeee);
    // syscall_dbg(0x3F8, (char *)"[LoadImage] IMAGE GOT SIZE.\n");
    // syscall_FileRead(imgg, 0, imgdata, sizeeeee);
    // syscall_dbg(0x3F8, (char *)"[LoadImage] Image Readed\n");
    // // unsigned int *image = jpg_parse((uint8_t *)imgdata, sizeeeee);
    // unsigned int *image = tga_parse((uint8_t *)imgdata, sizeeeee);
    // syscall_dbg(0x3F8, (char *)"[LoadImage] Image Parsed\n");
    // if (image == nullptr)
    //     syscall_dbg(0x3F8, (char *)"failed to parse image file!\n");

    // uint32_t x = Xi, y = 0;
    // for (size_t i = 0; i < image[0] * image[1]; i++)
    // {
    //     if (x <= width || y <= height)
    //         ((uint32_t *)address)[x + (y * ppsl)] = image[i];
    //     y++;
    //     if (y == image[1])
    //     {
    //         x++;
    //         y = 0;
    //     }
    // }
    // syscall_FileClose(imgg);
    // free(image);