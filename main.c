#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

/*
* Фибоначиева пирамида - набор корневых деревьев,
* упорядоченных в соответствии со свойствами
* неубывающей пирамиды, те каждое дерево подчиняется свойству:
* ключ узла не меньше ключа его родителя.
*/

/*
* Проблемы в реализации:
* 3 - где используется "child"
* 4 - нужна ли ф-ция создания дерева
* 5 - как сделать форич
* 6 - как создать ДЕРЕВО, а не узел
* 7 - нужно ли прибавлять degree
* 8 - почему мы возвращаем z, если мы его удалили fib_heap_delete_min?
*/

struct heapnode{
	int key; /* ключ */
	char *value; /* значение */

	struct heapnode *parent; /* указатель на родительский узел */
	struct heapnode *child; /* указатель на дочерний список узлов */
	struct heapnode *left; /* указатель на левый узел, может быть цикличным */
	struct heapnode *right; /* указатель на правый узел, может быть цикличным */

	int degree; /* количество узлов в дочернем списке */

	bool mark; /* были ли изменения дочерних узлов начиная с момента, когда x стал дочерним*/
};

struct heap{
	struct heapnode *min; /* указатель на минимальный элемент кучи */
	int n; /* количество элементов в кучу */
};

void fib_heap_add_node_to_root_list(struct heapnode *node, struct heapnode *h);

int D(int n)
{
	return floor(log2(n));
}

/* уплотнение списка корней */
void fib_heap_consolidate(struct heap *heap)
{
	int array_size = D(heap->n) + 1;
	struct heapnode **temp_array = malloc(array_size * sizeof(struct heapnode*));
	for (int i = 0; i <= array_size; ++i)
		temp_array[i] = NULL;

	struct heapnode *begin_node = heap->min;
	struct heapnode *temp_node = heap->min->right;
	struct heapnode *x, *y;

	do{
		x = temp_node;
		int d = x->degree;
		while (temp_array[d] != NULL){
			y = temp_array[d];
			if (x->key > y->key){
				struct heapnode *swap_node = x;
				x = y;
				y = swap_node;
			}
		printf("Link\n");
		temp_array[d] = NULL;
		++d;
		}
		temp_array[d] = x;

		temp_node = temp_node->right;
		} while (temp_node != begin_node);

		heap->min = NULL;
		for (int i = 0; i <= array_size; ++i){
			if (temp_array[i] != NULL){
		/* добавляем temp_array[i] в список корней */
				fib_heap_add_node_to_root_list(temp_array[i], heap->min);
					if ((heap->min == NULL) || (temp_array[i]->key < heap->min->key))
						heap->min = temp_array[i];
			}
	}
}

/* простой foreach для итерации объектов типа heapnode */
void simple_foreach(struct heapnode *z, struct heap *heap)
{
	struct heapnode *begin_node = z;
	struct heapnode *temp_node = begin_node->right;

	do{
		fib_heap_add_node_to_root_list(temp_node, heap->min);
		temp_node->parent = NULL;

		/* рекурсивыный вызов */
		//if (temp_node->child != NULL)
		//simple_foreach(temp_node, heap);

		/* добавляем итерацию */
		temp_node = temp_node->right;

	} while (temp_node != begin_node);
}

/* удаляем узел из списка корней */
void fib_heap_remove_from_root_list(struct heapnode *z)
{
	if (z->left == z){
		printf("List have 1 node\n");
	return;
	}
	
	else{
		z->right->left = z->left;
		z->left->right = z->right;
	}
}

/* удаление минимального узла */
struct heapnode *fib_heap_delete_min(struct heap *heap)
{
	struct heapnode *z = heap->min;

	if (z == NULL)
		return NULL;

	/* поднимаем дочерние узлы в список корней */
	if (z->child == NULL)
		printf("z.child == NULL in func \"fib_heap_delete\"\n");
	else
		simple_foreach(z->child, heap);

	/* удаляем z из списка корней */
	fib_heap_remove_from_root_list(z);

	if (z == z->right)
		heap->min = NULL;
	else{
		heap->min = z->right;
		fib_heap_consolidate(heap);
	}
	--heap->n;
	return z;
}

/* конкатенация списков корней */
void fib_heap_link_lists(struct heapnode *heap1, struct heapnode *heap2)
{
	if ((heap1 == NULL) || (heap2 == NULL))
		return;

	struct heapnode *left1 = heap1->left;
	struct heapnode *left2 = heap2->left;

	left1->right = heap2;
	heap2->left = left1;
	heap1->left = left2;
	left2->right = heap1;
}

/* слияние двух фиб. куч */
struct heap *fib_heap_union(struct heap *heap1, struct heap *heap2)
{
	struct heap *heap = malloc(sizeof(*heap));
	heap->min = heap1->min;

	fib_heap_link_lists(heap1->min, heap2->min);

	if ((heap1->min == NULL) || ((heap2->min != NULL) && (heap2->min->key < heap1->min->key)))
		heap->min = heap2->min;

	heap->n = heap1->n + heap2->n;

	free(heap1);
	free(heap2);

	return heap;
}

/* получение минимального элемента кучи */
struct heapnode *fib_heap_min(struct heap *heap)
{
	return heap->min;
}

/* добавить узел node в список корней */
void fib_heap_add_node_to_root_list(struct heapnode *node, struct heapnode *h)
{
	if (h == NULL)
		return;

	/* случай 1: список h содержит 1 корень */
	if (h->left == h){
		h->left = node;
		node->right = h;
		node->left = h;
	}
	/* случай 2: список h содержит > 1 корня */
	else{
		struct heapnode *lnode = h->left;
		h->left = node;
		node->right = h;
		node->left = lnode;
		lnode->right = node;
	}
}

/* вставка узла с ключем "key" и значением "value" в фиб.пирамиду */
struct heap *fib_heap_insert(struct heap *heap, int key, char *value)
{
	struct heapnode *node = malloc(sizeof(*node));

	if (node == NULL){
		printf("Memory allocation error in the function \"fib_heap_insert\"\n");
		return NULL;	
	}

	node->key = key;
	node->value = value;
	node->degree = 0;
	node->mark = false;
	node->parent = NULL;
	node->child = NULL;
	node->left = node;
	node->right = node;

	/*
	* if (heap == NULL){
	* heap = malloc(sizeof(*heap));
	* heap->min = node;
	* heap->n = 1;
	* }
	* */

	if (heap == NULL){
		heap = malloc(sizeof(*heap));
		heap->min = node;
		heap->n = 1;	
	}

	if (heap == NULL){
		printf("Memory error. Struct heap *heap == NULL \n");
		getchar();	
	}

	printf("add node in the list heap\n");
	/* добавляем node в список корней _heap */
	getchar();

	fib_heap_add_node_to_root_list(node, heap->min);

	if (node->key < heap->min->key)
		heap->min = node;

	++heap->n;
	return heap;
}

int main()
{
	struct heap *node = NULL;
	node = fib_heap_insert(node, 5, "5");
	node = fib_heap_insert(node, 4, "4");
	node = fib_heap_insert(node, 1, "1");
	node = fib_heap_insert(node, 1000, "1000");
	printf("%d - %s\n", fib_heap_min(node)->key, fib_heap_min(node)->value);

	struct heap *node2 = NULL;
	node2 = fib_heap_insert(node2, -1, "-1");
	
	node = fib_heap_union(node, node2);
	printf("%d - %s\n", fib_heap_min(node)->key, fib_heap_min(node)->value);

	fib_heap_delete_min(node);

	printf("%d - %s\n", fib_heap_min(node)->key, fib_heap_min(node)->value);

	printf("%d\n", D(15));
	
	printf("Press any key...");
	getchar();

	return 0;
} 
