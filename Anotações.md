# 🚀 Guia Rápido: Compilação e Execução de Exercícios OpenGL/C++ com CMake

> Um guia prático e visual para quem está começando, focado em como **adicionar, compilar, rodar e organizar seus arquivos .cpp** em projetos de Computação Gráfica com CMake + VS Code!

---

## 🗂️ Estrutura Típica do Projeto

/seu-projeto/
├── CMakeLists.txt
├── src/
│ ├── Exercicio1.cpp
│ ├── Exercicio2.cpp
│ └── Exercicio3.cpp
└── build/

yaml
Copy
Edit

---

## 💡 Fluxo Básico: Quando Rodar Cada Comando?

| Mudança feita                      | Comando(s) que precisa rodar                                 |
|-------------------------------------|-------------------------------------------------------------|
| Só mudou código `.cpp`              | `cmake --build . --config Debug`                            |
| Mudou `CMakeLists.txt`              | `cmake ..` → `cmake --build . --config Debug`               |
| Adicionou/Removeu/Renomeou `.cpp`   | Atualize `CMakeLists.txt`, depois comandos acima             |
| Quer rodar o executável             | `.\Debug\NomeDoExe.exe` (Windows) ou `./NomeDoExe` (Linux)  |

---

## 🛠️ Passo a Passo Detalhado

### 1️⃣ **Adicione seu arquivo .cpp em `src/`**

Exemplo:  
`src/Exercicio4.cpp`

---

### 2️⃣ **Edite o `CMakeLists.txt`**

Adicione (ou remova) uma linha para cada exercício:
```cmake
add_executable(Exercicio4 src/Exercicio4.cpp)
⚠️ Dica: Não use acentos, espaços ou símbolos nos nomes!

3️⃣ Entre na pasta de build (caso ainda não esteja)
sh
Copy
Edit
cd build
4️⃣ Se mudou o CMakeLists.txt, sempre rode:
sh
Copy
Edit
cmake ..
Esse comando prepara o projeto para reconhecer novos arquivos ou mudanças.

5️⃣ Compile o projeto
sh
Copy
Edit
cmake --build . --config Debug
O executável aparece em build/Debug/ (Windows/Visual Studio)

Em Linux/MinGW: geralmente em build/

6️⃣ Execute seu programa
sh
Copy
Edit
.\Debug\Exercicio4.exe     # (Windows/Visual Studio)
./Exercicio4               # (Linux/MinGW)
🏁 Exemplo Prático
plaintext
Copy
Edit
1. Criei Exercicio4.cpp em src/
2. Editei o CMakeLists.txt:
   add_executable(Exercicio4 src/Exercicio4.cpp)
3. No terminal:
   cd build
   cmake ..
   cmake --build . --config Debug
   .\Debug\Exercicio4.exe
📋 Erros Comuns e Soluções
Mensagem	Possível solução
Cannot find source file	Caminho errado no CMakeLists.txt ou arquivo não existe
Target name is reserved or invalid	Nome com acento, espaço ou símbolo: corrija o nome
CommandNotFoundException ao rodar .exe	Executável não existe ou o build falhou
Executável não aparece após o build	Esqueceu o cmake .. após editar o CMakeLists.txt?

🎯 Resumo Ultra-Rápido
Alterou só o código (.cpp)?
→ Só rode cmake --build . --config Debug

Alterou CMakeLists.txt, adicionou, renomeou ou removeu .cpp?
→ Rode cmake ..
→ Depois cmake --build . --config Debug

Para rodar:
→ .\Debug\NomeDoExe.exe (Windows/VS)
→ ./NomeDoExe (Linux/MinGW)

🌈 Dicas Rápidas
Sempre atualize o CMakeLists.txt ao adicionar/excluir .cpp

Use nomes SEMPRE sem acentos, espaços ou símbolos

Se mover arquivo de pasta, atualize o caminho no CMakeLists.txt

Se aparecer erro, leia a mensagem! Quase sempre ela indica o caminho errado ou arquivo faltando

