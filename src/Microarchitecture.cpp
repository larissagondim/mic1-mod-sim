/* Nome do arquivo: Microarchitecture.cpp
 * Autoria: Larissa Gondim, Laura Morais, Maria Luiza Uchoa e Sérgio Gabriel
 * Data: 18/06/2026
 * Descrição: implementação das funções da Microarquitetura
 * Versão: 1.0
 */

#include "Microarchitecture.hpp"
#include <fstream>
#include <iostream>

Microarchitecture::Microarchitecture() {
    mar = mdr = pc = sp = lv = cpp = tos = opc = h = 0;
    mbr = 0;
    dataMemory.resize(16, 0); // Tamanho padronizado com base nas especificações de linhas de memória
}

void Microarchitecture::loadRegisters(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string line;

    // Ordem fixa do arquivo de estado inicial (um valor binario por linha):
    // H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR e, por fim, MBR (8 bits).
    uint32_t* ordem[] = {&h, &opc, &tos, &cpp, &lv, &sp, &pc, &mdr, &mar};
    size_t idx = 0;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (idx < 9) {
            *ordem[idx] = std::stoul(line, nullptr, 2); // registradores de 32 bits
        } else if (idx == 9) {
            mbr = static_cast<uint8_t>(std::stoul(line, nullptr, 2)); // MBR de 8 bits
        }
        idx++;
    }
}

void Microarchitecture::loadMemory(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string line;
    size_t idx = 0;
    while (std::getline(file, line) && idx < dataMemory.size()) {
        if (!line.empty()) {
            dataMemory[idx] = std::stoul(line, nullptr, 2);
            idx++;
        }
    }
}

uint32_t Microarchitecture::readB_Bus(uint8_t decoderSelection) {
    // Simulação do decodificador de 4 bits para seleção do barramento B
    switch (decoderSelection) {
        case 0: return mdr;
        case 1: return pc;
        case 2: 
            // MBR: Realiza a extensão de sinal obrigatória a partir do bit de sinal (bit 7)
            return static_cast<uint32_t>(static_cast<int32_t>(static_cast<int8_t>(mbr)));
        case 3: 
            // MBRU: Realiza o preenchimento estrito com zeros à esquerda
            return static_cast<uint32_t>(mbr);
        case 4: return sp;
        case 5: return lv;
        case 6: return cpp;
        case 7: return tos;
        case 8: return opc;
        default: return 0;
    }
}

void Microarchitecture::writeC_Bus(uint16_t encoderSelection, uint32_t value) {
    // Simulação do seletor de 9 bits mapeado bit a bit (permite escrita simultânea)
    if ((encoderSelection >> 0) & 1) mar = value;
    if ((encoderSelection >> 1) & 1) mdr = value;
    if ((encoderSelection >> 2) & 1) pc  = value;
    if ((encoderSelection >> 3) & 1) sp  = value;
    if ((encoderSelection >> 4) & 1) lv  = value;
    if ((encoderSelection >> 5) & 1) cpp = value;
    if ((encoderSelection >> 6) & 1) tos = value;
    if ((encoderSelection >> 7) & 1) opc = value;
    if ((encoderSelection >> 8) & 1) h   = value;
}

void Microarchitecture::executeMemoryCycle(uint8_t readSignal, uint8_t writeSignal) {
    // Gerenciamento síncrono das operações de leitura e escrita indexadas pelo MAR
    if (readSignal == 1 && mar < dataMemory.size()) {
        mdr = dataMemory[mar]; // Operação de READ
    }
    if (writeSignal == 1 && mar < dataMemory.size()) {
        dataMemory[mar] = mdr; // Operação de WRITE
    }
}