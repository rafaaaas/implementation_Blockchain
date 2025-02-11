#include "blockchain.h"

int main() {
  setlocale(1, "Portuguese");
  // Criar a blockchain
  Blockchain *cadeia = criaBlockchain();
  if (!cadeia) {
    printf("Erro ao criar a cadeia!\n");
    return 1;
  }

  // Adicionar dois blocos com transações
  printf("Criando blocos e minerando...\n");
  criarBloco("Transacao 1: Alice -> Bob", cadeia);
  criarBloco("Transacao 2: Bob -> Charlie", cadeia);
  criarBloco("Transacao 2: Bob -> Chçarlie", cadeia);
  criarBloco("Transacao 3: Bob -> Charlie", cadeia);
  criarBloco("Transacao 4: Bob -> Charlie", cadeia);

  if (verificar_transação(cadeia->ultimo_bloco->arvore->raiz,
                          "Transacao Hackeada: Eve -> Alice"))
    printf("Falha de segurança detectada!\n");

  // Exibir a blockchain completa
  printf("\nBlockchain Criada:\n");
  printBlockchain(cadeia);

  // printf("\nSimulando ataque ao primeiro bloco...\n");
  simularAtaque(cadeia, "Transacao Hackeada: Eve -> Alice");

  // Exibir a blockchain após o ataque
  printf("\nBlockchain Após o Ataque:\n");
  printBlockchain(cadeia);
  if (verificar_transacao(cadeia->ultimo_bloco->arvore->raiz,
                          "Transacao Hackeada: Eve -> Alice"))

    printf("Blockchain alterada com sucesso!\n");

  return 0;
}