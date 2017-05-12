/* tree.c -- tree support functions */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

/* local data type */
typedef struct pair {
    Trnode * parent;
    Trnode * child;
} Pair;

/* protototypes for local functions */
static Trnode * MakeNode(const Item * pi);
static BOOLEAN ToLeft(const Item * i1, const Item * i2);
static BOOLEAN ToRight(const Item * i1, const Item * i2);
static void AddNode (Trnode * new_node, Trnode * root);
static void InOrder(const Trnode * root, void (* pfun)(Item item));
static Pair SeekItem(const Item * pi, const Tree * ptree);
static void DeleteNode(Trnode **ptr);
static void DeleteAllNodes(Trnode * ptr);

/* function definitions */
void InitializeTree(Tree * ptree)
{
    ptree->root = NULL;
    ptree->size = 0;
}

BOOLEAN TreeIsEmpty(const Tree * ptree)
{
    if (ptree->root == NULL)
        return True;
    else
        return False;
}

BOOLEAN TreeIsFull(const Tree * ptree)
{
    if (ptree->size == MAXITEMS)
        return True;
    else
        return False;
}

int TreeItemCount(const Tree * ptree)
{
    return ptree->size;
}

BOOLEAN AddItem(const Item * pi, Tree * ptree)
{
    Trnode * new_node;
    
    if  (TreeIsFull(ptree))
    {
        fprintf(stderr,"Tree is full\n");
        return False;             /* early return           */
    }
    if (SeekItem(pi, ptree).child != NULL)
    {
        fprintf(stderr, "Attempted to add duplicate item\n");
        return False;             /* early return           */
    }
    new_node = MakeNode(pi);      /* points to new node     */
    if (new_node == NULL)
    {
        fprintf(stderr, "Couldn't create node\n");
        return False;             /* early return           */
    }
    /* succeeded in creating a new node */
    ptree->size++;
    
    if (ptree->root == NULL)      /* case 1: tree is empty  */
        ptree->root = new_node;   /* new node is tree root  */
    else                          /* case 2: not empty      */
        AddNode(new_node,ptree->root); /* add node to tree  */
    
    return True;                  /* successful return      */
}

BOOLEAN InTree(const Item * pi, const Tree * ptree)
{
    return (SeekItem(pi, ptree).child == NULL) ? False : True;
}

BOOLEAN DeleteItem(const Item * pi, Tree * ptree)
{
    Pair look;
    
    look = SeekItem(pi, ptree);
    if (look.child == NULL)
        return False;
    
    if (look.parent == NULL)      /* delete root item       */
        DeleteNode(&ptree->root);
    else if (look.parent->left == look.child)
        DeleteNode(&look.parent->left);
    else
        DeleteNode(&look.parent->right);
    ptree->size--;
    
    return True;
}

void Traverse (const Tree * ptree, void (* pfun)(Item item))
{
    
    if (ptree != NULL)
        InOrder(ptree->root, pfun);
}

void DeleteAll(Tree * ptree)
{
    if (ptree != NULL)
        DeleteAllNodes(ptree->root);
    ptree->root = NULL;
    ptree->size = 0;
}


/* local functions */
static void InOrder(const Trnode * root, void (* pfun)(Item item))
{
    if (root != NULL)
    {
        InOrder(root->left, pfun);
        (*pfun)(root->item);
        InOrder(root->right, pfun);
    }
}

static void DeleteAllNodes(Trnode * root)
{
    Trnode * pright;
    
    if (root != NULL)
    {
        pright = root->right;
        DeleteAllNodes(root->left);
        free(root);
        DeleteAllNodes(pright);
    }
}

static void AddNode (Trnode * new_node, Trnode * root)
{
    if (ToLeft(&new_node->item, &root->item))  // здесь проверка можем ли двигаться в лево - с помощью сравнения строк
    {
        if (root->left == NULL)      /* empty subtree       */     // если нашли пустой узел то вставляем туда новый
            root->left = new_node;   /* so add node here    */
        else
            AddNode(new_node, root->left);/* else process subtree*/   // продолжаем двигаться в лево 
    }
    else if (ToRight(&new_node->item, &root->item)) //здесь проверка можем ли двигаться в право - с помощью сравнения строк
    {
        if (root->right == NULL)                                 //  если нашли пустой узел то вставляем туда новый
            root->right = new_node;                       
        else
            AddNode(new_node, root->right);                            // продолжаем двигаться в право 
    }                                                                 //после установки нового узла ф-ия AddNode шаг за шагом выходит из рекурсии устанавливая root снова в положение корневого узла
    else                         /* should be no duplicates */
    {
        fprintf(stderr,"location error in AddNode()\n");
        exit(1);
    }
}

static BOOLEAN ToLeft(const Item * i1, const Item * i2)
{
    int comp1;
    
    if ((comp1 = strcmp(i1->petname, i2->petname)) < 0)
        return True;
    else if (comp1 == 0 &&
             strcmp(i1->petkind, i2->petkind) < 0 )
        return True;
    else
        return False;
}

static BOOLEAN ToRight(const Item * i1, const Item * i2)
{
    int comp1;
    
    if ((comp1 = strcmp(i1->petname, i2->petname)) > 0)
        return True;
    else if (comp1 == 0 &&
             strcmp(i1->petkind, i2->petkind) > 0 )
        return True;
    else
        return False;
}

static Trnode * MakeNode(const Item * pi)
{
    Trnode * new_node;
    
    new_node = (Trnode *) malloc(sizeof(Trnode));
    if (new_node != NULL)
    {
        new_node->item = *pi;
        new_node->left = NULL;
        new_node->right = NULL;
    }
    
    return new_node;
}

static Pair SeekItem(const Item * pi, const Tree * ptree)
{
    Pair look;  //  с помощью временной переменной двигаемся по дереву
    look.parent = NULL;
    look.child = ptree->root;
    
    if (look.child == NULL)
        return look;                        /* early return   */
    
    while (look.child != NULL)
    {
        if (ToLeft(pi, &(look.child->item))) // выбор левого направление от корневого узла
        {
            look.parent = look.child;   // продолжаем поиск - если не нашли искомый элемент (узел) то переставляем parent на child, а child на следующий левый узел
            look.child = look.child->left;
        }
        else if (ToRight(pi, &(look.child->item)))  // выбор правого направление от корневого узла
        {
            look.parent = look.child;   // продолжаем поиск - если не нашли искомый элемент (узел) то переставляем parent на child, а child на следующий правый узел
            look.child = look.child->right;
        }
        else       /* must be same if not to left or right    */   // это корневой узел
            break; /* look.child is address of node with item */
    }
    
    return look;                       /* successful return   */
}

static void DeleteNode(Trnode **ptr)   // т.к. аргумент должен быть указателем, а передаваемое значение уже указатель на удаляемый узел, то имеем указатель на указатель (**ptr)
/* ptr is address of parent member pointing to target node  */
{
    Trnode * temp;
    
    if ( (*ptr)->left == NULL)
    {
        temp = *ptr;
        *ptr = (*ptr)->right;
        free(temp);
    }
    else if ( (*ptr)->right == NULL)
    {
        temp = *ptr;
        *ptr = (*ptr)->left;
        free(temp);
    }
    else    /* deleted node has two children */
    {
        /* find where to reattach right subtree */
        for (temp = (*ptr)->left; temp->right != NULL;  /* для temp задается адрес левого дочернего узла удаляемого элемента */
             temp = temp->right)
            continue;
        temp->right = (*ptr)->right; // c помощью временной (temp) структуры типа Pair (содержит указатели на дочернии узлы) в цикле находят в левом поддереве удаляемого элемента первый пустой правый узел и вписывают в него адрес правого дочернего узла удаляемого элемента 
        temp = *ptr;				// установка для временной (temp) структуры типа Pair значения удаляемого узла
        *ptr =(*ptr)->left; // переустановка указателя удаляемого узла на его левый дочерний элемент
        free(temp);     // освобождение памяти - удаление искомого узла
    }
}
