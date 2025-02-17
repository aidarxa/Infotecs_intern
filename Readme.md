# Infotecs задание на стажировку
Репозиторий содержащий тестовое задание на позицию стажера.
## Быстрый старт
У вас должна быть установлена операционная система Linux Ubuntu/Debian, система контроля версий git, Make - для сборки проекта и компилятор GCC с поддержкой C++ 17.

0. Установите требуемые компоненты 
```shell
sudo apt update
sudo apt upgrade
sudo apt install git build-essential
```
1. Клонируйте этот репозиторий 
```
git clone https://github.com/aidarxa/Infotecs_intern.git
```
2. Перейдите в каталог проекта
```
cd Infotecs_intern
```
3. Соберите бибилиотеку
```
make lib
```
4. Или соберите приложение
```
make app
```
5. Запустите приложение
```
./out/app ИМЯ_ВЫХОДНОГО_ФАЙЛА УРОВЕНЬ_ВАЖНОСТИ_ПО_УМОЛЧАНИЮ
```
> [!TIP]
> Приложение поддерживает следующие уровни важности:  
> ${\small\textsf{\color{#B2FBA5}INFO}}$  
> ${\small\textsf{\color{#FFC067}WARNING}}$  
> ${\small\textsf{\color{#fe6761}ERROR}}$
## Документация
Документацию к библиотеке можно получить по [этой ссылке](./docs/lib.md), а к приложениею по [этой ссылке](./docs/application.md).