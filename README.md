# Курсовая работа, 3 курс

## Файлы

### /src/

* gen\_data\_graphs.cpp -- построение графов данных

* community\_detection.cpp -- поиск сообществ в графах

* main\_cd.cpp, main\_gen\_dg.cpp -- ввод/вывод для gen\_data\_graphs.cpp и community\_detection.cpp соответственно

* parse\_data.py -- парсинг датасетов

* utils.py -- "обёртка" на языке Python3.

* quality\_functions.py -- функции качества для валидации результатов

### /notebooks/

* cd\_(6dfb | amazon | twitter).ipynb -- поиск сообществ в каждом из датасетов

* community\_detection.ipynb -- тестирование label propagation

* data\_graphs.ipynb -- тестирование графов данных

* data\_to\_matrix\_(6dfb | amazon | twitter).ipynb -- построение матриц признаков по метаданным

* amazon\_encode\_title.ipynb -- обработка заголовка для data\_to\_matrix\_amazon.ipynb

* datasets.ipynb -- коэффициенты ассортативности и графики ассортативности в датасетах, размеры компонент связности

* qf\_test.ipynb -- тестирование функций качества.
