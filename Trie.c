#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#ifdef _WIN32
  #include <windows.h>
  
  void timeout(unsigned int seconds) {
    Sleep(seconds * 1000);  // No Windows, Sleep é em milissegundos
  }
#else
  #include <unistd.h>
  
  void timeout(unsigned int seconds) {
    sleep(seconds);  // No POSIX, sleep é em segundos
  }
#endif

#define MAX_CARACTERES 256
#define TAMANHO_ALFABETO 26

typedef struct NoTrie {
	struct NoTrie *filhos[TAMANHO_ALFABETO];
	bool fim_palavra;
} NoTrie;

/* ==================== OPERAÇÕES BÁSICAS ==================== */

NoTrie *criar_no(void) {
	NoTrie *no = malloc(sizeof(NoTrie));

	if (no == NULL)
		exit(1);

	for (int i = 0; i < 26; i++)
		no->filhos[i] = NULL;

	no->fim_palavra = false;
	return no;
}

bool existe_palavra(NoTrie *raiz, const char *palavra, bool verboso) {
	NoTrie *no_atual = raiz;
	size_t tamanho_palavra = strlen(palavra);

	for (size_t i = 0; i < tamanho_palavra; i++) {
		// Calcula o indice da letra, número entre 0 - 25.
		int indice_palavra = palavra[i] - 'a';

		if (verboso)
			printf("Letra atual: %c\nIndice: %d\n", palavra[i], indice_palavra);

		// Dado o nó atual, caso o indice dessa letra
		// não seja encontrado nos filhos, então não é possível encontrá-la.
		if (no_atual->filhos[indice_palavra] == NULL) {
			if (verboso)
				printf("No nulo encontrado.\n\n");
			
			return false;
		}

		// Se a letra realmente exista (existe um nó nesse índice)
		// então o nó atual vai passar a ser esse nó
		no_atual = no_atual->filhos[indice_palavra];

		if (verboso) {
			printf("\n");
			timeout(1);
		}
	}

	// Percorridos os nós para todas as letras, ao chegar ao fim,
	// verifica-se se foi encontrada uma palavra inteira.
	return no_atual->fim_palavra;
}

void inserir_palavra(NoTrie *raiz, const char *palavra, bool verboso) {
	NoTrie *no_atual = raiz;
	size_t tamanho_palavra = strlen(palavra);

	if (tamanho_palavra > MAX_CARACTERES - 1) {
		printf("Tamanho máximo para inserir novas palavras é de %d caracteres.\n", MAX_CARACTERES - 1);
		return;
	}

	// Percorre para cada caractere da palavra
	for (size_t i = 0; i < tamanho_palavra; i++) {
		// Descobre o 'indice' que representa a letra atual da palavra
		int indice_palavra = palavra[i] - 'a';

		if (verboso)
			printf("Letra atual: %c\nIndice: %d\n", palavra[i], indice_palavra);

		// Caso esse indice nos filhos do nó atual esteja vazio, criamos ele
		if (no_atual->filhos[indice_palavra] == NULL) {
			if (verboso)
				printf("No nulo encontrado, alocando memoria...\n");

			// Nó para representar cada letra da palavra
			no_atual->filhos[indice_palavra] = criar_no();
		}

		// O nó atual passa a ser esse índice recém-criado
		no_atual = no_atual->filhos[indice_palavra];

		if (verboso) {
			printf("\n");
			timeout(1);
		}
	}

	// Marca o nó atual como contendo uma palavra inteira,
	// do caminho da raíz até ele
	no_atual->fim_palavra = true;
}

NoTrie *criar_trie(void) {
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

	int qtd_palavras = sizeof(palavras) / sizeof(palavras[0]);

	for (int i = 0; i < qtd_palavras; i++)
		inserir_palavra(raiz, palavras[i], false);

	return raiz;
}

bool possui_filhos(NoTrie *raiz) {
	for (int i = 0; i < TAMANHO_ALFABETO; i++) {
		if (raiz->filhos[i] != NULL)
			return true;
	}
	return false;
}

NoTrie *remover_palavra_interno(NoTrie *raiz, const char *chave, size_t nivel, bool verboso) {
	char letra_atual = nivel ? chave[nivel - 1] : ' ';
	
	if (verboso)
		printf("Nivel: %zu, Letra: %c\n", nivel, letra_atual);

	if (raiz == NULL) {
		if (verboso) {
			printf("No nulo, chave nao esta presente na trie.\n\n");
			timeout(1);
		}

		return NULL;
	}

	if (nivel == strlen(chave)) {
		if (verboso) {
			printf("No correspondente ao fim da chave encontrado, removendo o seu valor.\n\n");
			timeout(1);
		}
		
		raiz->fim_palavra = false;
	} else {
		int indice = chave[nivel] - 'a';
		
		if (verboso) {
			printf("Fim de chave nao encontrado.\n");
			printf("Proxima letra: %c, Indice: %d\n\n", chave[nivel], indice);
			timeout(1);
		}

		raiz->filhos[indice] = remover_palavra_interno(raiz->filhos[indice], chave, nivel + 1, verboso); 
	}

	if (verboso)
		printf("Nivel: %zu, Letra: %c\n", nivel, letra_atual);

	if (raiz->fim_palavra) {
		if (verboso) {
			printf("No esta marcado como fim de outra chave, mantenha.\n\n");
			timeout(1);
		}
		
		return raiz;
	}

	if (possui_filhos(raiz)) {
		if (verboso) {
			printf("No possui ligacoes com outros nos, mantenha.\n\n");
			timeout(1);
		}
		
		return raiz;
	}
	
	if (verboso) {
		printf("Removendo no da trie.\n\n");
		timeout(1);
	}

	free(raiz);
	raiz = NULL;
	return raiz;
}

void remover_palavra(NoTrie **raiz, const char *chave, bool verboso) {
	*raiz = remover_palavra_interno(*raiz, chave, 0, verboso);
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

/* ==================== OPERAÇÕES BÁSICAS ==================== */

/* ==================== APLICAÇÕES REAIS ==================== */

void coletar_palavras(NoTrie *no_atual, char *prefixo_temporario, size_t nivel, bool verboso) {
	bool nivel_exibido = false;
	char letra_atual = ' ';

	if (verboso) {
		printf("Nivel: %zu\n", nivel);
		
		if (no_atual->fim_palavra)
			printf("Palavra encontrada!\n");
		
		prefixo_temporario[nivel] = '\0';
		printf("Palavra atual: %s\n\n", prefixo_temporario);
		
		timeout(1);
	} else {
		// Caso tenha chegado em um nó marcado como fim de palavra,
		// exibe a palavra que foi coletada até ele
		if (no_atual->fim_palavra) {
			prefixo_temporario[nivel] = '\0';
			printf("%s\n", prefixo_temporario);
		}
	}

	for (int i = 0; i < TAMANHO_ALFABETO; i++) {
		// Transforma a posição de volta na letra original,
		// para ser possível exibi-la
		letra_atual = i + 'a';

		if (verboso) {
			if (!nivel_exibido) {
				printf("Nivel: %zu\n", nivel);
				printf("Buscando proxima letra...\n");
				nivel_exibido = true;
			}

			printf("%c ", letra_atual);
		}
		
		// Caso não exista um nó nessa posição, ignora a posição
		if (no_atual->filhos[i] == NULL) {
			if (verboso && i == TAMANHO_ALFABETO - 1) {
				printf("\n\n");
				nivel_exibido = false;
				timeout(1);
			}

			continue;
		}

		if (verboso) {
			printf("<--- Encontrada!\n\n");
			nivel_exibido = false;
			timeout(1);
		}

		// Adiciona a letra atual no fim da string
		prefixo_temporario[nivel] = letra_atual;

		// Chama a função novamente, passando esse nó filho
		coletar_palavras(no_atual->filhos[i], prefixo_temporario, nivel + 1, verboso);
	}
}

NoTrie *encontrar_no(NoTrie *raiz, const char *prefixo, bool verboso) {
	NoTrie *no_atual = raiz;
	size_t tamanho_prefixo = strlen(prefixo);

	for (size_t i = 0; i < tamanho_prefixo; i++) {
		int indice_letra = prefixo[i] - 'a';

		if (verboso)
			printf("Letra atual: %c, Indice: %d\n", prefixo[i], indice_letra);

		if (no_atual->filhos[indice_letra] == NULL) {
			if (verboso) {
				printf("No nulo encontrado.\nPrefixo nao existe na trie\n\n");
				timeout(1);
			}

			return NULL;
		}

		no_atual = no_atual->filhos[indice_letra];

		if (verboso) {
			printf("\n");
			timeout(1);
		}
	}

	if (verboso) {
		printf("Prefixo existe na Trie.\n\n");
		timeout(1);
	}

	return no_atual;
}

void exibir_palavras_com_prefixo(NoTrie *raiz, const char *prefixo, bool verboso) {
	NoTrie *no_prefixo = encontrar_no(raiz, prefixo, verboso);

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

	coletar_palavras(no_prefixo, prefixo_temporario, tamanho_prefixo, verboso);
}

void exibir_todas_palavras(NoTrie *raiz, bool verboso) {	
	char prefixo_temporario[MAX_CARACTERES];
	coletar_palavras(raiz, prefixo_temporario, 0, verboso);
}

size_t obter_tamanho_maior_prefixo(NoTrie *raiz, const char *palavra, size_t nivel, size_t tamanho, bool verboso) {
	char letra_atual = nivel ? palavra[nivel - 1] : ' ';

	if (verboso)
		printf("Nivel: %zu, Letra atual: %c\n", nivel, letra_atual);

	if (raiz == NULL) {
		if (verboso) {
			printf("No nulo encontrado.\n\n");
			timeout(1);
		}

		return tamanho;
	}
	
	if (raiz->fim_palavra) {
		tamanho = nivel;
		
		if (verboso)
			printf("Fim da palavra encontrado, tamanho atual: %zu\n", tamanho);
	}

	if (nivel == strlen(palavra)) {
		if (verboso) {
			printf("Palavra percorrida por completo.\n\n");
			timeout(1);
		}

		return tamanho;
	}
	
	int indice = palavra[nivel] - 'a';

	if (verboso) {
		printf("Proxima letra: %c, Indice: %d\n\n", palavra[nivel], indice);
		timeout(1);
	}
	
	return obter_tamanho_maior_prefixo(raiz->filhos[indice], palavra, nivel + 1, tamanho, verboso);
}

void exibir_maior_prefixo(NoTrie *raiz, const char *palavra, bool verboso) {
	size_t tamanho_maior_prefixo = obter_tamanho_maior_prefixo(raiz, palavra, 0, 0, verboso);
	
	char *maior_prefixo = (char *) malloc(sizeof(char) * (tamanho_maior_prefixo + 1));
	
	if (maior_prefixo == NULL) {
		printf("Ocorreu um erro ao alocar memoria para o maior prefixo.\n");
		exit(1);
	}
	
	strncpy(maior_prefixo, palavra, tamanho_maior_prefixo);
	maior_prefixo[tamanho_maior_prefixo] = '\0';

	printf("%s\n", maior_prefixo[0] == '\0' ? "Nao ha prefixo associado" : maior_prefixo);

	free(maior_prefixo);
	maior_prefixo = NULL;
}

/* ==================== APLICAÇÕES REAIS ==================== */

/* ==================== UTILITÁRIOS ==================== */

bool entrada_valida(const char *str) {
	while (*str) {
		if (!islower((unsigned char)*str))
			return false;
		str++;
	}
	return true;
}

void limpar_buffer(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

void aguardar_usuario(void) {
	printf("Pressione Enter para continuar...\n");
	getchar();
}

/* ==================== UTILITÁRIOS ==================== */

/* ==================== INTERFACES ==================== */
void exibir_menu(void) {
	printf("======================================================================\n");
	printf("Opcoes:\n\n");

	printf("- 1: Exibir todas as palavras\n");
	printf("- 2: Exibir todas as palavras com um certo prefixo\n");
	printf("- 3: Exibir maior prefixo de uma palavra que esta registrado como chave na Trie\n");
	printf("- 4: Verificar se uma palavra existe no sistema\n");
	printf("- 5: Inserir uma nova palavra no sistema\n");
	printf("- 6: Remover uma palavra do sistema\n");
	printf("- 7: Alterar modo verboso\n");
	printf("- 0: Encerrar o sistema ou encerrar uma operacao\n\n");
	
	printf("Sao permitidas apenas palavras:\n");
	printf("- Minusculas.\n");
	printf("- Sem acentos.\n");
	printf("- Sem espacos.\n");
	printf("======================================================================\n");
}

void exibir_todas_palavras_interface(NoTrie *raiz, bool verboso) {
	printf("\nExibindo todas as palavras cadastradas:\n\n");
	exibir_todas_palavras(raiz, verboso);
	aguardar_usuario();
}

void exibir_palavras_com_prefixo_interface(NoTrie *raiz, bool verboso) {
	char entrada[MAX_CARACTERES];
	
	printf("\nDigite um prefixo, para descobrir as suas palavras possiveis.\n");	
	
	while (true) {
		printf("> ");

		scanf(" %255[^\n]s", entrada);
		limpar_buffer();

		if (!entrada_valida(entrada)) {
			if (entrada[0] == '0') {
				printf("Retornando ao menu...\n");
				aguardar_usuario();
				return;
			}

			printf("Tente novamente...\nDigite uma palavra minuscula sem acentos ou espacos.\n\n");
			continue;
		}

		exibir_palavras_com_prefixo(raiz, entrada, verboso);
		printf("\n");
	}
}

void exibir_maior_prefixo_interface(NoTrie *raiz, bool verboso) {
	char entrada[MAX_CARACTERES];

	printf("\nDigite uma palavra, para descobrir o seu maior prefixo que e uma chave na Trie.\n");

	do {
		printf("> ");
		
		scanf(" %255[^\n]s", entrada);
		limpar_buffer();

		if (entrada_valida(entrada))
			break;
	
		if (entrada[0] == '0') {
			printf("\nRetornando ao menu...\n");
			aguardar_usuario();
			return;
		}

		printf("\nTente novamente...\nDigite uma palavra minuscula sem acentos ou espacos.\n");
	} while (true);

	exibir_maior_prefixo(raiz, entrada, verboso);
	aguardar_usuario();
}

void existe_palavra_interface(NoTrie *raiz, bool verboso) {
	char entrada[MAX_CARACTERES];
	
	printf("\nInsira uma palavra para verificar se existe no dicionario:\n");

	do {
		printf("> ");
		
		scanf(" %255[^\n]s", entrada);
		limpar_buffer();

		if (entrada_valida(entrada))
			break;
	
		if (entrada[0] == '0') {
			printf("\nRetornando ao menu...\n");
			aguardar_usuario();
			return;
		}

		printf("\nTente novamente...\nDigite uma palavra minuscula sem acentos ou espacos.\n");
	} while (true);

	bool existe = existe_palavra(raiz, entrada, verboso);
	
	printf("A palavra %s no dicionario\n", existe ? "esta" : "nao existe");
	aguardar_usuario();
}

void inserir_palavra_interface(NoTrie *raiz, bool verboso) {
	char entrada[MAX_CARACTERES];
	
	printf("\nInsira uma palavra que voce deseja no dicionario:\n");

	do {
		printf("> ");
		
		scanf(" %255[^\n]s", entrada);
		limpar_buffer();

		if (entrada_valida(entrada))
			break;
	
		if (entrada[0] == '0') {
			printf("\nRetornando ao menu...\n");
			aguardar_usuario();
			return;
		}

		printf("\nTente novamente...\nDigite uma palavra minuscula sem acentos ou espacos.\n");
	} while (true);

	inserir_palavra(raiz, entrada, verboso);
	
	printf("Palavra cadastrada com sucesso.\n");
	aguardar_usuario();
}

void remover_palavra_interface(NoTrie **raiz, bool verboso) {
	char entrada[MAX_CARACTERES];
	
	printf("\nInsira uma palavra para remove-la do dicionario, caso exista:\n");

	do {
		printf("> ");
		
		scanf(" %255[^\n]s", entrada);
		limpar_buffer();

		if (entrada_valida(entrada))
			break;
	
		if (entrada[0] == '0') {
			printf("\nRetornando ao menu...\n");
			aguardar_usuario();
			return;
		}

		printf("\nTente novamente...\nDigite uma palavra minuscula sem acentos ou espacos.\n");
	} while (true);

	bool existe = existe_palavra(*raiz, entrada, false);

	if (!existe) {
		printf("Palavra nao esta no dicionario, logo nao e possivel remove-la.\n");
		aguardar_usuario();
		return;
	}

	remover_palavra(raiz, entrada, verboso);

	printf("Palavra removida com sucesso.\n");
	aguardar_usuario();
}

void alterar_modo_verboso_interface(bool *verboso) {
	printf("Alterando modo verboso...\n");
	timeout(1);
	*verboso = !(*verboso);
	printf("Modo verboso esta %s.\n", (*verboso) ? "ativado" : "desativado");
	aguardar_usuario();
}

/* ==================== INTERFACES ==================== */

int main(void)
{
	NoTrie *raiz = criar_trie();
	int opcao = -1;
	bool verboso = false;

	while (true) {
		opcao = -1;
		exibir_menu();
		
		printf("> ");
		scanf("%d", &opcao);
		limpar_buffer();

		if (opcao == 0) {
			printf("Encerrando programa...\n");
			break;
		}

		switch (opcao) {
			case 1:
				exibir_todas_palavras_interface(raiz, verboso);
				break;
			case 2:
				exibir_palavras_com_prefixo_interface(raiz, verboso);
				break;
			case 3:
				exibir_maior_prefixo_interface(raiz, verboso);
				break;
			case 4:
				existe_palavra_interface(raiz, verboso);
				break;
			case 5:
				inserir_palavra_interface(raiz, verboso);
				break;
			case 6:
				remover_palavra_interface(&raiz, verboso);
				break;
			case 7:
				alterar_modo_verboso_interface(&verboso);
				break;
			default:
				printf("Digite uma opcao valida...\n");
				aguardar_usuario();
				break;
		}
	}

	destruir_trie(raiz);
}
