Está tudo no jeito para compilar e executar.

Pré requisitos:
Versão do GCC utilizado: 4.7.2 x86_32 (qualquer versão serve, mas deve ser 32 bits)
Versão da libCURL utilizada: 8.10.1 x86_32
Codificação utilizada no código: ISO 8859-1 (opcional)
Codificação utilizada nos docs: UTF-8 (opcional)


Para compilar, não é necessário a curl e o executável estarem na mesma pasta, muito menos na pasta do gcc, o que se recomenda é apenas referenciar tanto no arquivo .c quanto na linha de compilação as seguintes pastas:

- curl\bin
- curl\Include
- curl\lib

Não é necessário inserir nada nas variáveis de ambiente.

O arquivo dll deve estar no mesmo diretório do executável.

A linha de compilação consta no arquivo .c, no entanto, pode ser acessada na pasta "docs"


Bugs Encontrados:
- As vezes o servidor da CVM retorna 203 como sucesso em vez de 200, e também retorna 200 mesmo não carregando a pagina.
-- Isso será resolvido com uma filtração de código html, onde será decidido se será reciclado a url ou não.
-- Problemas: Salva um CNPJ como "não consta", sendo que consta mas está com outro código http.