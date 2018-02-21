void printReg(MIPS ir); //above main

printReg(curr_instruction); //Call in main


//Vinnie is going to be working on print out registers
void printReg(MIPS ir) {
    int rs = (ir >> 21) & 0x1F;
    int rt = (ir >> 16) & 0x1F;
    int rd = (ir >> 11) & 0x1F;

    int RsReg;
    if (rs == 0) {
        printf("rs: %d\n", rs);
    } else if (rs <= 3) {
        RsReg= rs - 2;
        printf("rs: %d\t$v%d\n", rs,RsReg);
    } else if (rs <= 7) {
        RsReg= rs - 4;
        printf("rs: %d\t$a%d\n", rs,RsReg);
    } else if (rs <= 15) {
        RsReg= rs - 8;
        printf("rs: %d\t$t%d\n", rs,RsReg);
    } else if (rs <= 23) {
        RsReg= rs - 16;
        printf("rs: %d\t$s%d\n", rs,RsReg);
    } else if (rs == 31) {
        printf("rs: %d\tReturn Address Register\n", rs);
    }

    int RtReg;

    if (rt == 0) {
            printf("rt: %d\n", rt);
        } else if (rt <= 3) {
            RtReg= rt - 2;
            printf("rt: %d\t$v%d\n", rt,RtReg);
        } else if (rt <= 7) {
            RtReg= rt - 4;
            printf("rt: %d\t$a%d\n", rt,RtReg);
        } else if (rt <= 15) {
            RtReg= rt - 8;
            printf("rt: %d\t$t%d\n", rt,RtReg);
        } else if (rt <= 23) {
            RtReg= rt - 16;
            printf("rt: %d\t$s%d\n", rt,RtReg);
        } else if (rt == 31) {
            printf("rt: %d\tReturn Address Register\n", rt);
        }

        int RdReg;

        if (rd == 0) {
            printf("rd: %d\n", rd);
        } else if (rd <= 3) {
            RdReg= rd - 2;
            printf("rd: %d\t$v%d\n", rd,RdReg);
        } else if (rd <= 7) {
            RdReg= rd - 4;
            printf("rd: %d\t$a%d\n", rd,RdReg);
        } else if (rd <= 15) {
            RdReg= rd - 8;
            printf("rd: %d\t$t%d\n", rd,RdReg);
        } else if (rd <= 23) {
            RdReg= rd - 16;
            printf("rd: %d\t$s%d\n", rd,RdReg);
        } else if (rd == 31) {
            printf("rd: %d\tReturn Address Register\n", rd);
        }
}
