#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "merkletree.h"
#include "sha256.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DIFICULDADE 4

// Estrutura do bloco
typedef struct block {
  int num;   // Número do bloco atual
  int nonce; // Número para modificar o hash
  unsigned char previous_hash[SHA256_SIZE_BYTES]; // Hash do bloco anterior
  unsigned char
      hash_transacoes[SHA256_SIZE_BYTES]; // hash da merkle tree de transações
  unsigned char hash[SHA256_SIZE_BYTES];  // Hash do bloco atual
  time_t timestamp;                       // Tempo em que o bloco foi criado
  Merkletree *arvore;     // Ponteiro para a mekletree de transções
  struct block *proximo;  // Ponteiro para o próximo nó
  struct block *anterior; // Ponteiro para o nó anterior
} Block;

typedef struct blockchain {
  Block *ultimo_bloco;
  int numBlocks;
} Blockchain;

Blockchain *criaBlockchain();
Block *criarBloco(const char *transacao, Blockchain *blockchain);
void adicionaBloco(Blockchain *blockchain, Block *bloco); // Precisa do POW
void minerar(Block *block /*, int Dificuldade*/);
void printBlockchain(Blockchain *blockchain);
void simularAtaque(Blockchain *blockAtacada, const char *novaTransacao);
void adicionar_transacao(Block *bloco, const char *transacao);
void print_sha256_hash(const unsigned char *data, size_t length);
#endif