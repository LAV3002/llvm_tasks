#include <iostream>
#include "elfio/elfio.hpp"
#include "sim.h"

using namespace ELFIO;

int main( int argc, char** argv )
{
    if ( argc != 2 ) {
        std::cout << "Usage: " << argv[0] << " <elf_file>" << std::endl;
        return 1;
    }

    // Create an elfio reader
    elfio reader;

    // Load ELF data
    if ( !reader.load( argv[1] ) ) {
        std::cout << "Can't find or process ELF file " << argv[1] << std::endl;
        return 2;
    }

    // Print ELF file sections info
    Elf_Half sec_num = reader.sections.size();
    const uint32_t* code = nullptr;
    Elf_Xword code_size = 0;
    Elf_Xword code_start = 0;
    for ( int i = 0; i < sec_num; ++i ) {
        section* psec = reader.sections[i];
        if (psec->get_name() == ".text") {
            code = (const uint32_t*)reader.sections[i]->get_data();
            code_size = psec->get_size() / 4;
            std::cout << "  [" << i << "] " << psec->get_name() << "\t"
                    << psec->get_size() << std::endl;
        }
    }

    for ( int i = 0; i < sec_num; ++i ) {
        section* psec = reader.sections[i];
        // Check section type
        if ( psec->get_type() == SHT_SYMTAB ) {
            const symbol_section_accessor symbols( reader, psec );
            for ( unsigned int j = 0; j < symbols.get_symbols_num(); ++j ) {
                std::string   name;
                Elf64_Addr    value;
                Elf_Xword     size;
                unsigned char bind;
                unsigned char type;
                Elf_Half      section_index;
                unsigned char other;

                // Read symbol properties
                symbols.get_symbol( j, name, value, size, bind, type,
                                    section_index, other );
                if (name == "app") {
                    code_start = value;
                    std::cout << j << " " << name << " " << value << std::endl;
                }
            }
        }
    }
    if (code) {
        const int REG_FILE_SIZE = 16;
        int32_t REG_FILE[REG_FILE_SIZE] = {};
        REG_FILE[0] = code_size + 1; // RA for exit
        REG_FILE[1] = 7500;
        uint8_t MEMORY[10000];
        uint32_t PC = code_start / 4;
        uint32_t NEXT_PC = PC + 1;
        uint32_t RUN = 1;
        for (int j = 0; j < 16; ++j) {
            printf("r%d: %i ", j, REG_FILE[j]);
        }
        printf("\n");
        std::cout << "[CODE] SIZE:" << code_size << "(x4) START:" << code_start << std::endl;

        simInit();

        int cnt = 0;
        while (RUN) {
            uint16_t r3_imm = code[PC] & 0xFFFF;
            uint8_t r6 = (code[PC] >> 0) & 0xF;
            uint8_t r5 = (code[PC] >> 4) & 0xF;
            uint8_t r4 = (code[PC] >> 8) & 0xF;
            uint8_t r3 = (code[PC] >> 12) & 0xF;
            uint8_t r2 = (code[PC] >> 16) & 0xF;
            uint8_t r1 = (code[PC] >> 20) & 0xF;
            uint32_t op = code[PC] >> 24;
            printf("%04x: %08x%s ", PC, code[PC], PC == code_start ? "<=" : "");
            uint32_t *ptr;
            switch (op) {
                default:
                    printf("[ERROR] PC %04x: Incorect opcode %02x\n", PC, op);
                    break;
                case 0x30:
                // ADD r1 r1 -1832
                    printf("ADD r%d r%d r%d\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] + REG_FILE[r3_imm];
                    break;
                case 0x31:
                // SUB
                    printf("SUB r%d r%d r%d\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] - REG_FILE[r3_imm];
                    break;
                case 0x32:
                // MUL
                    printf("MUL r%d r%d r%d\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] * REG_FILE[r3_imm];
                    break;
                case 0x33:
                // DIV
                    printf("DIV r%d r%d r%d\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] / REG_FILE[r3_imm];
                    break;
                case 0x34:
                // OR
                    printf("OR r%d r%d r%d\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] | REG_FILE[r3_imm];
                    break;
                case 0x35:
                // AND
                    printf("AND r%d r%d r%d\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] & REG_FILE[r3_imm];
                    break;
                case 0x36:
                // XOR
                    printf("XOR r%d r%d r%d\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] ^ REG_FILE[r3_imm];
                    break;
                case 0x37:
                // SHL
                    printf("SHL r%d r%d r%d\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] << REG_FILE[r3_imm];
                    break;
                case 0x38:
                // SRL
                    printf("SRL r%d r%d r%d\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] >> REG_FILE[r3_imm];
                    break;
                case 0x39:
                // SREM
                    printf("SREM r%d r%d r%d\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] % REG_FILE[r3_imm];
                    break;
                case 0x40:
                // ADDi r1 r1 -1832
                    if (r1 == r2 && r1 == 1) {
                        printf("ADDi r%d r%d %i\n", r1, r2, (int16_t)r3_imm);
                        REG_FILE[r1] = REG_FILE[r2] + (int16_t)r3_imm;
                    } else {
                        printf("ADDi r%d r%d %i\n", r1, r2, r3_imm);
                        REG_FILE[r1] = REG_FILE[r2] + r3_imm;
                    }
                    break;
                case 0x41:
                // SUBi
                    printf("SUBi r%d r%d %i\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] - r3_imm;
                    break;
                case 0x42:
                // MULi
                    printf("MULi r%d r%d %i\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] * r3_imm;
                    break;
                case 0x44:
                // ORi
                    printf("ORi r%d r%d %i\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] | r3_imm;
                    break;
                case 0x45:
                // XORi
                    printf("XORi r%d r%d %i\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] ^ r3_imm;
                    break;
                case 0x46:
                // SHLI aaaaaaaaaaaaaaaaaa
                    printf("SHLI r%d r%d %i\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] << r3_imm;
                    break;
                case 0x47:
                // SRLI aaaaaaaaaaaaaaaaaaa
                    printf("SRLI r%d r%d %i\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] >> r3_imm;
                    break;
                case 0x48:
                // SREMI
                    printf("SREMI r%d r%d %i\n", r1, r2, r3_imm);
                    REG_FILE[r1] = REG_FILE[r2] % r3_imm;
                    break;
                case 0x50:
                // ADDRMI
                    printf("ADDRMI r%d r%d %i\n", r1, r2, r3_imm);
                    REG_FILE[r1] = (REG_FILE[r2] + r3_imm) % r3_imm;
                    break;
                case 0x70:
                // B.EQ
                    printf("B.EQ r%d r%d %i\n", r1, r2, r3_imm);
                    if (REG_FILE[r1] == REG_FILE[r2]) {
                        NEXT_PC = PC + (int16_t)r3_imm;
                    }
                    break;
                case 0x71:
                // B.NE
                    printf("B.NE r%d r%d %i\n", r1, r2, r3_imm);
                    if (REG_FILE[r1] != REG_FILE[r2]) {
                        NEXT_PC = PC + (int16_t)r3_imm;
                    }
                    break;
                case 0x72:
                // B.LE
                    printf("B.LE r%d r%d %i\n", r1, r2, r3_imm);
                    if (REG_FILE[r1] <= REG_FILE[r2]) {
                        NEXT_PC = PC + (int16_t)r3_imm;
                    }
                    break;
                case 0x73:
                // B.GT
                    printf("B.GT r%d r%d %i\n", r1, r2, r3_imm);
                    if (REG_FILE[r1] > REG_FILE[r2]) {
                        NEXT_PC = PC + (int16_t)r3_imm;
                    }
                    break;
                case 0x74:
                // B.LEU
                    printf("B.LEU r%d r%d %i\n", r1, r2, r3_imm);
                    if ((uint32_t)REG_FILE[r1] <= (uint32_t)REG_FILE[r2]) {
                        NEXT_PC = PC + (int16_t)r3_imm;
                    }
                    break;
                case 0x75:
                // B.GEU
                    printf("B.GEU r%d r%d %i\n", r1, r2, r3_imm);
                    if ((uint32_t)REG_FILE[r1] >= (uint32_t)REG_FILE[r2]) {
                        NEXT_PC = PC + (int16_t)r3_imm;
                    }
                    break;
                case 0x76:
                // B.GTU
                    printf("B.GTU r%d r%d %i\n", r1, r2, r3_imm);
                    if ((uint32_t)REG_FILE[r1] > (uint32_t)REG_FILE[r2]) {
                        NEXT_PC = PC + (int16_t)r3_imm;
                    }
                    break;
                case 0x77:
                // B.LTU
                    printf("B.LTU r%d r%d %i\n", r1, r2, r3_imm);
                    if ((uint32_t)REG_FILE[r1] < (uint32_t)REG_FILE[r2]) {
                        NEXT_PC = PC + (int16_t)r3_imm;
                    }
                    break;
                case 0x80:
                // SEQ
                    printf("SEQ r%d r%d r%d r%d\n", r1, r2, r3, r4);
                    if (REG_FILE[r1] == REG_FILE[r2]) {
                        REG_FILE[r1] = REG_FILE[r3];
                    } else {
                        REG_FILE[r1] = REG_FILE[r4];
                    }
                    break;
                case 0x81:
                // SNE
                    printf("SNE r%d r%d r%d r%d\n", r1, r2, r3, r4);
                    if (REG_FILE[r1] != REG_FILE[r2]) {
                        REG_FILE[r1] = REG_FILE[r3];
                    } else {
                        REG_FILE[r1] = REG_FILE[r4];
                    }
                    break;
                case 0x82:
                // SLE
                    printf("SLE r%d r%d r%d r%d\n", r1, r2, r3, r4);
                    if (REG_FILE[r1] <= REG_FILE[r2]) {
                        REG_FILE[r1] = REG_FILE[r3];
                    } else {
                        REG_FILE[r1] = REG_FILE[r4];
                    }
                    break;
                case 0x83:
                // SGT
                    printf("SGT r%d r%d r%d r%d\n", r1, r2, r3, r4);
                    if (REG_FILE[r1] > REG_FILE[r2]) {
                        REG_FILE[r1] = REG_FILE[r3];
                    } else {
                        REG_FILE[r1] = REG_FILE[r4];
                    }
                    break;
                case 0x84:
                // SULE
                    printf("SULE r%d r%d r%d r%d\n", r1, r2, r3, r4);
                    if ((uint32_t)REG_FILE[r1] <= (uint32_t)REG_FILE[r2]) {
                        REG_FILE[r1] = REG_FILE[r3];
                    } else {
                        REG_FILE[r1] = REG_FILE[r4];
                    }
                    break;
                case 0x85:
                // SUGT
                    printf("SUGT r%d r%d r%d r%d\n", r1, r2, r3, r4);
                    if ((uint32_t)REG_FILE[r1] > (uint32_t)REG_FILE[r2]) {
                        REG_FILE[r1] = REG_FILE[r3];
                    } else {
                        REG_FILE[r1] = REG_FILE[r4];
                    }
                    break;
                case 0x86:
                // SUGE
                    printf("SUGE r%d r%d r%d r%d\n", r1, r2, r3, r4);
                    if ((uint32_t)REG_FILE[r1] >= (uint32_t)REG_FILE[r2]) {
                        REG_FILE[r1] = REG_FILE[r3];
                    } else {
                        REG_FILE[r1] = REG_FILE[r4];
                    }
                    break;
                case 0x87:
                // SULT
                    printf("SULT r%d r%d r%d r%d\n", r1, r2, r3, r4);
                    if ((uint32_t)REG_FILE[r1] < (uint32_t)REG_FILE[r2]) {
                        REG_FILE[r1] = REG_FILE[r3];
                    } else {
                        REG_FILE[r1] = REG_FILE[r4];
                    }
                    break;
                case 0xAA:
                // 0xAA MOVli r2 imm
                    printf("MOVli r%d %i\n", r1, r3_imm);
                    REG_FILE[r1] = r3_imm;
                    break;
                case 0xAF:
                // 0xAF MOVhi r15 65280
                    printf("MOVhi r%d %i\n", r1, r3_imm);
                    REG_FILE[r1] = r3_imm << 16;
                    break;
                case 0xB0:
                // B 
                    printf("B %i\n", r3_imm);
                    NEXT_PC = PC + (int16_t)r3_imm;
                    break;
                case 0xBA:
                // BL 
                    printf("BL r%d %i\n", r1, r3_imm);
                    REG_FILE[r1] = PC + 1;
                    NEXT_PC = PC + (int16_t)r3_imm;
                    break;
                case 0xBB:
                // BR 
                    printf("BR r%d\n", r1);
                    NEXT_PC = REG_FILE[r1];
                    break;
                case 0xCA:
                // STi r0 r1 1828 
                    printf("STi r%d r%d %i\n", r1, r2, r3_imm);
                    ptr = (uint32_t*)(MEMORY + REG_FILE[r2] + r3_imm);
                    ptr[0] = REG_FILE[r1];
                    break;
                case 0xCB:
                // LDi r13 r1 12
                    printf("LDi r%d r%d %i\n", r1, r2, r3_imm);
                    ptr = (uint32_t*)(MEMORY + REG_FILE[r2] + r3_imm);
                    REG_FILE[r1] = ptr[0];
                    break;
                case 0xF0:
                // FLUSH 
                    printf("FLUSH\n");
                    simFlush();
                    break;
                case 0xF1:
                // PUTPIXEL
                    printf("PUTPIXEL r%d r%d r%d\n", r1, r2, r3_imm);
                    simPutPixel(REG_FILE[r1], REG_FILE[r2], REG_FILE[r3_imm]);
                    break;
                case 0xF2:
                // RAND
                    printf("RAND r%d\n", r1);
                    REG_FILE[r1] = simRand();
                    break;
            }
            for (int j = 0; j < 16; ++j) {
                printf("r%d: %i ", j, REG_FILE[j]);
            }
            printf("\n");
            PC = NEXT_PC;
            NEXT_PC = PC + 1;
            if (0 > PC || PC > code_size) {
                printf("stooooooooooooop\n");
                RUN = 0;
            }
        }

        simExit();

    }
    return 0;
}
