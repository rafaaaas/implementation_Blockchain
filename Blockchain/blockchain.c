#include "blockchain.h"

Blockchain *criaBlockchain() {
  Blockchain *blockchain = malloc(sizeof(Blockchain));
  if (!blockchain) {
    printf("Erro ao alocar memória para a Blockchain!");
    return NULL;
  }

  blockchain->numBlocks = 0;
  blockchain->ultimo_bloco = NULL;
  return blockchain;
}

void calcularHash(Block *bloco) {
  char input[1024]; // Buffer para armazenar os dados do bloco formatados

  char previous_hash_hex[SHA256_SIZE_BYTES * 2 + 1];
  char hash_transacoes_hex[SHA256_SIZE_BYTES * 2 + 1];

  for (int i = 0; i < SHA256_SIZE_BYTES; i++) {
    sprintf(previous_hash_hex + i * 2, "%02x", bloco->previous_hash[i]);
    sprintf(hash_transacoes_hex + i * 2, "%02x", bloco->hash_transacoes[i]);
  }

  snprintf(input, sizeof(input), "%d%s%s%d%ld", bloco->num, previous_hash_hex,
           hash_transacoes_hex, bloco->nonce, bloco->timestamp);

  sha256(input, strlen(input), bloco->hash);
}

Block *criarBloco(const char *transacao, Blockchain *blockchain) {
  Block *novo = (Block *)malloc(sizeof(Block));
  if (!novo) {
    printf("Erro ao alocar memória para o bloco!");
    return NULL;
  }

  novo->arvore = criar_Merkle_Tree();
  novo->num = blockchain->numBlocks++;
  novo->nonce = 0;
  novo->anterior = blockchain->ultimo_bloco;
  if (novo->anterior) {
    memcpy(novo->previous_hash, novo->anterior->hash, SHA256_SIZE_BYTES);
  } else {
    memset(novo->previous_hash, 0, SHA256_SIZE_BYTES); // Bloco Gênese
  }
  adicionar_transacao(novo, transacao);
  novo->anterior = blockchain->ultimo_bloco;
  if (novo->anterior != NULL)
    novo->anterior->proximo = novo;

  calcularHash(novo);

  blockchain->ultimo_bloco = novo;
  minerar(novo);
  novo->timestamp = time(NULL);

  return novo;
}

void minerar(Block *bloco) {
  char hashHex[SHA256_SIZE_BYTES * 2 +
               1]; // Armazena a hash convertida para string
  char zeros[DIFICULDADE + 1];

  memset(zeros, '0', DIFICULDADE);
  zeros[DIFICULDADE] = '\0';

  do {
    bloco->nonce++;
    calcularHash(bloco);

    // Converte o hash para string hexadecimal
    for (int i = 0; i < SHA256_SIZE_BYTES; i++) {
      sprintf(hashHex + i * 2, "%02x", bloco->hash[i]);
    }
    hashHex[SHA256_SIZE_BYTES * 2] = '\0';

  } while (strncmp(hashHex, zeros, DIFICULDADE) != 0);
}

void simularAtaque(Blockchain *blockchain, const char *novaTransacao) {
  if (!blockchain || !blockchain->ultimo_bloco) {
    printf("Blockchain inválida ou vazia!\n");
    return;
  }

  size_t tamTransacao = strlen(novaTransacao);
  if (tamTransacao == 0) {
    printf("Erro: Transação inválida!\n");
    return;
  }

  // Encontrar o bloco inicial
  Block *blocoInicial = blockchain->ultimo_bloco;
  while (blocoInicial->anterior != NULL) {
    blocoInicial = blocoInicial->anterior;
  }

  // adiciona uma nova transação no bloco inicial
  No *ultima_transacao = ultimo_no(blocoInicial->arvore);

  // Calcula o hash da nova transação
  unsigned char novo_hash[SHA256_SIZE_BYTES];
  sha256(novaTransacao, tamTransacao, novo_hash);

  memcpy(ultima_transacao->hash, novo_hash, SHA256_SIZE_BYTES);

  atualizar_caminho(ultima_transacao);

  if (ultima_transacao == blocoInicial->arvore->raiz)
    memcpy(blocoInicial->hash_transacoes, ultima_transacao->hash,
           SHA256_SIZE_BYTES);

  // Iniciar a medição do tempo
  clock_t inicio = clock();

  // Recalcular hash e nonce do bloco inicial
  blocoInicial->nonce = 0;
  minerar(blocoInicial);

  // Reajustar os nonces e hashes de todos os blocos subsequentes
  Block *blocoAtual = blocoInicial->proximo;
  while (blocoAtual) {
    // Atualizar o campo previous_hash do bloco atual
    memcpy(blocoAtual->previous_hash, blocoAtual->anterior->hash,
           SHA256_SIZE_BYTES);

    // Resetar nonce e minerar novamente
    blocoAtual->nonce = 0;
    minerar(blocoAtual);

    // Avançar para o próximo bloco
    blocoAtual = blocoAtual->proximo;
  }

  // Finalizar a medição do tempo
  clock_t fim = clock();
  double tempoGasto = (double)(fim - inicio) / CLOCKS_PER_SEC;

  printf("Ataque ao bloco inicial simulado com sucesso!\n");
  printf("Tempo gasto para reajustar os nonces: %.2f segundos\n", tempoGasto);
}

void printBloco(Block *bloco) {
  if (!bloco) {
    printf("Bloco inválido!\n");
    return;
  }

  printf("Bloco #%d\n", bloco->num);
  printf("Data e Hora: %s\n", ctime(&bloco->timestamp));
  printf("Nonce: %d\n", bloco->nonce);
  // printf("Hash da Raiz: \t");
  // print_sha256_hash(bloco->hash_transacoes, SHA256_SIZE_BYTES);
  printf("Hash Anterior: \t");
  print_sha256_hash(bloco->previous_hash, SHA256_SIZE_BYTES);
  printf("Hash Atual: \t");
  print_sha256_hash(bloco->hash, SHA256_SIZE_BYTES);
  printf("--------------------------\n");
}

void printBlockchain(Blockchain *blockchain) {
  if (!blockchain || !blockchain->ultimo_bloco) {
    printf("Blockchain vazia!\n");
    return;
  }
  printf("Dificuldade: %d\n\n", DIFICULDADE);
  Block *atual = blockchain->ultimo_bloco;
  while (atual) {
    printBloco(atual);
    atual = atual->anterior;
  }
}

void adicionar_transacao(Block *bloco, const char *transacao) {
  if (transacao == NULL || bloco == NULL || bloco->arvore == NULL) {
    return;
  }
  adicionar_folha(bloco->arvore, transacao);
  calcularHash(bloco);
  return;
}

void print_sha256_hash(const unsigned char *data, size_t length) {
  for (int i = 0; i < SHA256_SIZE_BYTES; i++) {
    printf("%02x", data[i]);
  }
  printf("\n");
}