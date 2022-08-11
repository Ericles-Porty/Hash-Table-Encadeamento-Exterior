#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#define BEFORE_TEST before();

#define NOME_ARQUIVO_HASH "tabHash.dat"
#define NOME_ARQUIVO_DADOS "clientes.dat"

#include <limits.h>
#include <stdlib.h>

#include "ufftest.h"
#include "lista_clientes.h"
#include "cliente.h"
#include "lista_compartimentos.h"
#include "compartimento_hash.h"
#include "encadeamento_exterior.h"


void before() {
	remove(NOME_ARQUIVO_HASH);
	remove(NOME_ARQUIVO_DADOS);
}

BEGIN_TESTS();

TEST("Cria Tabela Vazia. Testa o caso de tabela hash com compartimentos vazios");
if (!skip) {
	// Estamos usando tabela hash de tamanho 7
	// Adicionar 7 compartimentos que nao apontam para ninguem (prox = -1)
	ListaCompartimentos *tab_hash, *tab_hash_saida;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1));

	cria_hash(NOME_ARQUIVO_HASH, 7);

	tab_hash_saida = le_compartimentos(NOME_ARQUIVO_HASH);

	ASSERT_EQUAL_CMP(tab_hash, tab_hash_saida, cmp_compartimentos);
	libera_compartimentos(tab_hash);
	libera_compartimentos(tab_hash_saida);
}

TEST("Busca chave que eh encontrada na primeira tentativa");
if (!skip) {
	ListaCompartimentos *tab_hash;
	ListaClientes *tab_cliente;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(-1),
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(1,
		cliente(50, "Carlos", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = busca(50, NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS);

	ASSERT_EQUAL(end, 0);
	//endereco retornado deve ser igual a 0
	libera_compartimentos(tab_hash);
	libera_clientes(tab_cliente);
}

TEST("Busca chave que existia mas foi removida");
if (!skip) {
	ListaCompartimentos *tab_hash;
	ListaClientes *tab_cliente;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(-1),
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(1,
		cliente(50, "Carlos", -1, LIBERADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = busca(50, NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS);

	ASSERT_EQUAL(end, -1);
	//endereco retornado deve ser igual a -1
	libera_compartimentos(tab_hash);
	libera_clientes(tab_cliente);
}

TEST("Busca chave que eh encontrada na segunda tentativa");
if (!skip) {
	ListaCompartimentos *tab_hash;
	ListaClientes *tab_cliente;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(4),
		compartimento_hash(1),
		compartimento_hash(-1),
		compartimento_hash(2),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(6,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(59, "Maria", 3, OCUPADO),
		cliente(103, "Pedro", -1, OCUPADO),
		cliente(10, "Janio", 5, OCUPADO),
		cliente(51, "Ana", -1, OCUPADO),
		cliente(87, "Mauricio", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = busca(10, NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS);

	ASSERT_EQUAL(end, 3);
	//endereco retornado deve ser igual a 3
	libera_compartimentos(tab_hash);
	libera_clientes(tab_cliente);
}

TEST("Busca chave inexistente");
if (!skip) {
	ListaCompartimentos *tab_hash;
	ListaClientes *tab_cliente;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(4),
		compartimento_hash(1),
		compartimento_hash(-1),
		compartimento_hash(2),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(6,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(59, "Maria", 3, OCUPADO),
		cliente(103, "Pedro", -1, OCUPADO),
		cliente(3, "Janio", 5, OCUPADO),
		cliente(51, "Ana", -1, OCUPADO),
		cliente(87, "Mauricio", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = busca(10, NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS);

	ASSERT_EQUAL(end, -1);
	//endereco retornado deve ser igual a -1
	libera_compartimentos(tab_hash);
	libera_clientes(tab_cliente);
}

TEST("Busca chave que havia sido removida, mas foi reinserida mais adiante no arquivo");
if (!skip) {
	ListaCompartimentos *tab_hash;
	ListaClientes *tab_cliente;
	tab_hash = cria_compartimentos(7,
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(4),
		compartimento_hash(1),
		compartimento_hash(-1),
		compartimento_hash(2),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(7,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(59, "Maria", 3, OCUPADO),
		cliente(103, "Pedro", -1, OCUPADO),
		cliente(10, "Janio", 5, LIBERADO),
		cliente(51, "Ana", -1, OCUPADO),
		cliente(87, "Mauricio", 6, OCUPADO),
		cliente(10, "Janio S.", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = busca(10, NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS);

	ASSERT_EQUAL(end, 6);
	//endereco retornado deve ser igual a 6
	libera_compartimentos(tab_hash);
	libera_clientes(tab_cliente);
}

TEST("Insercao na 1 tentativa. Existe compartimento vazio na tabela para receber o registro");
if (!skip) {
	ListaCompartimentos *tab_hash, *tab_hash_saida;
	ListaClientes *tab_cliente, *tab_cliente_saida;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(1,
		cliente(49, "Joao", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = insere(50, "Mariana", NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS, 1);
	ASSERT_EQUAL(end, 1);


	tab_hash->lista[1]->prox = 1;
	tab_hash_saida = le_compartimentos(NOME_ARQUIVO_HASH);
	ASSERT_EQUAL_CMP(tab_hash, tab_hash_saida, cmp_compartimentos);

	libera_clientes(tab_cliente);
	tab_cliente = cria_clientes(2,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(50, "Mariana", -1, OCUPADO));
	tab_cliente_saida = le_clientes(NOME_ARQUIVO_DADOS);
	ASSERT_EQUAL_CMP(tab_cliente, tab_cliente_saida, cmp_clientes);

	libera_compartimentos(tab_hash);
	libera_compartimentos(tab_hash_saida);
	libera_clientes(tab_cliente);
	libera_clientes(tab_cliente_saida);
}

TEST("Insercao de registro com chave que ja existe");
if (!skip) {
	ListaCompartimentos *tab_hash, *tab_hash_saida;
	ListaClientes *tab_cliente, *tab_cliente_saida;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(1,
		cliente(49, "Joao", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = insere(49, "Jorge", NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS, 1);
	ASSERT_EQUAL(end, -1);


	tab_hash_saida = le_compartimentos(NOME_ARQUIVO_HASH);
	ASSERT_EQUAL_CMP(tab_hash, tab_hash_saida, cmp_compartimentos);

	tab_cliente_saida = le_clientes(NOME_ARQUIVO_DADOS);
	ASSERT_EQUAL_CMP(tab_cliente, tab_cliente_saida, cmp_clientes);

	libera_compartimentos(tab_hash);
	libera_compartimentos(tab_hash_saida);
	libera_clientes(tab_cliente);
	libera_clientes(tab_cliente_saida);
}

TEST("Insercao no final da lista encadeada");
if (!skip) {
	ListaCompartimentos *tab_hash, *tab_hash_saida;
	ListaClientes *tab_cliente, *tab_cliente_saida;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(4),
		compartimento_hash(1),
		compartimento_hash(-1),
		compartimento_hash(2),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(6,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(59, "Maria", 3, OCUPADO),
		cliente(103, "Pedro", -1, OCUPADO),
		cliente(3, "Janio", 5, OCUPADO),
		cliente(51, "Ana", -1, OCUPADO),
		cliente(87, "Mauricio", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = insere(10, "Ana", NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS, 6);
	ASSERT_EQUAL(end, 6);

	tab_hash_saida = le_compartimentos(NOME_ARQUIVO_HASH);
	ASSERT_EQUAL_CMP(tab_hash, tab_hash_saida, cmp_compartimentos);

	libera_clientes(tab_cliente);
	tab_cliente = cria_clientes(7,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(59, "Maria", 3, OCUPADO),
		cliente(103, "Pedro", -1, OCUPADO),
		cliente(3, "Janio", 5, OCUPADO),
		cliente(51, "Ana", -1, OCUPADO),
		cliente(87, "Mauricio", 6, OCUPADO),
		cliente(10, "Ana", -1, OCUPADO));
	tab_cliente_saida = le_clientes(NOME_ARQUIVO_DADOS);
	ASSERT_EQUAL_CMP(tab_cliente, tab_cliente_saida, cmp_clientes);

	libera_compartimentos(tab_hash);
	libera_compartimentos(tab_hash_saida);
	libera_clientes(tab_cliente);
	libera_clientes(tab_cliente_saida);
}

TEST("Insercao de registro no espaco vazio deixado por registro excluido");
if (!skip) {
	ListaCompartimentos *tab_hash, *tab_hash_saida;
	ListaClientes *tab_cliente, *tab_cliente_saida;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(4),
		compartimento_hash(1),
		compartimento_hash(-1),
		compartimento_hash(2),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(6,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(59, "Maria", 3, OCUPADO),
		cliente(103, "Pedro", -1, OCUPADO),
		cliente(3, "Janio", 5, LIBERADO),
		cliente(51, "Ana", -1, OCUPADO),
		cliente(87, "Mauricio", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = insere(10, "Ana", NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS, 6);
	ASSERT_EQUAL(end, 3);

	tab_hash_saida = le_compartimentos(NOME_ARQUIVO_HASH);
	ASSERT_EQUAL_CMP(tab_hash, tab_hash_saida, cmp_compartimentos);

	libera_clientes(tab_cliente);
	tab_cliente = cria_clientes(6,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(59, "Maria", 3, OCUPADO),
		cliente(103, "Pedro", -1, OCUPADO),
		cliente(10, "Ana", 5, OCUPADO),
		cliente(51, "Ana", -1, OCUPADO),
		cliente(87, "Mauricio", -1, OCUPADO));
	tab_cliente_saida = le_clientes(NOME_ARQUIVO_DADOS);
	ASSERT_EQUAL_CMP(tab_cliente, tab_cliente_saida, cmp_clientes);

	libera_compartimentos(tab_hash);
	libera_compartimentos(tab_hash_saida);
	libera_clientes(tab_cliente);
	libera_clientes(tab_cliente_saida);
}

TEST("Exclusao de chave inexistente");
if (!skip) {
	ListaCompartimentos *tab_hash, *tab_hash_saida;
	ListaClientes *tab_cliente, *tab_cliente_saida;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(4),
		compartimento_hash(1),
		compartimento_hash(-1),
		compartimento_hash(2),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(6,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(59, "Maria", 3, OCUPADO),
		cliente(103, "Pedro", -1, OCUPADO),
		cliente(3, "Janio", 5, LIBERADO),
		cliente(51, "Ana", -1, OCUPADO),
		cliente(87, "Mauricio", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = exclui(10, NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS);
	ASSERT_EQUAL(end, -1);

	tab_hash_saida = le_compartimentos(NOME_ARQUIVO_HASH);
	ASSERT_EQUAL_CMP(tab_hash, tab_hash_saida, cmp_compartimentos);

	tab_cliente_saida = le_clientes(NOME_ARQUIVO_DADOS);
	ASSERT_EQUAL_CMP(tab_cliente, tab_cliente_saida, cmp_clientes);

	libera_compartimentos(tab_hash);
	libera_compartimentos(tab_hash_saida);
	libera_clientes(tab_cliente);
	libera_clientes(tab_cliente_saida);
}

TEST("Exclusao do primeiro noh da lista encadeada de um determinado compartimento");
if (!skip) {
	ListaCompartimentos *tab_hash, *tab_hash_saida;
	ListaClientes *tab_cliente, *tab_cliente_saida;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(4),
		compartimento_hash(1),
		compartimento_hash(-1),
		compartimento_hash(2),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(6,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(59, "Maria", 3, OCUPADO),
		cliente(103, "Pedro", -1, OCUPADO),
		cliente(3, "Janio", 5, LIBERADO),
		cliente(51, "Ana", -1, OCUPADO),
		cliente(87, "Mauricio", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = exclui(59, NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS);
	ASSERT_EQUAL(end, 1);

	tab_hash_saida = le_compartimentos(NOME_ARQUIVO_HASH);
	ASSERT_EQUAL_CMP(tab_hash, tab_hash_saida, cmp_compartimentos);

	tab_cliente->lista[1]->status = LIBERADO;
	tab_cliente_saida = le_clientes(NOME_ARQUIVO_DADOS);
	ASSERT_EQUAL_CMP(tab_cliente, tab_cliente_saida, cmp_clientes);

	libera_compartimentos(tab_hash);
	libera_compartimentos(tab_hash_saida);
	libera_clientes(tab_cliente);
	libera_clientes(tab_cliente_saida);
}

TEST("Exclusao do ultimo noh da lista encadeada de um determinado compartimento");
if (!skip) {
	ListaCompartimentos *tab_hash, *tab_hash_saida;
	ListaClientes *tab_cliente, *tab_cliente_saida;

	tab_hash = cria_compartimentos(7,
		compartimento_hash(0),
		compartimento_hash(-1),
		compartimento_hash(4),
		compartimento_hash(1),
		compartimento_hash(-1),
		compartimento_hash(2),
		compartimento_hash(-1));
	salva_compartimentos(NOME_ARQUIVO_HASH, tab_hash);

	tab_cliente = cria_clientes(6,
		cliente(49, "Joao", -1, OCUPADO),
		cliente(59, "Maria", 3, OCUPADO),
		cliente(103, "Pedro", -1, OCUPADO),
		cliente(3, "Janio", 5, LIBERADO),
		cliente(51, "Ana", -1, OCUPADO),
		cliente(87, "Mauricio", -1, OCUPADO));
	salva_clientes(NOME_ARQUIVO_DADOS, tab_cliente);

	int end = exclui(87, NOME_ARQUIVO_HASH, NOME_ARQUIVO_DADOS);
	ASSERT_EQUAL(end, 5);

	tab_hash_saida = le_compartimentos(NOME_ARQUIVO_HASH);
	ASSERT_EQUAL_CMP(tab_hash, tab_hash_saida, cmp_compartimentos);

	tab_cliente->lista[5]->status = LIBERADO;
	tab_cliente_saida = le_clientes(NOME_ARQUIVO_DADOS);
	ASSERT_EQUAL_CMP(tab_cliente, tab_cliente_saida, cmp_clientes);

	libera_compartimentos(tab_hash);
	libera_compartimentos(tab_hash_saida);
	libera_clientes(tab_cliente);
	libera_clientes(tab_cliente_saida);
}

END_TESTS();