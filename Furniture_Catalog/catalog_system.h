#ifndef CATALOG_SYSTEM_H
#define CATALOG_SYSTEM_H

#define MAX_NAME 50
#define MAX_PRODUCT_NAME 50
#define MAX_ORDER_NUMBER 10

//struct to represent a product's name and product number
typedef struct {
    char name[MAX_NAME];
    int productNumber;
    char category[MAX_PRODUCT_NAME];
} product_t;

//node structure for the linked list for all products in the catalog
typedef struct product_node {
    product_t product;
    struct product_node* next; //moves to next in list
} product_node_t;

//struct to represent the catalog
typedef struct {
    product_node_t* head;
} catalog_t;

/*struct for creating orders: customer name, product number(s),
quantity of product, and to check if it's a returned order*/
typedef struct {
    char customerName[MAX_NAME];
    int productNumber;
    int quantity;
    char orderNumber[MAX_ORDER_NUMBER];
    int isReturn;  //indicator for return
} order_t;

//node for the linked list of orders
typedef struct order_node {
    order_t order;
    struct order_node* next;
} order_node_t;

//struct to represent a list of orders
typedef struct {
    order_node_t* head;    //linked list of current orders
    order_node_t* returns; //linked list of returned orders
} order_list_t;

//function declarations

//display the products in the catalog txt file
void display_catalog(const catalog_t *catalog);

//placing new order in the system from customer input
void place_order(order_list_t *orders, const catalog_t *catalog);

//generate a randomized, unique order number for customer after order is executed
void generate_order_number(char *orderNumber);

//displays all of the current orders in the system
void display_orders(const order_list_t *orders);

//checks if a given product number is valid from the catalog txt file
int isProductNumberValid(const catalog_t *catalog, int productNumber);

//processes a return for a given order number
void process_return(order_list_t *orders);

//displays all of the current returns processed in the system
void display_returns(order_list_t *orders);

//quit the program
void quit_program();

//frees the memory allocated for the catalog
void free_catalog(catalog_t *catalog);

//frees the memory allocated for the orders
void free_orders(order_list_t *orders);

//loads product information from a file into the catalog
void load_catalog_from_file(catalog_t *catalog, const char *filename);

//saves customer order information to a file ('customer_information.txt')
void save_customer_information(const order_list_t *orders, const char *filename);

//load customer order information from 'customer_information.txt' file
void load_customer_information(order_list_t *orders, const char *filename);

#endif /* CATALOG_SYSTEM_H */