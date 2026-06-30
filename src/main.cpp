/* Nome do arquivo: main.cpp
 * Autoria: Larissa Gondim, Laura Morais, Maria Luiza Uchoa e Sérgio Gabriel
 * Data: 26/06/2026
 * Descrição: simulação da arquitetura do MIC-1
 * Versão: 1.2
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
void dumpRegistradores(std::ofstream& logFile, const Microarchitecture& cpu) {
    logFile << "h   = " << toBinStr(cpu.h, 32) << "\n";
    logFile << "opc = " << toBinStr(cpu.opc, 32) << "\n";
    logFile << "tos = " << toBinStr(cpu.tos, 32) << "\n";
    logFile << "cpp = " << toBinStr(cpu.cpp, 32) << "\n";
    logFile << "lv  = " << toBinStr(cpu.lv, 32) << "\n";
    logFile << "sp  = " << toBinStr(cpu.sp, 32) << "\n";
    logFile << "mbr = " << toBinStr(cpu.mbr, 8)  << "\n";
    logFile << "pc  = " << toBinStr(cpu.pc, 32)  << "\n";
    logFile << "mdr = " << toBinStr(cpu.mdr, 32) << "\n";
    logFile << "mar = " << toBinStr(cpu.mar, 32) << "\n";
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
    dumpRegistradores(logFile, cpu);

    std::string IR;
    int cycle = 1;

    while (std::getline(instFile, IR)) {
        if (IR.empty()) continue;

        uint8_t ulaControl, busBSelection;
        uint16_t busCSelection;

        // Fatiamento da instrução de 21 bits (sem campo de memória)
        Decoder::split21Bits(IR, ulaControl, busCSelection, busBSelection);

        logFile << "\n============================================================\n";
        logFile << "Ciclo " << cycle << "\n";
        logFile << "ir = " << IR.substr(0, 8) << " " << IR.substr(8, 9) << " " << IR.substr(17, 4) << "\n";
        logFile << "b = " << getBBusName(busBSelection) << "\n";
        logFile << "c = " << getCBusNames(busCSelection) << "\n";

        logFile << "*******************************\n";
        logFile << "> Registradores antes da instrução\n";
        dumpRegistradores(logFile, cpu);

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

        logFile << "*******************************\n";
        logFile << "> Registradores depois da instrução\n";
        dumpRegistradores(logFile, cpu);

        cycle++;
    }

    logFile << "\nFim da simulacao da Etapa 2 - Tarefa 2.\n";
    instFile.close();
    logFile.close();
    std::cout << "Simulacao da Etapa 2 - Tarefa 2 concluida! Log em dados/etapa2/saida_etapa2_tarefa2.txt\n";
}

/**
 * @brief Executa um único ciclo de clock para uma microinstrução de 23 bits e registra no log.
 * 
 * Esta função auxiliar encapsula toda a lógica de decodificação, execução combinacional
 * (ULA + Deslocador), escrita nos barramentos e ciclo de memória para uma microinstrução.
 * Também realiza o registro completo do estado da máquina antes e depois da execução.
 * 
 * @param irStr String contendo os 23 bits binários da microinstrução atual.
 * @param cpu Referência para o objeto da microarquitetura (registradores e memória).
 * @param logFile Referência para o stream de saída do arquivo de log.
 * @param cycle Número do ciclo de clock atual (para identificação no log).
 * @param dumpMemory Se verdadeiro, imprime o estado completo da memória após a execução.
 */
void executarMicroinstrucao(const std::string& irStr, Microarchitecture& cpu, 
                            std::ofstream& logFile, int cycle, bool dumpMemory) {
    logFile << "\n============================================================\n";
    logFile << "Cycle " << cycle << "\n";

    uint8_t ulaControl, busBSelection, memControl;
    uint16_t busCSelection;

    // Fatiamento lógico da instrução em campos específicos de hardware
    Decoder::split23Bits(irStr, ulaControl, busCSelection, memControl, busBSelection);

    uint8_t readSignal  = (memControl >> 0) & 1;
    uint8_t writeSignal = (memControl >> 1) & 1;

    // Exibição do desmembramento do Registrador de Instrução (IR) e barramentos ativos
    logFile << "ir = " << irStr.substr(0, 8) << " " << irStr.substr(8, 9) << " " 
            << irStr.substr(17, 2) << " " << irStr.substr(19, 4) << "\n";
    logFile << "b = " << getBBusName(busBSelection) << "\n";
    logFile << "c = " << getCBusNames(busCSelection) << "\n";

    // LOG: Amostragem dos 10 registradores ANTES da execução combinacional
    logFile << "*******************************\n";
    logFile << "> Registradores antes da instrução\n";
    dumpRegistradores(logFile, cpu);

    // INÍCIO DO CLOCK COMBINACIONAL (Simulação física)
    uint32_t b_bus_value = cpu.readB_Bus(busBSelection);
    ALUResult alu_out;

    // Interceptor do caso especial do BIPUSH fetch (Tratamento síncrono imediato de H)
    // Os 8 bits do argumento são passados para o MBR e o valor de MBR é passado para H.
    if (readSignal == 1 && writeSignal == 1) {
        cpu.mbr = ulaControl;
        cpu.h = static_cast<uint32_t>(cpu.mbr);
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

    // Ativação do ciclo de barramento de memória (Apenas se não for o caso especial)
    if (!(readSignal == 1 && writeSignal == 1)) {
        cpu.executeMemoryCycle(readSignal, writeSignal);
    }
    // FIM DO CICLO DE CLOCK

    // LOG: Amostragem de TODOS os 10 registradores DEPOIS da execução e escrita nos barramentos
    logFile << "*******************************\n";
    logFile << "> Registradores depois da instrução\n";
    dumpRegistradores(logFile, cpu);

    // LOG: Despejo completo da memória RAM (opcional, controlado pelo parâmetro)
    if (dumpMemory) {
        logFile << "*******************************\n";
        logFile << "> Memória depois da instrução\n";
        for (uint32_t word : cpu.dataMemory) {
            logFile << toBinStr(word, 32) << "\n";
        }
    }
    
    logFile << "============================================================\n";
}

/**
 * @brief Executa a simulação da Etapa 3 - Tarefa 1 (microinstruções de 23 bits lidas diretamente de arquivo).
 * 
 * Carrega o estado inicial dos registradores e da memória, lê cada microinstrução de 23 bits
 * sequencialmente do arquivo de entrada e executa o ciclo de clock, gerando um log completo
 * com o estado da memória após cada microinstrução.
 * 
 * @note O arquivo de entrada (microinstrucoes_etapa3_tarefa1.txt) será fornecido pelo professor
 * durante a avaliação. Se não estiver presente, a função emite um aviso e retorna sem erro.
 */
void simularEtapa3Tarefa1() {
    Microarchitecture cpu;
    cpu.loadRegisters("dados/etapa3/registradores_etapa3_tarefa1.txt");
    cpu.loadMemory("dados/etapa3/dados_etapa3_tarefa1.txt");

    std::ifstream instFile("dados/etapa3/microinstrucoes_etapa3_tarefa1.txt");
    if (!instFile.is_open()) {
        std::cerr << "Arquivo microinstrucoes_etapa3_tarefa1.txt nao encontrado. Pulando Etapa 3 - Tarefa 1.\n";
        return;
    }
    std::ofstream logFile("dados/etapa3/saida_etapa3_tarefa1.txt");

    // Log: impressão do estado inicial do sistema
    logFile << "Estado Inicial da Memoria:\n";
    for (uint32_t word : cpu.dataMemory) {
        logFile << "\t" << toBinStr(word, 32) << "\n";
    }

    logFile << "\nEstado Inicial dos Registradores:\n";
    dumpRegistradores(logFile, cpu);

    logFile << "\nInicio da Simulacao\n";

    std::string IR;
    int cycle = 1;

    // Loop de leitura sequencial das microinstruções de 23 bits
    while (std::getline(instFile, IR)) {
        if (IR.empty()) continue;
        // Executa e registra cada microinstrução com dump completo da memória
        executarMicroinstrucao(IR, cpu, logFile, cycle, true);
        cycle++;
    }

    logFile << "> Linha vazia, fim do programa.\n";
    instFile.close();
    logFile.close();
    std::cout << "Simulacao da Etapa 3 - Tarefa 1 concluida! Log em dados/etapa3/saida_etapa3_tarefa1.txt\n";
}

/**
 * @brief Executa a simulação do Entregável Final (Interpretador IJVM completo).
 * 
 * Lê macroinstruções IJVM em alto nível (DUP, BIPUSH, ILOAD) do arquivo instrucoes.txt,
 * traduz cada uma em sequências de microinstruções de 23 bits via Translator, e executa
 * os ciclos de clock. O log imprime a memória após cada macroinstrução completa.
 */
void simularEntregavel() {
    Microarchitecture cpu;

    // Configura os caminhos dos arquivos locais conforme a estrutura de diretórios do repositório
    cpu.loadMemory("dados/etapa3/dados_etapa3_tarefa1.txt");

    std::ifstream instFile("dados/etapa3/instrucoes.txt");
    std::ofstream logFile("dados/etapa3/saida_entregavel.txt");

    if (!instFile.is_open() || !logFile.is_open()) {
        std::cerr << "Erro ao abrir os arquivos de instrucao ou de log do entregavel.\n";
        return;
    }

    std::string ijvmLine;
    int cycle = 1;

    // Log: impressão do estado inicial do sistema
    logFile << "Estado Inicial da Memoria:\n";
    for (uint32_t word : cpu.dataMemory) {
        logFile << "\t" << toBinStr(word, 32) << "\n";
    }

    logFile << "\nEstado Inicial dos Registradores:\n";
    dumpRegistradores(logFile, cpu);

    logFile << "\nInicio do Programa\n";

    // Loop de leitura do arquivo de macroinstruções da ISA da IJVM
    while (std::getline(instFile, ijvmLine)) {
        if (ijvmLine.empty()) continue;

        // Tradução dinâmica de mnemônicos textuais para blocos de microcódigo de 23 bits
        std::vector<std::string> microcode = Translator::translateToMicroinstructions(ijvmLine);

        // Loop interno que simula os ciclos de clock rígidos para cada microinstrução
        for (const auto& irStr : microcode) {
            // Executa a microinstrução sem dump de memória (será feito após a macroinstrução)
            executarMicroinstrucao(irStr, cpu, logFile, cycle, false);
            cycle++;
        }

        // LOG: Despejo completo das linhas da memória RAM após execução da macroinstrução
        logFile << "*******************************\n";
        logFile << "> Memória depois da instrução\n";
        for (uint32_t word : cpu.dataMemory) {
            logFile << toBinStr(word, 32) << "\n";
        }
        logFile << "============================================================\n";
    }

    // Tratamento de finalização de fluxo de programa (EOP) conforme o gabarito
    logFile << "> Linha vazia, fim do programa.\n";

    instFile.close();
    logFile.close();

    std::cout << "Simulacao do entregavel concluida com sucesso! Log em dados/etapa3/saida_entregavel.txt\n";
}

int main() {
    simularEtapa1();
    simularEtapa2Tarefa1();
    simularEtapa2Tarefa2();
    simularEntregavel();
    simularEtapa3Tarefa1();
    return 0;
}