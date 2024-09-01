#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#define N 5

typedef struct data {
    int dia;
    int mes;
    int ano;
} data;

typedef struct medicamentos {
    int codigoDeBarras;
    char nomeMedicamento[50];
    data dataMedicamento;
    int quantidade;
    float preco;
} medicamento;

typedef struct page {
    int qnt = 0; // Número de chaves na página
    bool folha = false; // Flag indicando se a página é folha
    int codigos[N - 1]; // Array de chaves
    struct page *pai; // Ponteiro para a página pai
    medicamento *medicamentos[N - 1]; // Array de ponteiros para medicamentos
    struct page *filhos[N]; // Ponteiros para as páginas filhas
    struct page *prox; // Ponteiro para a próxima página (usado em folhas)
} pagina;

// Função para criar uma nova página
pagina *criarpagina() {
    pagina *novapage = (pagina *)malloc(sizeof(pagina));
    novapage->qnt = 0;
    novapage->folha = true;
    novapage->pai = NULL;
    novapage->prox = NULL;
    
    for (int i = 0; i < N - 1; i++) {
        novapage->codigos[i] = 0;
        novapage->medicamentos[i] = NULL;
    }
    
    for (int i = 0; i < N; i++) {
        novapage->filhos[i] = NULL;
    }
    
    return novapage;
}

// Função para dividir a página à esquerda
pagina *paginaesquerda(pagina *paginapai){
    if (paginapai == NULL){
        return NULL;
    }
    int mid = paginapai->qnt / 2;
    pagina *novapage = criarpagina(); 

    novapage->qnt = mid;
    novapage->folha = paginapai->folha;
    for (int i = 0; i < mid; ++i) {
        novapage->codigos[i] = paginapai->codigos[i];
        novapage->medicamentos[i] = paginapai->medicamentos[i];
        novapage->filhos[i] = paginapai->filhos[i];
        if (novapage->filhos[i]) novapage->filhos[i]->pai = novapage;
    }
    novapage->filhos[mid] = paginapai->filhos[mid];
    if (novapage->filhos[mid]) novapage->filhos[mid]->pai = novapage;

    novapage->pai = paginapai->pai;

    for (int i = 0; i < paginapai->qnt - mid - 1; ++i) {
        paginapai->codigos[i] = paginapai->codigos[i + mid + 1];
        paginapai->medicamentos[i] = paginapai->medicamentos[i + mid + 1];
        paginapai->filhos[i] = paginapai->filhos[i + mid + 1];
        if (paginapai->filhos[i]) paginapai->filhos[i]->pai = paginapai;
    }

    paginapai->qnt = mid;

    if (paginapai->pai) {
        adicionafilho(paginapai->pai, novapage);
    }

    return novapage;
}

// Função para dividir a página à direita
pagina *paginadireita(pagina *paginapai){
    if (paginapai == NULL){
        return NULL;
    }

    int mid = paginapai->qnt / 2;
    pagina *novapage = criarpagina();

    // Definindo a quantidade de chaves na nova página
    novapage->qnt = paginapai->qnt - mid - 1;
    novapage->folha = paginapai->folha;

    // Movendo as chaves e medicamentos para a nova página
    for (int i = 0; i < novapage->qnt; ++i) {
        novapage->codigos[i] = paginapai->codigos[mid + 1 + i];
        novapage->medicamentos[i] = paginapai->medicamentos[mid + 1 + i];
        paginapai->medicamentos[mid + 1 + i] = NULL; // Opcional: limpar ponteiros antigos
    }

    // Movendo os filhos para a nova página, se não for folha
    if (!paginapai->folha) {
        for (int i = 0; i < novapage->qnt + 1; ++i) {
            novapage->filhos[i] = paginapai->filhos[mid + 1 + i];
            paginapai->filhos[mid + 1 + i] = NULL; // Opcional: limpar ponteiros antigos
            if (novapage->filhos[i]) {
                novapage->filhos[i]->pai = novapage;
            }
        }
    }

    // Atualizando a quantidade de chaves na página original
    paginapai->qnt = mid;

    // Ajustando o ponteiro para a próxima página
    novapage->prox = paginapai->prox;
    paginapai->prox = novapage;

    // Definindo o pai da nova página
    novapage->pai = paginapai->pai;

    // Inserindo a nova página no pai, se existir
    if (paginapai->pai) {
        adicionafilho(paginapai->pai, novapage);
    }

    return novapage;
}


void adicionafilho(pagina *pai, pagina *filho) {
     if (pai == NULL || filho == NULL) return;

    int i;
    for (i = 0; i < pai->qnt && filho->codigos[0] > pai->codigos[i]; ++i);

    for (int j = pai->qnt; j > i; --j) {
        pai->filhos[j + 1] = pai->filhos[j];
    }

    pai->filhos[i + 1] = filho;
    filho->pai = pai;
   
}
medicamento *criaMedicamento(int codigoDeBarras, const char *nome, int dia, int mes, int ano, int quantidade,
                             float preco) {
    medicamento *novoMedicamento = (medicamento *) malloc(sizeof(medicamento));

    novoMedicamento->codigoDeBarras = codigoDeBarras;
    strcpy(novoMedicamento->nomeMedicamento, nome);

    novoMedicamento->dataMedicamento.dia = dia;
    novoMedicamento->dataMedicamento.mes = mes;
    novoMedicamento->dataMedicamento.ano = ano;

    novoMedicamento->quantidade = quantidade;
    novoMedicamento->preco = preco;

    return novoMedicamento;
}
// Função para inserir um medicamento na árvore B+
void inserirMedicamento(pagina **raiz, medicamento *novoMedicamento) {
    pagina *atual = *raiz;

    // Busca pela folha onde o medicamento deve ser inserido
    while (!atual->folha) {
        int i = 0;
        while (i < atual->qnt && novoMedicamento->codigoDeBarras > atual->codigos[i]) {
            i++;
        }
        atual = atual->filhos[i];
    }

    // Verifica se o medicamento já existe e incrementa a quantidade
    for (int i = 0; i < atual->qnt; i++) {
        if (atual->codigos[i] == novoMedicamento->codigoDeBarras) {
            atual->medicamentos[i]->quantidade += novoMedicamento->quantidade;
            return;
        }
    }

    // Se a página folha tem espaço, insere o novo medicamento
    if (atual->qnt < N - 1) {
        int i = atual->qnt - 1;
        while (i >= 0 && novoMedicamento->codigoDeBarras < atual->codigos[i]) {
            atual->codigos[i + 1] = atual->codigos[i];
            atual->medicamentos[i + 1] = atual->medicamentos[i];
            i--;
        }
        atual->codigos[i + 1] = novoMedicamento->codigoDeBarras;
        atual->medicamentos[i + 1] = novoMedicamento;
        atual->qnt++;
    } else {
        // Se a página folha não tem espaço, divide a página
        pagina *esquerda = paginaesquerda(atual);
        pagina *direita = paginadireita(atual);
        pagina *novaRaiz = criarpagina();

        novaRaiz->folha = false;
        novaRaiz->qnt = 1;
        novaRaiz->codigos[0] = direita->codigos[0];
        novaRaiz->filhos[0] = esquerda;
        novaRaiz->filhos[1] = direita;

        esquerda->pai = novaRaiz;
        direita->pai = novaRaiz;

        *raiz = novaRaiz;

        // Insere o medicamento na página correta
        if (novoMedicamento->codigoDeBarras < direita->codigos[0]) {
            inserirMedicamento(&esquerda, novoMedicamento);
        } else {
            inserirMedicamento(&direita, novoMedicamento);
        }
    }
}
medicamento *buscarMedicamento(pagina *raiz, int codigoDeBarras) {
    pagina *atual = raiz;

    // Percorre a árvore até encontrar a folha que pode conter o código de barras
    while (!atual->folha) {
        int i = 0;
        while (i < atual->qnt && codigoDeBarras > atual->codigos[i]) {
            i++;
        }
        atual = atual->filhos[i];
    }

    // Procura o código de barras na página folha
    for (int i = 0; i < atual->qnt; i++) {
        if (atual->codigos[i] == codigoDeBarras) {
            return atual->medicamentos[i];
        }
    }

    // Se não encontrar, retorna NULL
    return NULL;
}
void imprimirTodosOsMedicamentosOrdenados(pagina *raiz) {
    pagina *folha = encontrarFolhaMaisAEsquerda(raiz);
    
    while (folha != NULL) {
        for (int i = 0; i < folha->qnt; i++) {
            medicamento *med = folha->medicamentos[i];
            printf("Código de Barras: %d\n", med->codigoDeBarras);
            printf("Nome: %s\n", med->nomeMedicamento);
            printf("Data: %02d/%02d/%04d\n", med->dataMedicamento.dia, med->dataMedicamento.mes, med->dataMedicamento.ano);
            printf("Quantidade: %d\n", med->quantidade);
            printf("Preço: %.2f\n", med->preco);
            printf("-------------------------\n");
        }
        folha = folha->prox; // Avança para a próxima folha
    }
}
int main() {
    pagina *raiz = criarpagina();
    int opcao;

    do {
        printf("\n--- Sistema de Gestão de Medicamentos ---\n");
        printf("1. Buscar Medicamento\n");
        printf("2. Inserir Medicamento\n");
        printf("3. Retirar Medicamento\n");
        printf("4. Ver todos os Medicamentos em ordem\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
                int codigoDeBarras;
                printf("Digite o código de barras do medicamento: ");
                scanf("%d", &codigoDeBarras);
                medicamento *med = buscarMedicamento(raiz, codigoDeBarras);
                if (med != NULL) {
                    printf("Medicamento encontrado:\n");
                    printf("Código de Barras: %d\n", med->codigoDeBarras);
                    printf("Nome: %s\n", med->nomeMedicamento);
                    printf("Data: %02d/%02d/%04d\n", med->dataMedicamento.dia, med->dataMedicamento.mes, med->dataMedicamento.ano);
                    printf("Quantidade: %d\n", med->quantidade);
                    printf("Preço: %.2f\n", med->preco);
                } else {
                    printf("Medicamento não encontrado.\n");
                }
                break;
            }
            case 2: {
                int codigoDeBarras, dia, mes, ano, quantidade;
                float preco;
                char nome[50];
                printf("Digite o código de barras: ");
                scanf("%d", &codigoDeBarras);
                printf("Digite o nome do medicamento: ");
                scanf("%s", nome);
                printf("Digite a data de validade (dia mes ano): ");
                scanf("%d %d %d", &dia, &mes, &ano);
                printf("Digite a quantidade: ");
                scanf("%d", &quantidade);
                printf("Digite o preço: ");
                scanf("%f", &preco);

                medicamento *novoMedicamento = criaMedicamento(codigoDeBarras, nome, dia, mes, ano, quantidade, preco);
                inserirMedicamento(&raiz, novoMedicamento);
                printf("Medicamento inserido com sucesso!\n");
                break;
            }
            case 3: {
                int codigoDeBarras, quantidade;
                printf("Digite o código de barras do medicamento: ");
                scanf("%d", &codigoDeBarras);
                printf("Digite a quantidade a ser retirada: ");
                scanf("%d", &quantidade);
                retirarMedicamento(raiz, codigoDeBarras, quantidade);
                break;
            }
            case 4: {
                imprimirTodosOsMedicamentosOrdenados(raiz);
                break;
            }
            case 5: {
                printf("Saindo...\n");
                break;
            }
            default: {
                printf("Opcao invalida. Tente novamente.\n");
                break;
            }
        }
    } while (opcao != 5);

    return 0;
}

//stoy cansando jeffe//
