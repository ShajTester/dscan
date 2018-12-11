# dscan
R-BD-E5R-V4 i2c devices scanner

### В программе использованы библиотеки:

- Разбор параметров командной строки [Taywee/args](https://github.com/Taywee/args)
- Модульные тесты [Catch2](https://github.com/catchorg/Catch2)
- Журнал [spdlog](https://github.com/gabime/spdlog)
- JSON [rapidjson](https://github.com/Tencent/rapidjson)

## Использование

Запуск без параметров:
* происходит поиск файла конфигурации `/etc/dscan/dscan.conf`. Если такого файла нет, то поиск файла `dscan.conf` в текущей директории. Если такого файла нет, то выход с ошибкой.
* Сканирование только известных узлов, описанных в конфигурации
* Вывод отчета в виде простого текста.

Запуск без параметров соответствует следующему набору ключей:
```
dscan -c /etc/dscan/dscan.conf --certain --text
```

Общие параметры:
* `-h    --help` - справка
* `-v    --version` - версия
* `-c <имя файла>    --conf <имя файла>` - файл конфигурации

Формат вывода:
* `--json` - форматированный json с отступами и переводом строк
* `--json1` - json в одну строку
* `--text` - простой текст

Способ сканирования:
* `--certain` - сканирование только известных узлов, описанных в массиве `devices` для каждой шины
* `--full` - сканирование для каждой шины диапазона адресов между `first` и `last`, и адресов устройств, описанных в `devices`

## Файл конфигурации

Файл конфигурации представляет собой json, состоящий из массива объектов, описывающих шины i2c, подлежащие сканированию.

Адреса устройств должны быть записаны как строки, содержащие 16-ричное число с префиксом `0x` без дополнительных пробелов. Например:
```
    "first": "0x03"
    "last": "0x77"
    "addr": "0x2b"
```

Для каждой шины используются следующие ключи:
* `bus` - номер шины. Десятичное число.
* `first` - первый адрес диапазона для сканирования
* `last` - последний адрес диапазона для сканирования
* `devices` - массив известных устройств на данной шине. Для каждого устройства используются следующие ключи:
    * `addr` - 7-битный адрес устройства (без бита чтения/записи)
    * `name` - краткое описание утсройства
    * `descr` - длинное описание устройства

После разбора конфигурации, из списка известных устройств удаляются устройства с повторяющимися адресами. О чем выводится предупреждение в стандартный вывод.


### Пример файла конфигурации

```json
[
    {
        "bus": 2,
        "first": "0x03",
        "last": "0x77"
    },
    {
        "bus": 3,
        "first": "0x03",
        "last": "0x77",
        "devices": [
            {
                "addr": "0x10",
                "descr": "test device"
            },
            {
                "addr": "0x52",
                "name": "fru1",
                "descr": "AT24C02"
            },
            {
                "addr": "0x68",
                "name": "clkgen"
            }
        ]
    },
    {
        "bus": 4,
        "first": "0x03",
        "last": "0x77"
    }
]
```

## Возвращаемое значение

### Plain text

В этом случае возвращается список найденных узлов по всем сканированным шинам с расшифровкой их состояния.

### json

Программа возвращает json, состоящий из массива объектов, с описанием найденных узлов. В массив включены узлы со всеми состояниями кроме 2 - свободные адреса, не описанные в конфигурации.

Каждый объект содержит следующие ключи:
* `bus` - номер шины
* `addr` - адрес устройства
* `name` - краткое описание из файла конфигурации
* `descr` - полное описание из файла конфигурации
* `state` - [код состояния](#%D0%BA%D0%BE%D0%B4%D1%8B-%D1%81%D0%BE%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%B8%D1%8F) устройства
* `state_descr` - описание состояния устройства

**Пример вывода в json**
```json
[
    {
        "bus": 3,
        "addr": "0x10",
        "name": "NoNameDevice",
        "descr": "test device",
        "state": 101,
        "state_descr": "Warning! Device don't answer"
    },
    {
        "bus": 3,
        "addr": "0x52",
        "name": "fru1",
        "descr": "AT24C02",
        "state": 103,
        "state_descr": "A driver is connected to the device"
    },
    {
        "bus": 3,
        "addr": "0x68",
        "name": "clkgen",
        "descr": "",
        "state": 103,
        "state_descr": "A driver is connected to the device"
    }
]
```

## Коды состояния

| Код состояния узла | Описание состояния |  |
|:------------------:|:-------------------|:-|
| 0   | Адрес не сканировалось |  |
| 1   | Нет в конфигурации, не ответил на запросы |  |
| 2   | Нет в конфигурации, ответил на запросы | Найдено новое устройство |
| 3   | Неизвестный драйвер подключен к неизвестному устройству | Новое устройство с подключенным драйвером |
| 4   | Ошибка открытия шины |  |
| 101 | Есть в конфигурации, не отвечает на запросы | Потеряно устройство |
| 102 | Есть в конфигурации, отвечает на запросы | Всё в порядке |
| 103 | Есть в конфигурации, подключен драйвер | Нужно проверять через драйвер |

