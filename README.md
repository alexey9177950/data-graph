# Курсовая работа, 3 курс

## Файлы

### /src/

* gen\_data\_graphs.cpp -- построение графов данных

* community\_detection.cpp -- поиск сообществ

* main\_cd, main\_gen\_dg -- файлы для тестирования gen\_data\_graphs.cpp и community\_detection.cpp

* utils.py -- коэфициенты ассортативности, построение графа данных и label propagation через подпрограмму на с++.

* quality\_functions.py -- функции качества

### /notebooks/

* amazon\_encode\_title.ipynb -- часть data\_to\_matrix\_amazon.ipynb

* community\_detection.ipynb -- тестирование написанного на С++ label propagation.

* data\_graphs.ipynb -- строит графы данных, ищет в них сообщества и визуализирует полученный результат.

* data\_to\_matrix\_(6dfb | amazon | twitter).ipynb -- построение матриц признаков по метаданным.

* datasets.ipynb -- коэффициенты ассортативности и графики ассортативности, размеры компонент

* modularity.ipynb -- алгоритм CNM для твиттера и 6dfb. (для амазона слишком долго работает).

* qf\_test.ipynb -- тестирование функций качества.

## TODO:


### Запустить программу на кластере

* Сделать PCA для word2vec.

* Сделать матрицы с текстовыми признаками.

* Построить графы данных.

* Построить много разбиений с помощью label propagation и одно с CNM.

## Время запуска

### 6dfb

* eps -- 57s

* knn (5) -- 297s
 
* rng -- 924s ~ 15m

* gabr -- 2324s ~ 45m

* inf -- 46s
