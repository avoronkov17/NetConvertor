# NetConvertor
Программа по перебросу udp сообщений регистратора в http сервер.

# Назначение
### Переброс udp сообщений регистратора в http сервер. Репозиторий регистратра: [https://github.com/avoronkov17/Logger]
### Имитация работы регистратора для последующей разработки.
### Отладка работы связки **Регистратор->Эта программа<->Сервер**

# Сборка и запуск
- В системе должен быть установлен фреймворк Qt5
- Войти в каталог с исходниками
- **qmake**
- **make**
- Выставить настройки в файле **settings.ini**. Параметры файла откомментированы.
- Выполнить **./NetConvertor**
