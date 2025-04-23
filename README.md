<div align="justify">

# Ramificación y Poda para Camino de Coste Mínimo

Este repositorio alberga una solución en C++ que aplica el método de **ramificación y poda** (branch & bound) para encontrar el camino de mínima dificultad en una cuadrícula bidimensional. Cada celda de la malla está asociada a un valor natural que representa su coste de acceso, y el objetivo es conectar la esquina superior izquierda con la inferior derecha minimizando la suma de todos los costes. La estrategia aprovecha cotas optimistas y pesimistas para descartar de forma sistemática aquellas ramas de la búsqueda que no pueden conducir a una solución mejor que la ya conocida, garantizando así un uso eficiente de los recursos computacionales.

![image](https://github.com/user-attachments/assets/d666389b-0fa8-431a-80e7-34a79e2ff7db)

## Estructura del Nodo

En el núcleo del algoritmo, cada nodo almacena su posición `(x,y)`, el coste acumulado desde el origen, una estimación optimista del coste restante hasta la meta y el histórico de casillas recorridas. Esta organización de datos permite priorizar la exploración de aquellos nodos cuyo `optimisticCost` resulte más prometedor, al tiempo que se preserva la información necesaria para reconstruir el camino completo una vez alcanzada la celda final.

## Cota Optimista

Para calcular la **cota optimista**, se recurre a la distancia de Chebyshev entre las coordenadas actuales y el destino. Esta métrica —que equivale al número mínimo de desplazamientos diagonales posibles— ofrece una estimación ajustada, nunca sobrestimada, del coste restante. Su bajo coste computacional facilita su aplicación en tiempo real durante la exploración, y su precisión contribuye a una poda más agresiva de los subárboles que no pueden mejorar la solución vigente.

## Cota Pesimista

La primera aproximación a la cota pesimista intentó basarse en una matriz precomputada de costes mínimos desde cada casilla al destino, pero resultó compleja de integrar sin obstaculizar la correcta retención de nodos. En su lugar, se adoptó un cálculo inicial mediante un algoritmo iterativo de programación dinámica (vector) que obtiene rápidamente una estimación viable y sensata. Esta cota inicial actúa como umbral superior desde el arranque, descartando a priori todos los nodos cuyo potencial optimista lo supere.

## Algoritmo de Búsqueda

La función principal `mcp_bb` inicia su ejecución generando el nodo raíz y calculando de inmediato su cota pesimista con el método iterativo. A continuación, emplea una cola de prioridad ordenada por `optimisticCost` para ir procesando los nodos más prometedores. En cada iteración, el algoritmo extrae el nodo de menor cota optimista, verifica si ha alcanzado la meta, actualiza la mejor solución conocida y expande sus hasta ocho vecinos solo si su cota los convierte en candidatos válidos. Cada vez que un nodo es descartado —ya sea por no factible o no prometedor— se incrementan contadores estadísticos que permiten evaluar el comportamiento de la poda y la eficiencia global del proceso.

## Resultados y Evaluación

La aplicación demuestra su superioridad frente a enfoques exhaustivos y heurísticos simples, proporcionando siempre el coste mínimo en un tiempo significativamente menor cuando la cuadrícula crece. Por ejemplo, en instancias de tamaño moderado (500×500), el algoritmo encuentra la ruta óptima en unos pocos cientos de milisegundos, mientras que métodos no guiados pueden alargarse mucho más o fallar en encontrar la mejor solución.

## Conclusiones

El empleo combinado de cotas optimistas ligeras y una cota pesimista inicial robusta convierte al método de ramificación y poda en una técnica práctica y escalable para problemas de optimización en grafos implícitos. La capacidad de rechazar sin ambages subárboles enteros reduce drásticamente el número de nodos explorados y, por ende, el tiempo de cómputo, manteniendo al mismo tiempo la garantía de optimalidad. Este enfoque, gracias a su flexibilidad para integrar nuevas estrategias de poda o heurísticas, puede extenderse a multitud de dominios donde resulte crítico encontrar soluciones óptimas de forma eficiente.

</div>
