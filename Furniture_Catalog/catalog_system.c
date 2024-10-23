/*
T. Annmarie Geiger (x00468652)

12/5/2023

This program features a catalog that takes customer's orders, returns, and reads from the custom catalog. After orders
are made, all orders minus returns are stored in the 'customer_information.txt' file.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h> 
#include "catalog_system.h" //header file

int main() 
{
    //initialize the catalog and orders
    catalog_t *furnitureCatalog = (catalog_t *)malloc(sizeof(catalog_t));
    furnitureCatalog->head = NULL;

    order_list_t *orders = (order_list_t *)malloc(sizeof(order_list_t));
    orders->head = NULL;

    //load existing records from the file (if any)
    load_catalog_from_file(furnitureCatalog, "furniture_catalog.txt");

    //load customer information from the file
    load_customer_information(orders, "customer_information.txt");

    //display the main menu
    int choice = 0;

    do 
    {
        printf("\n-----Furniture Catalog System-----\n");
        printf(" +   1. Display Catalog          +\n");
        printf(" +   2. Place Order              +\n");
        printf(" +   3. Display Current Orders   +\n");
        printf(" +   4. Process Return           +\n");
        printf(" +   5. Display Returns          +\n");
        printf(" +   6. Quit                     +\n");
        printf("----------------------------------\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Clear the input buffer
        while (getchar() != '\n');

        switch (choice) 
        {
            case 1:
                display_catalog(furnitureCatalog);
                break;
            case 2:
                place_order(orders, furnitureCatalog);
                break;
            case 3:
                display_orders(orders);
                break;
            case 4:
                process_return(orders);
                break;
            case 5:
                display_returns(orders);
                break;
            case 6:
                //save orders before quitting
                save_customer_information(orders, "customer_information.txt");
                free_catalog(furnitureCatalog);
                free_orders(orders);
                quit_program();
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 6.\n");
        }

    } while (choice != 6);

    return 0;
}


void place_order(order_list_t *orders, const catalog_t *catalog) 
{
    char customerName[MAX_NAME];
    int productNumber;
    int quantity;
    char orderNumber[MAX_ORDER_NUMBER];

    //customer name with buffer size
    printf("Enter your name: ");
    fgets(customerName, MAX_NAME, stdin);

    //generate a single order number for the entire order
    generate_order_number(orderNumber);

    do 
    {
        //gets product number from the user
        printf("Enter the product number you want to order (0 to finish): ");

        //input validation
        if (scanf("%d", &productNumber) != 1) 
        {
            printf("Invalid input. Please enter a valid product number.\n");
            while (getchar() != '\n');
            continue;  // Ask for product number again
        }

        //checks if the user wants to finish adding products
        if (productNumber == 0) 
        {
            break;
        }

        //input validation
        if (!isProductNumberValid(catalog, productNumber)) 
        {
            printf("Product number invalid. Product does not exist.\n");
            continue;
        }

        //get quantity and input validation
        printf("Enter the quantity you want to order: ");
        if (scanf("%d", &quantity) != 1 || quantity < 0) 
        {
            printf("Invalid input. Please enter a valid quantity.\n");
            while (getchar() != '\n');
            continue;
        }

        //checks if the product number already exists in the order
        order_node_t *existingOrder = orders->head;

        while (existingOrder != NULL) 
        {
            if (strcmp(existingOrder->order.customerName, customerName) == 0 &&
                strcmp(existingOrder->order.orderNumber, orderNumber) == 0 &&
                existingOrder->order.productNumber == productNumber) {
                //updates the quantity for the existing product number
                existingOrder->order.quantity += quantity;
                printf("Quantity updated for existing product.\n");
                break;
            }

            existingOrder = existingOrder->next;
        }

        //if the product number does not exist in the order, create a new order
        if (existingOrder == NULL) 
        {
            // Create a new order node for each product
            order_node_t *newOrderNode = (order_node_t *)malloc(sizeof(order_node_t));

            //checks if memory allocation is successful
            if (!newOrderNode) 
            {
                printf("Memory allocation failure.\n");
                exit(EXIT_FAILURE);
            }

            //set order details for the new order node
            strcpy(newOrderNode->order.customerName, customerName);
            newOrderNode->order.productNumber = productNumber;
            newOrderNode->order.quantity = quantity;
            strcpy(newOrderNode->order.orderNumber, orderNumber);

            //initialize isReturn to 0 for each new order
            newOrderNode->order.isReturn = 0;

            newOrderNode->next = NULL;

            //adds the new order node to the orders list
            if (orders->head == NULL) 
            {
                orders->head = newOrderNode;
            } else {
                order_node_t *temp = orders->head;
                while (temp->next != NULL) 
                {
                    temp = temp->next;
                }
                temp->next = newOrderNode;
            }

            printf("Product added to order successfully.\n");
        }

    } while (productNumber != 0);

    printf("Order completed. Your order number is: %s\n", orderNumber);
}



void generate_order_number(char *orderNumber) 
{
    srand(time(NULL));
    snprintf(orderNumber, MAX_ORDER_NUMBER, "%04d", rand() % 10000);
}

void display_orders(const order_list_t *orders) 
{
    if (orders->head == NULL) 
    {
        printf("\nNo orders placed yet.\n");
    } else {
        printf("\nCurrent Orders:\n");

        order_node_t *current = orders->head;

        while (current != NULL) 
        {
            printf("Customer: %s (Order No. %s)\n", current->order.customerName, current->order.orderNumber);

            order_node_t *orderGroup = current;

            //print all orders for the current customer and order number
            while (orderGroup != NULL && strcmp(orderGroup->order.customerName, current->order.customerName) == 0 &&
                   strcmp(orderGroup->order.orderNumber, current->order.orderNumber) == 0) {
                printf(" | Product Number: %d | Quantity: %d |\n",
                       orderGroup->order.productNumber, orderGroup->order.quantity);

                orderGroup = orderGroup->next;
            }

            current = orderGroup;  //move to the next customer and order number
        }

        printf("\n");
    }
}

int isProductNumberValid(const catalog_t *catalog, int productNumber) 
{
    product_node_t *current = catalog->head;

    while (current != NULL) 
    {
        if (current->product.productNumber == productNumber) 
        {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

void process_return(order_list_t *orders) 
{
    char orderNumber[MAX_ORDER_NUMBER];

    printf("Enter the order number for the return: ");
    scanf("%s", orderNumber);

    order_node_t *current = orders->head;
    order_node_t *prev = NULL;

    while (current != NULL) 
    {
        if (strcmp(current->order.orderNumber, orderNumber) == 0) 
        {
            //remove the entire order from the current orders list
            if (prev == NULL) 
            {
                orders->head = current->next;
            } else {
                prev->next = current->next;
            }

            //move the entire order to the returns list
            if (orders->returns == NULL) 
            {
                orders->returns = current;
            } else {
                order_node_t *temp = orders->returns;
                while (temp->next != NULL) 
                {
                    temp = temp->next;
                }
                temp->next = current;
            }

            //update current to the next order in the og list
            order_node_t *temp = current;
            current = current->next;

            //set next to NULL for the moved node
            temp->next = NULL;

            printf("Return processed successfully.\n");
        } else {
            prev = current;
            current = current->next;
        }
    }

    //if no matching order is found
    if (orders->returns == NULL) 
    {
        printf("Order number not found. Return cannot be processed.\n");
    }
}



void display_returns(order_list_t *orders) 
{
    int hasReturns = 0;  //flag to track if there are any returns

    order_node_t *current = orders->returns;  //go through the returns list

    while (current != NULL) 
    {
        printf("Customer: %s | Product Number: %d | Quantity: %d | Order Number: %s\n",
               current->order.customerName, current->order.productNumber,
               current->order.quantity, current->order.orderNumber);

        hasReturns = 1;  //set the flag to indicate there are returns
        current = current->next;
    }

    if (!hasReturns) 
    {
        printf("\nNo returns processed yet.\n");
    }

    printf("\n");
}


void quit_program() 
{
    printf("\nThank you for using our Furniture Catalog System!\n");
}

void display_catalog(const catalog_t *catalog) 
{
    if (catalog->head == NULL) 
    {
        printf("\nNo products in the catalog.\n");
    } else 
    {
        printf("\nFurniture Catalog:\n");

        //initialize currentCategory to an empty string
        char currentCategory[MAX_PRODUCT_NAME] = "";
        char previousProduct[MAX_PRODUCT_NAME] = "";

        product_node_t *current = catalog->head;

        while (current != NULL) 
        {
            //if conditionals to check category and product 
            //only print the category when it changes
            if (strcmp(currentCategory, current->product.category) != 0) {
                if (current != catalog->head) {
                    printf("\n");  //add a newline before printing a new category
                }
                strcpy(currentCategory, current->product.category);
                printf("  %s:\n", currentCategory);

                //reset the previous product for the new category
                previousProduct[0] = '\0';
            }

            //only prints the product name if it's different from the previous one
            if (strstr(current->product.name, ":") == NULL &&
                strcmp(current->product.name, previousProduct) != 0) {
                printf("    %s, product no. %d\n", current->product.name, current->product.productNumber);

                //update the previous product
                strcpy(previousProduct, current->product.name);
            }

            current = current->next;
        }

        printf("\n");
    }
}

void free_catalog(catalog_t *catalog) 
{
    //free allocated memory for the catalog
    product_node_t *current = catalog->head;
    product_node_t *next;

    while (current != NULL) 
    {
        next = current->next;
        free(current);
        current = next;
    }

    free(catalog);
}

void free_orders(order_list_t *orders) 
{
    //free allocated memory for the orders
    order_node_t *current = orders->head;
    order_node_t *next;

    while (current != NULL) 
    {
        next = current->next;
        free(current);
        current = next;
    }

    free(orders);
}

void load_catalog_from_file(catalog_t *catalog, const char *filename) 
{
    FILE *file = fopen(filename, "r");

    if (!file) 
    {
        printf("Error opening file: %s\n", filename); //catching file opening errors
        return;
    }

    char line[MAX_PRODUCT_NAME];
    char category[MAX_PRODUCT_NAME] = "";

    while (fgets(line, sizeof(line), file) != NULL) 
    {
        //remove newline character from the end of the line
        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '-') 
        {
            //create new category
            strcpy(category, line + 1);
        } else {
            //parsing product info
            product_t product;
            sscanf(line, "%[^,], product no. %d", product.name, &product.productNumber);
            strcpy(product.category, category);

            //add the product to the catalog
            product_node_t *newNode = (product_node_t *)malloc(sizeof(product_node_t));

            if (!newNode) 
            {
                printf("Memory allocation failure.\n");
                exit(EXIT_FAILURE);
            }

            newNode->product = product;
            newNode->next = NULL;

            if (catalog->head == NULL) 
            {
                catalog->head = newNode;
            } else 
            {
                product_node_t *temp = catalog->head;
                while (temp->next != NULL) 
                {
                    temp = temp->next;
                }
                temp->next = newNode;
            }
        }
    }

    fclose(file);
}

void save_customer_information(const order_list_t *orders, const char *filename) 
{
    FILE *file = fopen(filename, "w");

    if (!file) //if file isn't opening or hasn't opened yet
    {
        printf("Error opening file: %s\n", filename);
        return;
    }

    order_node_t *currentOrder = orders->head;

    while (currentOrder != NULL) {
        //print customer information and order number without newline
        fprintf(file, "%s order no. %s", currentOrder->order.customerName, currentOrder->order.orderNumber);

        //print product details for the current order
        fprintf(file, " | Product No. %d | Quantity: %d |\n", currentOrder->order.productNumber, currentOrder->order.quantity);

        currentOrder = currentOrder->next;
    }

    fclose(file);
}

void load_customer_information(order_list_t *orders, const char *filename) 
{
    //open the file for reading
    FILE *file = fopen(filename, "r");

    //check if the file is opened successfully
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_NAME];
    char orderNumber[MAX_ORDER_NUMBER];

    //reads customer information and order number from the file
    while (fscanf(file, " %[^\n] order no. %s", line, orderNumber) == 2) 
    {
        //display information about the loaded customer and order number
        printf("Loaded customer: %s, order number: %s\n", line, orderNumber);

        //create a new order node for each customer and order number
        order_node_t *newOrderNode = (order_node_t *)malloc(sizeof(order_node_t));

        //check if memory allocation is successful
        if (!newOrderNode) 
        {
            printf("Memory allocation failure.\n");
            exit(EXIT_FAILURE);
        }

        //copy customer name and order number, ensuring null-terminated strings
        strncpy(newOrderNode->order.customerName, line, MAX_NAME - 1);
        newOrderNode->order.customerName[MAX_NAME - 1] = '\0';

        strncpy(newOrderNode->order.orderNumber, orderNumber, MAX_ORDER_NUMBER - 1);
        newOrderNode->order.orderNumber[MAX_ORDER_NUMBER - 1] = '\0';

        //initialize isReturn to 0 for each new order
        newOrderNode->order.isReturn = 0;

        newOrderNode->next = NULL;

        //consume the newline character after reading customer information
        fgetc(file);

        //read product numbers and quantities for the current order
        while (fscanf(file, "| Product No. %d | Quantity: %d |",
                      &newOrderNode->order.productNumber, &newOrderNode->order.quantity) == 2) {
            //display information about the loaded product details
            printf("   Product number: %d, Quantity: %d\n",
                   newOrderNode->order.productNumber, newOrderNode->order.quantity);

            //adds the new order node to the orders list
            if (orders->head == NULL) 
            {
                orders->head = newOrderNode;
            } else 
            {
                order_node_t *temp = orders->head;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = newOrderNode;
            }

            //consume the newline character after reading product details
            fgetc(file);
        }
    }

    //close the file
    fclose(file);
}