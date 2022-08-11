#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "encadeamento_exterior.h"
#include "lista_compartimentos.h"
#include "cliente.h"

int tamanho_do_arquivo(FILE *arquivo)
{
	fseek(arquivo, 0L, SEEK_END);
	return (ftell(arquivo) / tamanho_compartimento());
}

void cria_hash(char *nome_arquivo_hash, int tam)
{
	ListaCompartimentos *lc = (ListaCompartimentos *)malloc(sizeof(ListaCompartimentos));
	lc->qtd = tam;
	lc->lista = (CompartimentoHash **)malloc(sizeof(CompartimentoHash *) * (tam));
	int i;
	for (i = 0; i < tam; i++)
	{
		lc->lista[i] = compartimento_hash(-1);
	}
	salva_compartimentos(nome_arquivo_hash, lc);
}

int busca(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
	Cliente *cliente;

	FILE *in_hash = fopen(nome_arquivo_hash, "rb");
	FILE *in_dados = fopen(nome_arquivo_dados, "rb");
	ListaCompartimentos *lc_hash = (ListaCompartimentos *)malloc(sizeof(ListaCompartimentos));;
	ListaCompartimentos *lc_dados;
	lc_hash->qtd = tamanho_do_arquivo(in_hash);
	fseek(in_hash, tamanho_compartimento() * (cod_cli % lc_hash->qtd), SEEK_SET);
	int temp = le_compartimento(in_hash)->prox;
	fclose(in_hash);

	fseek(in_dados, temp * tamanho_cliente(), SEEK_SET);
	cliente = le_cliente(in_dados);
	
	if (cod_cli == cliente->cod_cliente && cliente->status == 1)
	{
		fclose(in_dados);
		return temp;
	}
	else if (cod_cli == cliente->cod_cliente && cliente->status == 0)
	{
		fclose(in_dados);
		return -1;
	}
	do
	{
		temp = cliente->prox;
		fseek(in_dados, temp * tamanho_cliente(), SEEK_SET);
		cliente = le_cliente(in_dados);
		if (cod_cli == cliente->cod_cliente && cliente->status == 1)
		{
			fclose(in_dados);
			return temp;
		}
		else if(cliente->cod_cliente == cod_cli && cliente->prox != -1 && cliente->status == 0){
			continue;
		}
		else if (cod_cli == cliente->cod_cliente && cliente->status == 0)
		{
			fclose(in_dados);
			return -1;  //liberado
		}
		else if (cliente->prox == -1 && cliente->cod_cliente != cod_cli)
		{
			fclose(in_dados);
			return -1;
		}
	} while (cliente->prox != -1);
	fclose(in_dados);

	return INT_MAX;
}

int insere(int cod_cli, char *nome_cli, char *nome_arquivo_hash, char *nome_arquivo_dados, int num_registros)
{
	FILE *arq_hash = fopen(nome_arquivo_hash,"rb+");
	if(arq_hash == NULL){
        printf("\n Erro ao tentar acessar o arquivo");
        return 0;
	}

	FILE *arq_dados = fopen(nome_arquivo_dados,"rb+");
	if(arq_dados == NULL){
        printf("\n Erro ao tentar acessar o arquivo");
        return 0;
	}

    fseek(arq_hash,(cod_cli%7)*tamanho_compartimento(),0);
    CompartimentoHash *comp = le_compartimento(arq_hash);

    //prox != -1 significa que o espaco na tabela ja esta preenchido
    if(comp->prox != -1){
        rewind(arq_dados);
        fseek(arq_dados,comp->prox*tamanho_cliente(),0);
        Cliente *client = le_cliente(arq_dados);

        /* chaves iguais o retorno e -1, pois o ponteiro esta "vazio" e nao  cadastra o cliente */
        if(client->cod_cliente == cod_cli){
            fclose(arq_hash);
            fclose(arq_dados);
            return -1;
        }

        int auxPont; // guarda a linha do cliente que ira apontar para o novo cliente
        /* roda por toda a lista encdeada de clientes */
        while(client->prox != -1 && client->status != LIBERADO){
            rewind(arq_dados);
            fseek(arq_dados,client->prox*tamanho_cliente(),0);
            auxPont = client->prox;
            client = le_cliente(arq_dados);
        }
        if(client->status != LIBERADO){
            /* o cliente aponta para o cliente que serja cadastrado */
            comp->prox = num_registros;
            client->prox = comp->prox;

            /* e necessario usar o rewind para retornar ao ponto 0 do arquivo, e depois o fseek
            para estar no ponto exato do arquivo */
            rewind(arq_dados);
            fseek(arq_dados,auxPont*tamanho_cliente(),0);
            salva_cliente(client,arq_dados);

            Cliente *novoClient = (Cliente *) malloc(sizeof(Cliente));
            novoClient->cod_cliente = cod_cli;
            strcpy(novoClient->nome,nome_cli);
            novoClient->prox = -1;
            novoClient->status = OCUPADO;

            /* Nesse caso o rewind nao se faz necessario, pois, o buffer ja esta
            posicionado na linha exata onde o cliente sera salvo */
            salva_cliente(novoClient,arq_dados);

        }else{
            /* o cliente que sera cadastrado recebe o ponteiro do cliente liberado */
            comp->prox = auxPont;


            /* cria o novo cliente */
            Cliente *novoClient = (Cliente *) malloc(sizeof(Cliente));
            novoClient->cod_cliente = cod_cli;
            strcpy(novoClient->nome,nome_cli);
            novoClient->prox = client->prox; // ponteiro do cliente liberado, mantendo a ordem da lista encadeada
            novoClient->status = OCUPADO;

            /* e necessario usar o rewind para retornar ao ponto 0 do arquivo, e depois o fseek
            para estar no ponto exato do arquivo */
            rewind(arq_dados);
            fseek(arq_dados,comp->prox*tamanho_cliente(),0);
            salva_cliente(novoClient,arq_dados);
        }

    }else{

        /* atribiu os dados do cliente a uma struct Cliente */
        Cliente *client = (Cliente *) malloc(sizeof(Cliente));
        client->cod_cliente = cod_cli;
        strcpy(client->nome,nome_cli);
        client->prox = -1;
        client->status = OCUPADO;

        comp->prox = num_registros;

        /* e necessario usar o rewind para retornar ao ponto 0 do arquivo, e depois o fseek
        para estar no ponto exato do arquivo */
        rewind(arq_dados);
        fseek(arq_dados,comp->prox*tamanho_cliente(),0);
        salva_cliente(client,arq_dados);

        //salva no arquivo de hash o ponteiro para o arquivo de dados
        rewind(arq_hash);
        fseek(arq_hash,(cod_cli%7)*tamanho_compartimento(),0);
        salva_compartimento(comp,arq_hash);

    }

    fclose(arq_hash);
    fclose(arq_dados);
    return comp->prox;
}

int exclui(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
	FILE *arq_hash = fopen(nome_arquivo_hash,"rb+");
	if(arq_hash == NULL){
        printf("\n Erro ao tentar acessar o arquivo");
        return 0;
	}

	FILE *arq_dados = fopen(nome_arquivo_dados,"rb+");
	if(arq_dados == NULL){
        printf("\n Erro ao tentar acessar o arquivo");
        return 0;
	}

    fseek(arq_hash,(cod_cli%7)*tamanho_compartimento(),0);
    CompartimentoHash *comp = le_compartimento(arq_hash);
    int pontExclu;

    //prox != -1 significa que o espaco na tabela ja esta preenchido
    if(comp->prox != -1){
        rewind(arq_dados);
        fseek(arq_dados,comp->prox*tamanho_cliente(),0);
        Cliente *client = le_cliente(arq_dados);

        int auxPont; // guarda a linha do cliente que ira apontar para o novo cliente
        /* roda por toda a lista encdeada de clientes */
        if(client->cod_cliente == cod_cli){
            pontExclu = comp->prox;
            client->status = LIBERADO;
            // atualiza no arquivo de dados o cliente especificado com o flag liberado
            rewind(arq_dados);
            fseek(arq_dados,comp->prox*tamanho_cliente(),0);
            salva_cliente(client,arq_dados);

            fclose(arq_hash);
            fclose(arq_dados);
            return pontExclu;
        }

        /* percorre toda a lista encadada, se ela existir */
        while(client->prox != -1){
            rewind(arq_dados);
            fseek(arq_dados,client->prox*tamanho_cliente(),0);
            auxPont = client->prox;
            client = le_cliente(arq_dados);
            if(client->cod_cliente == cod_cli)
                break;
        }
        /* compara se o cliente escolhido existe e possivel de ser excluido */
        if(client->status != LIBERADO && client->cod_cliente == cod_cli){
            pontExclu = auxPont;
            // atualiza no arquivo de dados o cliente especificado com o flag liberado
            client->status = LIBERADO;
            rewind(arq_dados);
            fseek(arq_dados,pontExclu*tamanho_cliente(),0);
            salva_cliente(client,arq_dados);

        }else{
            fclose(arq_hash);
            fclose(arq_dados);
            /* retorna -1 pois o cliente nao existe ou ja esta com o flag liberado*/
            return -1;
        }

    }else{
        fclose(arq_hash);
        fclose(arq_dados);
        /* retorna -1 pois o espaco ja esta vazio */
        return -1;
    }

    fclose(arq_hash);
    fclose(arq_dados);
    return pontExclu;
}
