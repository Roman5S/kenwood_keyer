Kenwood keyer позволяет программно манипулировать CW на трансиверах, использующих CAT протокол Kenwood (нужны команды RX TX).
Необходимо только подключить CAT кабель и настроить программу.

Необходимо дополнительно установить пару виртуальных COM портов (с помощью com0com или другой программы).
Программа, осуществляющая CW манипуляцию, соединяется с помощью пары виртуальных портов с kenwood keyer, kenwood keyer
соединяется с радиостанцией. Пока CW не передается, все работает, как будто трансивер подключен к программе с CW.
Когда передается CW (меняется уровень RTS или DTR на виртуальном порте), kenwood keyer командует трансиверу перейти в режим приема/передачи.
На трансивере должен быть выбран режим CW.
Подробнее в readme-kenwood-keyer.
