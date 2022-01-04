# SuperKeeper
## Introduction
Existing approaches for super spread detection cannot achieve the requirement of accuracy, especially in small memory.
Meanwhile, they cann't provide the estimation of cardinality precisely which is important in practice.

SuperKeeper is a novel Sketh for super spread detection which inspired by the positive feedback of Cybernetic. 
It adopt a power-weakening increment strategy for indentify super spread accurately. We combine SuperSpread
with two typical cardinality estimator (Adaptive Counter and Linear Counter).
We compare it with three state-of-the-art algorithms, GMF, SpreadSketch to show the effective and efficient of super spread detection. 


## File
- cSkt/. the implementation of cSkt
- GMF/. the implementation of GMF
- SpreadSketch/. the implementation of SpreadSketch
- SKAC/. the implementation of SuperKeeper with Adaptive Counter estimator
- SKLC/. the implementation of SuperKeeper with Linear Counter estimator
- Estimator/. the implementation of two estiamtor
## Data
we provide a test case in data folder.
This dataset is collected from a [real-world e-commerce website ](https://www.kaggle.com/retailrocket/ecommerce-dataset?select=events.csv) and we use the visitor 
behavior data. We take item ID as flow label and visitor ID as 
element identifier.


## Requirement
Ensure g++ and make are installed. Our experimental platform is equipped with Ubuntu 16, g++ 5.4 and make 4.1

## Compile and Run
```
$cd demo
$make clean
$make
$./main.out
```
Then input parameter according to the guideline. You will get the result of previous mentioned algorithms.
