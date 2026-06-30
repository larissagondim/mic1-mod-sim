/* Nome do arquivo: Translator.hpp
 * Autoria: Larissa Gondim, Laura Morais, Maria Luiza Uchoa e Sérgio Gabriel
 * Data: 18/06/2026
 * Descrição: protótipo do Translator
 * Versão: 1.1
 */
 
 #ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include <string>
#include <vector>
#include <sstream>

/**
 * @brief Classe responsável pela tradução (compilação) de instruções em alto nível da ISA da IJVM.
 * * Atua como a unidade de controle de macroinstruções, decodificando mnemônicos textuais 
 * em sequências dinâmicas de microinstruções de 23 bits compatíveis com o hardware da Mic-1.
 *
 * Layout da microinstrução de 23 bits:
 *   [7:0]  Controle ULA: SLL8 SRA1 F0 F1 ENA ENB INVA INC
 *   [16:8] Barramento C: bit0=MAR bit1=MDR bit2=PC bit3=SP bit4=LV bit5=CPP bit6=TOS bit7=OPC bit8=H
 *   [18:17] Memória: bit0=READ bit1=WRITE
 *   [22:19] Barramento B: 0=MDR 1=PC 2=MBR 3=MBRU 4=SP 5=LV 6=CPP 7=TOS 8=OPC
 */
class Translator {
public:
    /**
     * @brief Converte uma linha textual da ISA da IJVM em uma sequência equivalente de microinstruções binárias.
     * * Mapeia dinamicamente os argumentos de comandos como BIPUSH e ILOAD para gerar os vetores de bits corretos.
     * * @param ijvmLine String contendo a instrução em alto nível da IJVM (ex: "BIPUSH 00110011", "DUP", "ILOAD 1").
     * @return std::vector<std::string> Vetor contendo a sequência ordenada de microinstruções em strings binárias de 23 bits.
     */
    static std::vector<std::string> translateToMicroinstructions(const std::string& ijvmLine) {
        std::vector<std::string> microinsts;
        std::stringstream ss(ijvmLine);
        std::string opcode;
        ss >> opcode;

        if (opcode == "DUP") {
            // Microinstrução 1: MAR = SP = SP + 1
            //   ULA: S = B + 1 -> F0=1 F1=1 ENB=1 INC=1 -> 00110101
            //   C-Bus: MAR(bit0) + SP(bit3) -> 000001001
            //   Mem: 00 (sem operação)
            //   B-Bus: SP(4) -> 0100
            microinsts.push_back("00110101000001001000100");
            // Microinstrução 2: MDR = TOS; wr (escreve TOS na memória[MAR])
            //   ULA: S = B -> F0=1 F1=1 ENB=1 -> 00110100
            //   C-Bus: MDR(bit1) -> 000000010
            //   Mem: 10 (WRITE: bit1=1, READ: bit0=0)
            //   B-Bus: TOS(7) -> 0111
            microinsts.push_back("00110100000000010100111");
        } 
        else if (opcode == "BIPUSH") {
            std::string byteStr;
            ss >> byteStr; // Extrai o argumento binário de 8 bits (o literal constante)

            // Microinstrução 1: SP = MAR = SP + 1 (aloca espaço na pilha)
            //   ULA: S = B + 1 -> 00110101
            //   C-Bus: MAR(bit0) + SP(bit3) -> 000001001
            //   Mem: 00
            //   B-Bus: SP(4) -> 0100
            microinsts.push_back("00110101000001001000100");
            // Microinstrução 2: Caso especial BIPUSH Fetch
            //   Os 8 bits do byte são passados para MBR e de MBR para H.
            //   READ e WRITE ativados simultaneamente (11) sinalizam o caso especial.
            //   C-Bus: 000000000 (H é carregado diretamente pelo tratamento especial em main.cpp)
            //   B-Bus: 0000 (irrelevante)
            microinsts.push_back(byteStr + "000000000110000"); 
            // Microinstrução 3: MDR = TOS = H; wr (persiste o valor carregado no topo da pilha)
            //   ULA: S = A -> F1=1 ENA=1 -> 00011000
            //   C-Bus: MDR(bit1) + TOS(bit6) -> 001000010
            //   Mem: 10 (WRITE: bit1=1, READ: bit0=0)
            //   B-Bus: 0000 (irrelevante)
            microinsts.push_back("00011000001000010100000");
        } 
        else if (opcode == "ILOAD") {
            int x;
            ss >> x; // Extrai o índice numérico correspondente à variável local requisitada

            // Microinstrução 1: H = LV (carrega base das variáveis locais em H)
            //   ULA: S = B -> F0=1 F1=1 ENB=1 -> 00110100
            //   C-Bus: H(bit8) -> 100000000
            //   Mem: 00
            //   B-Bus: LV(5) -> 0101
            microinsts.push_back("00110100100000000000101");

            // Geração dinâmica de ciclos de incremento (H = H + 1) para alcançar LV + x
            for (int i = 0; i < x; ++i) {
                //   ULA: S = A + 1 -> F1=1 ENA=1 INC=1 -> 00111001
                //   C-Bus: H(bit8) -> 100000000
                //   Mem: 00
                //   B-Bus: 0000 (irrelevante)
                microinsts.push_back("00111001100000000000000"); 
            }

            // Microinstrução: MAR = H; rd (lê o dado da memória no endereço LV + x)
            //   ULA: S = A -> F1=1 ENA=1 -> 00011000
            //   C-Bus: MAR(bit0) -> 000000001
            //   Mem: 01 (READ: bit0=1, WRITE: bit1=0)
            //   B-Bus: 0000 (irrelevante)
            microinsts.push_back("00011000000000001010000");
            // Microinstrução: MAR = SP = SP + 1 (aloca espaço na pilha para o novo valor)
            //   ULA: S = B + 1 -> 00110101
            //   C-Bus: MAR(bit0) + SP(bit3) -> 000001001
            //   Mem: 00
            //   B-Bus: SP(4) -> 0100
            microinsts.push_back("00110101000001001000100");
            // Microinstrução: TOS = MDR; wr (empilha o valor lido e atualiza o cache do topo)
            //   ULA: S = B -> F0=1 F1=1 ENB=1 -> 00110100
            //   C-Bus: TOS(bit6) -> 001000000
            //   Mem: 10 (WRITE: bit1=1, READ: bit0=0)
            //   B-Bus: MDR(0) -> 0000
            microinsts.push_back("00110100001000000100000");
        }

        return microinsts;
    }
};

#endif