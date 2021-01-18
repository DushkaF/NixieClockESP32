# NixieClockESP32

## Репозиторий для часов на ESP32

1. Установить библиотеки 
    1. https://github.com/me-no-dev/AsyncTCP
    2. https://github.com/me-no-dev/ESPAsyncWebServer
    3. https://arduinojson.org/ (по идее есть в встроенном менеджере библиотек)
2. Следуя инструкции установи дополнение к Arduino IDE для загрузки файлов в файловую систему: https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/  
Хочу отметить, что структура папок проекта уже соответствует необходимой для правильной загрузки в память ESP32
3. Открываешь *GIT Bush Here* в папке, где где имеется данный **readme** и пишешь команду `git pull` и у тебя произойдёт обновление.
4. Открываешь *NixieESP32.ino* устанавливаешь плату. Я использовал дефолтные настройки. В той же вкладке, где есть выбор плат есть строка "ESP32 Sketch Data Upload", нажимаешь, загружаются файлы. После этого можешь загружать код.
5. Готово