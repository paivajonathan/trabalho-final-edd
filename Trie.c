#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_CARACTERES 256
#define TAMANHO_ALFABETO 26

typedef struct NoTrie {
	struct NoTrie *filhos[TAMANHO_ALFABETO];
	bool fim_palavra;
} NoTrie;

NoTrie *criar_no(void) {
	NoTrie *no = malloc(sizeof(NoTrie));

	if (no == NULL)
		exit(1);

	for (int i = 0; i < 26; i++)
		no->filhos[i] = NULL;

	no->fim_palavra = false;
	return no;
}

bool existe_palavra(NoTrie *raiz, const char *palavra) {
	NoTrie *no_atual = raiz;
	size_t tamanho_palavra = strlen(palavra);

	for (size_t i = 0; i < tamanho_palavra; i++) {
		// Calcula o indice da letra, número entre 0 - 25.
		int indice_palavra = palavra[i] - 'a';

		// Dado o nó atual, caso o indice dessa letra
		// não seja encontrado nos filhos, então não é possível encontrá-la.
		if (no_atual->filhos[indice_palavra] == NULL)
			return false;

		// Se a letra realmente exista (existe um nó nesse índice)
		// então o nó atual vai passar a ser esse nó
		no_atual = no_atual->filhos[indice_palavra];
	}

	// Percorridos os nós para todas as letras, ao chegar ao fim,
	// verifica-se se foi encontrada uma palavra inteira.
	return no_atual->fim_palavra;
}

void coletar_palavras(NoTrie *no_atual, char *prefixo_temporario, size_t tamanho) {
	// Caso tenha chegado em um nó marcado como fim de palavra,
	// exibe a palavra que foi coletada até ele
	if (no_atual->fim_palavra) {
		prefixo_temporario[tamanho] = '\0';
		printf("%s\n", prefixo_temporario);
	}

	for (int i = 0; i < TAMANHO_ALFABETO; i++) {
		// Caso não exista um nó nessa posição, ignora a posição
		if (no_atual->filhos[i] == NULL)
			continue;

		// Transforma a posição de volta na letra original,
		// para ser possível exibi-la
		prefixo_temporario[tamanho] = i + 'a';

		// Chama a função novamente, passando esse nó filho
		coletar_palavras(no_atual->filhos[i], prefixo_temporario, tamanho + 1);
	}
}

NoTrie *encontrar_no(NoTrie *raiz, const char *prefixo) {
	NoTrie *no_atual = raiz;
	size_t tamanho_prefixo = strlen(prefixo);

	for (size_t i = 0; i < tamanho_prefixo; i++) {
		int indice_letra = prefixo[i] - 'a';

		if (no_atual->filhos[indice_letra] == NULL)
			return NULL;

		no_atual = no_atual->filhos[indice_letra];
	}

	return no_atual;
}

void exibir_palavras_com_prefixo(NoTrie *raiz, const char *prefixo) {
	NoTrie *no_prefixo = encontrar_no(raiz, prefixo);

	if (no_prefixo == NULL) {
		printf("nao foram encontradas palavras.\n");
		return;
	}

	// Armazena o valor do prefixo em uma variável temporária
	char prefixo_temporario[MAX_CARACTERES];
	strcpy(prefixo_temporario, prefixo);

	// Guarda valor do tamanho do prefixo original,
	// para que o temporário seja povoado a partir desse tamanho,
	// com as possibilidades que encontrar
	size_t tamanho_prefixo = strlen(prefixo);

	coletar_palavras(no_prefixo, prefixo_temporario, tamanho_prefixo);
}

void exibir_todas_palavras(NoTrie *raiz) {
	printf("\nExibindo todas as palavras registradas:\n\n");
	
	char prefixo_temporario[MAX_CARACTERES];
	coletar_palavras(raiz, prefixo_temporario, 0);
}

void inserir_palavra(NoTrie *raiz, const char *palavra) {
	NoTrie *no_atual = raiz;
	size_t tamanho_palavra = strlen(palavra);

	if (tamanho_palavra > MAX_CARACTERES) {
		printf("Tamanho máximo para inserir novas palavras é de %d caracteres.\n", MAX_CARACTERES);
		return;
	}

	// Percorre para cada caractere da palavra
	for (size_t i = 0; i < tamanho_palavra; i++) {
		// Descobre o 'indice' que representa a letra atual da palavra
		int indice_palavra = palavra[i] - 'a';

		// Caso esse indice nos filhos do nó atual esteja vazio, criamos ele
		if (no_atual->filhos[indice_palavra] == NULL) {
			// Nó para representar cada letra da palavra
			no_atual->filhos[indice_palavra] = criar_no();
		}

		// O nó atual passa a ser esse índice recém-criado
		no_atual = no_atual->filhos[indice_palavra];
	}

	// Marca o nó atual como contendo uma palavra inteira,
	// do caminho da raíz até ele
	no_atual->fim_palavra = true;
}

bool possui_filhos(NoTrie *raiz) {
	for (int i = 0; i < TAMANHO_ALFABETO; i++) {
		if (raiz->filhos[i] != NULL)
			return true;
	}
	return false;
}

NoTrie *remover_palavra_interno(NoTrie *raiz, const char *chave, int nivel) {
	if (raiz == NULL)
		return NULL;

	if (nivel == strlen(chave)) {
		raiz->fim_palavra = false;
	} else {
		int indice = chave[nivel] - 'a';
		raiz->filhos[indice] = remover_palavra_interno(raiz->filhos[indice], chave, nivel + 1); 
	}

	if (raiz->fim_palavra || possui_filhos(raiz))
		return raiz;
	
	free(raiz);
	raiz = NULL;
	return raiz;
}

void remover_palavra(NoTrie **raiz, const char *chave) {
	*raiz = remover_palavra_interno(*raiz, chave, 0);
}

void destruir_trie(NoTrie *raiz) {
	if (raiz == NULL)
		return;

	for (int i = 0; i < TAMANHO_ALFABETO; i++) {
		if (raiz->filhos[i] == NULL)
			continue;
		destruir_trie(raiz->filhos[i]);
	}

	free(raiz);
	raiz = NULL;
}

void exibir_menu(void) {
	printf("\n======================================================================");
	printf("\nDigite um prefixo, para descobrir as suas palavras possiveis.\n\n");
	printf("Regras:\n- Digite apenas letras minusculas.\n- Para encerrar o programa digite 0.\n");
	printf("======================================================================\n\n");
}

bool entrada_valida(const char *str) {
	while (*str) {
		if (!islower((unsigned char)*str))
			return false;
		str++;
	}
	return true;
}

int main(void)
{
	NoTrie *raiz = criar_no();

	const char *palavras[] = {
		"abacaxi", "barata", "caderno", "dente", "elefante", "foco", "gato", "hoquei", "ilha", "jogo",
		"kiwi", "livro", "mapa", "nuvem", "ovo", "pedra", "queijo", "rato", "sapo", "telefone",
		"uva", "vela", "xaxim", "zebra", "amigo", "bola", "cao", "dado", "escada", "faca",
		"gelo", "horta", "jujuba", "kilo", "lua", "manteiga", "nado", "ovelha", "pato", "rosto",
		"sol", "teto", "vaca", "zipe", "amendoim", "biscoito", "cabelo", "escova", "foca", "lapis",
		"mar", "olho", "pipa", "quadro", "sopa", "tomate", "urso", "vento", "abajur", "coracao",
		"dolar", "estrela", "fruta", "jardim", "leao", "mochila", "nave", "quente", "rua", "tartaruga",
		"yeti", "bolo", "iate"
	};
	int tamanho = sizeof(palavras) / sizeof(palavras[0]);

	for (int i = 0; i < tamanho; i++)
		inserir_palavra(raiz, palavras[i]);

	exibir_todas_palavras(raiz);	
	exibir_menu();
	
	char entrada[MAX_CARACTERES];
	while (true)
	{
		printf("> ");
		scanf("%s", entrada);

		if (!entrada_valida(entrada)) {
			if (entrada[0] == '0') {
				printf("Encerrando programa...\n");
				break;
			}
			printf("Tente novamente...\n");
			continue;
		}

		exibir_palavras_com_prefixo(raiz, entrada);
		printf("\n");
	}

	destruir_trie(raiz);
}
