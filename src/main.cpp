/* Nome do arquivo: main.cpp
 * Autoria: Larissa Gondim, Laura Morais, Maria Luiza Uchoa e Sérgio Gabriel
 * Data: 26/06/2026
 * Descrição: simulação da arquitetura do MIC-1
 * Versão: 1.1
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>
#include "Microarchitecture.hpp"
#include "ALU.hpp"
#include "Decoder.hpp"
#include "Translator.hpp"

/**
 * @brief Função utilitária para converter valores inteiros em strings binárias de tamanho fixo.
 * @param val O valor numérico a ser convertido.
 * @param bits A quantidade de bits desejada na string de saída (8, 9 ou 32).
 * @return std::string A string formatada com preenchimento de zeros à esquerda.
 */
std::string toBinStr(uint32_t val, int bits) {
    if (bits == 32) return std::bitset<32>(val).to_string();
    if (bits == 9)  return std::bitset<9>(val).to_string();
    if (bits == 8)  return std::bitset<8>(val).to_string();
    return "";
}

/**
 * @brief Mapeia textualmente qual registrador está ativo no Barramento B para fins de log.
 * @param selection Valor numérico de 4 bits vindo do IR.
 * @return std::string Nome descritivo do registrador ou barramento.
 */
std::string getBBusName(uint8_t selection) {
    switch (selection) {
        case 0: return "mdr";
        case 1: return "pc";
        case 2: return "mbr";
        case 3: return "mbru";
        case 4: return "sp";
        case 5: return "lv";
        case 6: return "cpp";
        case 7: return "tos";
        case 8: return "opc";
        default: return "nenhum";
    }
}

/**
 * @brief Identifica textualmente quais registradores receberão escrita pelo Barramento C.
 * @param selection Máscara de 9 bits vinda do IR.
 * @return std::string Lista de registradores habilitados separados por vírgula.
 */
std::string getCBusNames(uint16_t selection) {
    std::string names = "";
    if ((selection >> 0) & 1) names += "mar, ";
    if ((selection >> 1) & 1) names += "mdr, ";
    if ((selection >> 2) & 1) names += "pc, ";
    if ((selection >> 3) & 1) names += "sp, ";
    if ((selection >> 4) & 1) names += "lv, ";
    if ((selection >> 5) & 1) names += "cpp, ";
    if ((selection >> 6) & 1) names += "tos, ";
    if ((selection >> 7) & 1) names += "opc, ";
    if ((selection >> 8) & 1) names += "h, ";
    
    if (!names.empty()) {
        names = names.substr(0, names.length() - 2); // Remove a última vírgula e espaço
    } else {
        names = "nenhum";
    }
    return names;
}

/**
 * @brief Executa a simulação específica da etapa 1, que é a validação da ULA isolada.
 * Lê o arquivo programa_etapa1.txt de 6 bits e gera o log saida_etapa1.txt.
 */
void simularEtapa1() {
    std::ifstream instFile("dados/etapa1/programa_etapa1.txt");
    std::ofstream logFile("dados/etapa1/saida_etapa1.txt");

    if (!instFile.is_open() || !logFile.is_open()) {
        std::cerr << "Erro ao abrir os arquivos da Etapa 1.\n";
        return;
    }

    // Especificação da etapa 1: varoáveis para A, B, PC e IR
    uint32_t A = 1;
    uint32_t B = 1;
    uint32_t PC = 1;
    std::string IR;

    logFile << "Simulacao da Etapa 1 - Teste da ULA\n";
    logFile << "Valores iniciais: A = " << A << ", B = " << B << "\n\n";

    while (std::getline(instFile, IR)) {
        if (IR.empty()) continue;

        // Decodificação da palavra de 6 bits: F0 F1 ENA ENB INVA INC
        uint8_t f0   = IR[0] - '0';
        uint8_t f1   = IR[1] - '0';
        uint8_t ena  = IR[2] - '0';
        uint8_t enb  = IR[3] - '0';
        uint8_t inva = IR[4] - '0';
        uint8_t inc  = IR[5] - '0';

        // Execução da ULA combinacional (sem shifts da Etapa 2)
        ALUResult alu_out = ALU::execute(A, B, f0, f1, ena, enb, inva, inc, 0, 0);

        // Registro no log conforme exigência da etapa 1: IR, PC, A, B, S e Vai-um
        logFile << "Linha " << PC << " (PC = " << PC << "):\n";
        logFile << "\tIR = " << IR << "\n";
        logFile << "\tPC = " << PC << "\n";
        logFile << "\tA = " << toBinStr(A, 32) << " (" << static_cast<int32_t>(A) << ")\n";
        logFile << "\tB = " << toBinStr(B, 32) << " (" << static_cast<int32_t>(B) << ")\n";
        logFile << "\tS = " << toBinStr(alu_out.result, 32) << " (" << static_cast<int32_t>(alu_out.result) << ")\n";
        logFile << "\tVai-um = " << static_cast<int>(alu_out.carryOut) << "\n\n";

        PC++;
    }

    logFile << "Fim da simulacao da Etapa 1.\n";
    instFile.close();
    logFile.close();
    std::cout << "Simulacao da Etapa 1 concluida com sucesso! Log gerado em dados/etapa1/saida_etapa1.txt\n";
}

/**
 * @brief Executa a simulação da Etapa 2 - Tarefa 1 (ULA de 8 bits com deslocador e flags N/Z).
 * Lê o arquivo programa_etapa2_tarefa1.txt de 8 bits e gera o log saida_etapa2_tarefa1.txt.
 */
void simularEtapa2Tarefa1() {
    std::ifstream instFile("dados/etapa2/programa_etapa2_tarefa1.txt");
    std::ofstream logFile("dados/etapa2/saida_etapa2_tarefa1.txt");

    if (!instFile.is_open() || !logFile.is_open()) {
        std::cerr << "Erro ao abrir os arquivos da Etapa 2 - Tarefa 1.\n";
        return;
    }

    // Mesma convenção da Etapa 1: A e B fixos para testar a ULA isolada.
    uint32_t A = 1;
    uint32_t B = 1;
    uint32_t PC = 1;
    std::string IR;

    logFile << "Simulacao da Etapa 2 - Tarefa 1 (ULA de 8 bits: SLL8 SRA1 F0 F1 ENA ENB INVA INC)\n";
    logFile << "Valores iniciais: A = " << A << ", B = " << B << "\n\n";

    while (std::getline(instFile, IR)) {
        if (IR.empty()) continue;

        // Decodificação da palavra de 8 bits: SLL8 SRA1 F0 F1 ENA ENB INVA INC
        uint8_t sll8 = IR[0] - '0';
        uint8_t sra1 = IR[1] - '0';
        uint8_t f0   = IR[2] - '0';
        uint8_t f1   = IR[3] - '0';
        uint8_t ena  = IR[4] - '0';
        uint8_t enb  = IR[5] - '0';
        uint8_t inva = IR[6] - '0';
        uint8_t inc  = IR[7] - '0';

        // Execução da ULA combinacional com o deslocador (Sd) e as flags N e Z
        ALUResult alu_out = ALU::execute(A, B, f0, f1, ena, enb, inva, inc, sll8, sra1);

        // Saídas exigidas pela Tarefa 1: Sd (saída deslocada), Vai-um, N e Z
        logFile << "Linha " << PC << " (PC = " << PC << "):\n";
        logFile << "\tIR = " << IR << "\n";
        logFile << "\tPC = " << PC << "\n";
        logFile << "\tA  = " << toBinStr(A, 32) << " (" << static_cast<int32_t>(A) << ")\n";
        logFile << "\tB  = " << toBinStr(B, 32) << " (" << static_cast<int32_t>(B) << ")\n";
        logFile << "\tSd = " << toBinStr(alu_out.result, 32) << " (" << static_cast<int32_t>(alu_out.result) << ")\n";
        logFile << "\tVai-um = " << static_cast<int>(alu_out.carryOut) << "\n";
        logFile << "\tN = " << static_cast<int>(alu_out.flagN) << "\n";
        logFile << "\tZ = " << static_cast<int>(alu_out.flagZ) << "\n\n";

        PC++;
    }

    logFile << "Fim da simulacao da Etapa 2 - Tarefa 1.\n";
    instFile.close();
    logFile.close();
    std::cout << "Simulacao da Etapa 2 - Tarefa 1 concluida! Log em dados/etapa2/saida_etapa2_tarefa1.txt\n";
}

/**
 * @brief Imprime no log o estado dos 10 registradores da Mic-1 com a indentação informada.
 */
void dumpRegistradores(std::ofstream& logFile, const Microarchitecture& cpu, const std::string& tab) {
    logFile << tab << "h   = " << toBinStr(cpu.h, 32)   << "\n" << tab << "opc = " << toBinStr(cpu.opc, 32) << "\n";
    logFile << tab << "tos = " << toBinStr(cpu.tos, 32) << "\n" << tab << "cpp = " << toBinStr(cpu.cpp, 32) << "\n";
    logFile << tab << "lv  = " << toBinStr(cpu.lv, 32)  << "\n" << tab << "sp  = " << toBinStr(cpu.sp, 32)  << "\n";
    logFile << tab << "mbr = " << toBinStr(cpu.mbr, 8)  << "\n" << tab << "pc  = " << toBinStr(cpu.pc, 32)  << "\n";
    logFile << tab << "mdr = " << toBinStr(cpu.mdr, 32) << "\n" << tab << "mar = " << toBinStr(cpu.mar, 32) << "\n";
}

/**
 * @brief Executa a simulação da Etapa 2 - Tarefa 2 (caminho de dados, instruções de 21 bits).
 * Lê o estado inicial dos registradores e o programa de 21 bits, sem acesso à memória.
 * Gera o log saida_etapa2_tarefa2.txt com os registradores antes/depois de cada instrução.
 */
void simularEtapa2Tarefa2() {
    Microarchitecture cpu;
    cpu.loadRegisters("dados/etapa2/registradores_etapa2_tarefa2.txt");

    std::ifstream instFile("dados/etapa2/programa_etapa2_tarefa2.txt");
    std::ofstream logFile("dados/etapa2/saida_etapa2_tarefa2.txt");

    if (!instFile.is_open() || !logFile.is_open()) {
        std::cerr << "Erro ao abrir os arquivos da Etapa 2 - Tarefa 2.\n";
        return;
    }

    logFile << "Simulacao da Etapa 2 - Tarefa 2 (Caminho de dados, instrucoes de 21 bits)\n";
    logFile << "Formato da instrucao: Controle da ULA (8) | Barramento C (9) | Barramento B (4)\n";

    logFile << "\nEstado Inicial dos Registradores:\n";
    dumpRegistradores(logFile, cpu, "\t");

    std::string IR;
    int cycle = 1;

    while (std::getline(instFile, IR)) {
        if (IR.empty()) continue;

        uint8_t ulaControl, busBSelection;
        uint16_t busCSelection;

        // Fatiamento da instrução de 21 bits (sem campo de memória)
        Decoder::split21Bits(IR, ulaControl, busCSelection, busBSelection);

        logFile << "\nInstrucao " << cycle << "\n";
        logFile << "\tir = " << IR.substr(0, 8) << " " << IR.substr(8, 9) << " " << IR.substr(17, 4) << "\n";
        logFile << "\tbarramento B = " << getBBusName(busBSelection) << "\n";
        logFile << "\tbarramento C = " << getCBusNames(busCSelection) << "\n";

        logFile << "\tRegistradores antes da instrucao:\n";
        dumpRegistradores(logFile, cpu, "\t\t");

        // Caminho de dados: A = H sempre; B = registrador no barramento B
        uint32_t b_bus_value = cpu.readB_Bus(busBSelection);

        uint8_t sll8 = (ulaControl >> 7) & 1;
        uint8_t sra1 = (ulaControl >> 6) & 1;
        uint8_t f0   = (ulaControl >> 5) & 1;
        uint8_t f1   = (ulaControl >> 4) & 1;
        uint8_t ena  = (ulaControl >> 3) & 1;
        uint8_t enb  = (ulaControl >> 2) & 1;
        uint8_t inva = (ulaControl >> 1) & 1;
        uint8_t inc  = (ulaControl >> 0) & 1;

        ALUResult alu_out = ALU::execute(cpu.h, b_bus_value, f0, f1, ena, enb, inva, inc, sll8, sra1);

        // Escrita da saída deslocada nos registradores habilitados pelo barramento C
        cpu.writeC_Bus(busCSelection, alu_out.result);

        logFile << "\tRegistradores apos a instrucao:\n";
        dumpRegistradores(logFile, cpu, "\t\t");

        cycle++;
    }

    logFile << "\nFim da simulacao da Etapa 2 - Tarefa 2.\n";
    instFile.close();
    logFile.close();
    std::cout << "Simulacao da Etapa 2 - Tarefa 2 concluida! Log em dados/etapa2/saida_etapa2_tarefa2.txt\n";
}

int main() {
    simularEtapa1();
    simularEtapa2Tarefa1();
    simularEtapa2Tarefa2();

    Microarchitecture cpu;
    
    // Configura os caminhos dos arquivos locais conforme a estrutura de diretórios do repositório
    cpu.loadMemory("dados/etapa3/dados_etapa3_tarefa1.txt");
    
    std::ifstream instFile("dados/etapa3/instrucoes.txt");
    std::ofstream logFile("dados/etapa3/saida_etapa3_tarefa1.txt");
    
    if (!instFile.is_open() || !logFile.is_open()) {
        std::cerr << "Erro ao abrir os arquivos de instrução ou de log.\n";
        return 1;
    }

    std::string ijvmLine;
    int cycle = 1;

    // Log: impressão do estado inicial do sistema
    logFile << "Estado Inicial da Memoria:\n";
    for (uint32_t word : cpu.dataMemory) {
        logFile << "\t" << toBinStr(word, 32) << "\n";
    }
    
    logFile << "\nEstado Inicial dos Registradores:\n";
    logFile << "\tmar = " << toBinStr(cpu.mar, 32) << "\n\tmdr = " << toBinStr(cpu.mdr, 32) << "\n";
    logFile << "\tpc  = " << toBinStr(cpu.pc, 32)  << "\n\tmbr = " << toBinStr(cpu.mbr, 8)  << "\n";
    logFile << "\tsp  = " << toBinStr(cpu.sp, 32)  << "\n\tlv  = " << toBinStr(cpu.lv, 32)  << "\n";
    logFile << "\tcpp = " << toBinStr(cpu.cpp, 32) << "\n\ttos = " << toBinStr(cpu.tos, 32) << "\n";
    logFile << "\topc = " << toBinStr(cpu.opc, 32) << "\n\th   = " << toBinStr(cpu.h, 32)   << "\n";
    
    logFile << "\nInicio do Programa\n";

    // Loop de leitura do arquivo de macroinstruções da ISA da IJVM
    while (std::getline(instFile, ijvmLine)) {
        if (ijvmLine.empty()) continue;

        // Tradução dinâmica de mnemônicos textuais para blocos de microcódigo de 23 bits
        std::vector<std::string> microcode = Translator::translateToMicroinstructions(ijvmLine);

        // Loop interno que simula os ciclos de clock rígidos para cada microinstrução
        for (const auto& irStr : microcode) {
            logFile << "\nCiclo " << cycle << "\n";
            
            uint8_t ulaControl, busBSelection, memControl;
            uint16_t busCSelection;
            
            // Fatiamento lógico da instrução em campos específicos de hardware
            Decoder::split23Bits(irStr, ulaControl, busCSelection, memControl, busBSelection);
            
            uint8_t readSignal  = (memControl >> 0) & 1;
            uint8_t writeSignal = (memControl >> 1) & 1;

            // Exibição do desmembramento do Registrador de Instrução (IR) e barramentos ativos
            logFile << "\tir = " << irStr.substr(0, 8) << " " << irStr.substr(8, 9) << " " 
                    << irStr.substr(17, 2) << " " << irStr.substr(19, 4) << "\n";
            logFile << "\tbarramento B = " << getBBusName(busBSelection) << "\n";
            logFile << "\tbarramento C = " << getCBusNames(busCSelection) << "\n";

            // LOG: Amostragem dos 10 registradores ANTES da execução combinacional
            logFile << "\tRegistradores antes da instrucao:\n";
            logFile << "\t\tmar = " << toBinStr(cpu.mar, 32) << "\n\t\tmdr = " << toBinStr(cpu.mdr, 32) << "\n";
            logFile << "\t\tpc  = " << toBinStr(cpu.pc, 32)  << "\n\t\tmbr = " << toBinStr(cpu.mbr, 8)  << "\n";
            logFile << "\t\tsp  = " << toBinStr(cpu.sp, 32)  << "\n\t\tlv  = " << toBinStr(cpu.lv, 32)  << "\n";
            logFile << "\t\tcpp = " << toBinStr(cpu.cpp, 32) << "\n\t\ttos = " << toBinStr(cpu.tos, 32) << "\n";
            logFile << "\t\topc = " << toBinStr(cpu.opc, 32) << "\n\t\th   = " << toBinStr(cpu.h, 32)   << "\n";

            // INÍCIO DO CLOCK COMBINACIONAL (Simulação física)
            uint32_t b_bus_value = cpu.readB_Bus(busBSelection);
            ALUResult alu_out;

            // Interceptor do caso especial do BIPUSH fetch (Tratamento síncrono imediato de H)
            if (readSignal == 1 && writeSignal == 1) {
                cpu.h = ulaControl; // H é carregado diretamente com os bits do opcode (argumento)
                alu_out.result = cpu.h;
            } else {
                // Desmembramento bit a bit do bloco de 8 bits da ULA para processamento combinacional
                uint8_t sll8 = (ulaControl >> 7) & 1;
                uint8_t sra1 = (ulaControl >> 6) & 1;
                uint8_t f0   = (ulaControl >> 5) & 1;
                uint8_t f1   = (ulaControl >> 4) & 1;
                uint8_t ena  = (ulaControl >> 3) & 1;
                uint8_t enb  = (ulaControl >> 2) & 1;
                uint8_t inva = (ulaControl >> 1) & 1;
                uint8_t inc  = (ulaControl >> 0) & 1;

                // Propagação de sinais no circuito combinacional da ULA e do Deslocador
                alu_out = ALU::execute(cpu.h, b_bus_value, f0, f1, ena, enb, inva, inc, sll8, sra1);
                
                // Gravação síncrona nos registradores habilitados pelo Barramento C
                cpu.writeC_Bus(busCSelection, alu_out.result);
            }

            // Ativação controlled do ciclo de barramento de memória (Apenas se não for o caso especial)
            if (!(readSignal == 1 && writeSignal == 1)) {
                cpu.executeMemoryCycle(readSignal, writeSignal);
            }
            // FIM DO CICLO DE CLOCK

            // LOG: Amostragem dos 10 registradores DEPOIS da execução e escrita nos barramentos
            logFile << "\tRegistradores apos a instrucao:\n";
            logFile << "\t\tmar = " << toBinStr(cpu.mar, 32) << "\n\t\tmdr = " << toBinStr(cpu.mdr, 32) << "\n";
            logFile << "\t\tsp  = " << toBinStr(cpu.sp, 32)  << "\n\t\ttos = " << toBinStr(cpu.tos, 32) << "\n";
            logFile << "\t\th   = " << toBinStr(cpu.h, 32)   << "\n";

            // LOG: Despejo completo das linhas da memória RAM atualizada ao final do ciclo
            logFile << "\tMemoria apos a instrucao:\n";
            for (uint32_t word : cpu.dataMemory) {
                logFile << "\t\t" << toBinStr(word, 32) << "\n";
            }
            
            cycle++;
        }
    }
    
    // Tratamento de finalização de fluxo de programa (EOP) conforme o gabarito
    logFile << "\nFim do Programa.\nCiclo " << cycle << ": Nao ha mais linhas (EOP).\n";
    
    instFile.close();
    logFile.close();
    
    std::cout << "Simulacao concluida com sucesso! Log em portugues gerado.\n";
    return 0;
}