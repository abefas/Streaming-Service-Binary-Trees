/*****************************************************
 * @file   Movie.c                                   *
 * @author Paterakis Giorgos <geopat@csd.uoc.gr>     *
 *                                                   *
 * @brief Implementation for Movie.h 				 *
 * Project: Winter 2023						         *
 *****************************************************/
#include <stdlib.h>
#include <time.h>
#include "Movie.h"
#include <unistd.h>
/**
 * @brief Creates a new user.
 * Creates a new user with userID as its identification.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */

int get_next_prime(int key){
    for(int i = 0; i < 170; i++){
        if(primes_g[i] > key){
            return primes_g[i];
        }
    }
    return EXIT_FAILURE;
}

int generate_hashkey(int userID){
    int a,b,p,m;
    p = get_next_prime(max_id);
    time_t t;
    srand((unsigned) time(&t));
    a = (rand() % (p-1)) + 1;
    b = rand() % p;
    m = 1.3 * max_users;
    return ((a*userID+b) % p ) % m;
}

int register_user(int userID){
    int h = generate_hashkey(userID);
    user_t *temp;
    if(user_hashtable_p[h]->userID == -1){
        user_hashtable_p[h]->userID = userID;
    }else{
        user_t *new_node = malloc(sizeof *new_node);
        new_node->userID = userID;
        new_node->history = NULL;
        new_node->next = user_hashtable_p[h];
        user_hashtable_p[h] = new_node;
    }
    printf("Chain %d of Users:\n", h);
    temp = user_hashtable_p[h];
    while(temp){
        printf("\t%d\n", temp->userID);
        temp = temp->next;
    }
    printf("DONE\n");

    return 1;
}
 
/**
 * @brief Deletes a user.
 * Deletes a user with userID from the system, along with users' history tree.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */
void free_user_history(userMovie_t *root){
    if (root != NULL){
        free_user_history(root->lc);
        free_user_history(root->rc);
        free(root);
    }
    return;
}

int unregister_user(int userID){
    int hash_key = generate_hashkey(userID);
    user_t *p = user_hashtable_p[hash_key];
    user_t *prev = NULL;
    if(p->userID == -1){
        perror("User to unregister not found\n");
        exit(1);
    }
    while(p){
        if(p->userID == userID){
            break;
        }
        prev = p;
        p = p->next;
    }
    if(!p){
        perror("User to unregister not found\n");
        return 0;
    }
    free_user_history(p->history);
    if(!prev){
        user_hashtable_p[hash_key] = p->next;
        free(p);
    }else{
        prev->next = p->next;
        free(p);
    }
    printf("Chain %d of Users:\n", hash_key);
    p = user_hashtable_p[hash_key];
    while(p){
        printf("%d\n", p->userID);
        p = p->next;
    }
    printf("\nDONE\n");
    return 1;
}
 
 /**
 * @brief Add new movie to new release binary tree.
 * Create a node movie and insert it in 'new release' binary tree.
 *
 * @param movieID The new movie identifier
 * @param category The category of the movie
 * @param year The year movie released
 *
 * @return 1 on success
 *         0 on failure
 */


new_movie_t* createNode(int movieID, int category, int year) {
    new_movie_t *new_node = malloc(sizeof(new_movie_t));
    new_node->movieID = movieID;
    new_node->category = category;
    new_node->year = year;
    new_node->watchedCounter = 0;
    new_node->sumScore = 0;
    new_node->lc = new_node->rc = NULL;

    return new_node;
}

new_movie_t* insert(new_movie_t *new_releases, int movieID, int category, int year) {

    if(!new_releases){
        return createNode(movieID, category, year);
    }

    if(movieID < new_releases->movieID){
        new_releases->lc = insert(new_releases->lc, movieID, category, year);
    }else if(movieID > new_releases->movieID){
        new_releases->rc = insert(new_releases->rc, movieID, category, year);
    }
    return new_releases;

}

void inorderTraversal(new_movie_t* root, int cond) {
    if (root != NULL) {
        inorderTraversal(root->lc, cond);
        if(cond == 1){
            printf("%d ", root->movieID);
        }else if(cond == 2){
            counter[root->category]++;
        }
        inorderTraversal(root->rc, cond);
    }
}

int add_new_movie(int movieID, int category, int year){

    if(!new_releases){
        new_releases = insert(new_releases, movieID, category, year);
    }else{
        insert(new_releases, movieID, category, year);
    }
    printf("A %d %d %d\n", movieID, category, year);
    printf("New releases Tree:\n");
    inorderTraversal(new_releases, 1);
    printf("\nDONE\n");

    return 1;
}
 /**
 * @brief Distribute the movies from new release binary tree to the array of categories.
 *
 * @return 0 on success
 *         1 on failure
 */

void inorderTraversal_D(movie_t* root) {
    if (root != NULL) {
        inorderTraversal_D(root->lc);
        if(root->movieID != -1){
            printf("%d ", root->movieID);
        }
        inorderTraversal_D(root->rc);
    }
}

void inorder_add_mid(new_movie_t **array, new_movie_t *root, int *c) {
    if (root != NULL) {
        inorder_add_mid(array, root->lc, c);
        array[root->category][c[root->category]++] = *root;
        inorder_add_mid(array, root->rc, c);
    }
}

movie_t *newNode(new_movie_t data){
    movie_t *node = malloc(sizeof(movie_t));
    node->movieID = data.movieID;
    node->year = data.year;
    node->watchedCounter = data.watchedCounter;
    node->sumScore = data.sumScore;

    node->rc = NULL;
    node->lc = NULL;

    return node;
}

movie_t *sortedArrayToBST(new_movie_t arr[], int start, int end){

    if(start > end)
        return sentinel_node;
 
    int mid = (start + end) / 2;
    movie_t *root = newNode(arr[mid]);
 
    root->lc = sortedArrayToBST(arr, start, mid - 1);
 
    root->rc = sortedArrayToBST(arr, mid + 1, end);
 
    return root;
}


int distribute_movies(void){

    inorderTraversal(new_releases, 2);//get number of movies in each category

    new_movie_t *array_for_trees[6];
    for(int i = 0; i < 6; i++){
        if(counter[i] != 0)
            array_for_trees[i] = malloc(sizeof(new_movie_t) * counter[i]);
    }

    int c[6] = {0}; //counter
    inorder_add_mid(array_for_trees, new_releases, c); //add movies to arrays

    //create BSTs for categoryArray[]
    for(int i = 0; i < 6; i++){
        for(int j = 0; j < counter[i]; j++){
            printf("%d ", array_for_trees[i][j].movieID);
        }
        printf("\n");
        categoryArray[i]->movie = sortedArrayToBST(array_for_trees[i], 0, counter[i]-1);
    }


    printf("Movie Category Array:\n");
    for(int i = 0; i < 6; i++){
        printf("\t%d: ", i);
        inorderTraversal_D(categoryArray[i]->movie);
        printf("\n");
    }
    
    printf("\nDONE\n");

    return 1;
}
 
 /**
 * @brief User rates the movie with identification movieID with score
 *
 * @param userID The identifier of the user
 * @param category The Category of the movie
 * @param movieID The identifier of the movie
 * @param score The score that user rates the movie with id movieID
 *
 * @return 1 on success
 *         0 on failure
 */

void printWatchHistory(userMovie_t *root){
    if (root != NULL){
        printWatchHistory(root->lc);
        if(!root->lc)
            printf("%d, %d\n", root->movieID, root->score);
        printWatchHistory(root->rc);
    }
}

user_t *find_user_node(int userID){
    int hash_key = generate_hashkey(userID);
    user_t *p = user_hashtable_p[hash_key];
    if(p->userID == -1){
        perror("User not found in find_user_node\n");
        exit(1);
    }
    while(p){
        if(p->userID == userID){
            return p; 
        }
        p = p->next;
    }
}

void add_to_history(user_t *user_node, int category, int movieID, int score){

    userMovie_t *newNode = malloc(sizeof(userMovie_t));
    newNode->category = category;
    newNode->movieID = movieID;
    newNode->score = score;

    if(user_node->history == NULL){
        user_node->history = malloc(sizeof(userMovie_t));
        user_node->history = newNode;
        user_node->history->parent = NULL;
        user_node->history->lc = NULL;
        user_node->history->rc = NULL;
        return;
    }else if(user_node->history && !user_node->history->lc){
        userMovie_t *newRoot = malloc(sizeof(userMovie_t));
        newRoot->category = -1;
        newRoot->score = -1;
        newRoot->parent = NULL;
        if(user_node->history->movieID < newNode->movieID){
            newRoot->movieID = newNode->movieID;
            newRoot->lc = user_node->history;
            newRoot->rc = newNode;
            user_node->history->parent = newRoot;
            newNode->parent = newRoot;
        }else{
            newRoot->movieID = user_node->history->movieID;
            newRoot->rc = user_node->history;
            newRoot->lc = newNode;
            user_node->history->parent = newRoot;
            newNode->parent = newRoot;
        }
        user_node->history = newRoot;
        return;
    }else{
        userMovie_t *p = user_node->history;
        userMovie_t *parent = NULL;
        while(p){
            if(newNode->movieID < p->movieID){
                if(p->lc != NULL){
                    p = p->lc;
                }else{
                    parent = p->parent;
                    break;
                }
            }else{
                if(p->rc != NULL){
                    p = p->rc;
                }else{
                    parent = p->parent;
                    break;
                }
            }
        }
        userMovie_t *newParent = malloc(sizeof(userMovie_t));
        newParent->category = -1;
        newParent->score = -1;
        newParent->parent = parent;

        if(newNode->movieID < p->movieID){
            newParent->movieID = p->movieID;
            /* an to newParent tha mpei deksia i aristera tou parent  */
            if(parent->movieID > newNode->movieID){
                parent->lc = newParent;
            }else{
                parent->rc = newParent;
            }
            newNode->parent = newParent;
            p->parent = newParent;
            newParent->lc = newNode;
            newParent->rc = p;
        }else if(newNode->movieID > p->movieID){
            newParent->movieID = newNode->movieID;
            if(parent->movieID > newNode->movieID){
                parent->lc = newParent;
            }else{
                parent->rc = newParent;
            }
            newNode->parent = newParent;
            p->parent = newParent;
            newParent->lc = p;
            newParent->rc = newNode;
        }
    }
    return;
}

int watch_movie(int userID, int category, int movieID, int score){

    movie_t *p = categoryArray[category]->movie;
    user_t *user_node;
    while(p != sentinel_node){
        if(p->movieID == movieID){
            p->watchedCounter++;
            p->sumScore += score;
            user_node = find_user_node(userID);
            add_to_history(user_node, category, movieID, score);
            break;
        }else if(movieID < p->movieID){
            p = p->lc;
        }else{
            p = p->rc;
        }
    }
    
    printf("W %d %d %d %d\n", userID, category, movieID, score);
    printf("History Tree of user %d\n", userID);
    printWatchHistory(user_node->history);
    printf("root: %d\n", user_node->history->movieID);
    printf("\nDONE\n");
    return 1;
}
 
/**
 * @brief Identify the best rating score movie and cluster all the movies of a category.
 *
 * @param userID The identifier of the user
 * @param score The minimum score of a movie
 *
 * @return 1 on success
 *         0 on failure
 */
int countMovies(movie_t *root, int score, int k){
    if(root){
        k = countMovies(root->lc, score, k);
        if(root->watchedCounter != 0 && root->sumScore/root->watchedCounter >= score){
            k++;
        }
        k = countMovies(root->rc, score, k);
    }
    return k;
}

int fillArray(movie_t **array, movie_t *root, int score, int k){
    if(root){
        k = fillArray(array, root->lc, score, k);
        if(root->watchedCounter != 0 && root->sumScore/root->watchedCounter >= score){
            array[k] = root;
            printf("array[%d]->movieID = %d\n", k, array[k]->movieID);
            k++;
        }
        k = fillArray(array, root->rc, score, k);
    }
    return k;
}

void heapify(movie_t *arr[], int N, int i){
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < N && arr[left]->movieID > arr[largest]->movieID);
        largest = left;
    if (right < N && arr[right]->movieID > arr[largest]->movieID);
        largest = right;
    if (largest != i) {
        movie_t temp = *arr[i];
        *arr[i] = *arr[largest];
        *arr[largest] = temp;
        heapify(arr, N, largest);
    }
}

void heapSort(movie_t *arr[], int N){
    for(int i = N / 2 - 1; i >= 0; i--)
        heapify(arr, N, i);
    for (int i = N - 1; i >= 0; i--) {
        movie_t temp = *arr[0];
        *arr[0] = *arr[i];
        *arr[i] = temp;
        heapify(arr, i, 0);
    }
}

int filter_movies(int userID, int score){
    int numMovies = 0;
    movie_t *p;
    for(int i = 0; i < 6; i++){
        p = categoryArray[i]->movie;
        numMovies += countMovies(p, score, 0);
    }
    printf("numMovies %d\n", numMovies);

    movie_t **array = malloc(sizeof(movie_t*) * numMovies);
    int k = 0;
    for(int i = 0; i < 6; i++){
        p = categoryArray[i]->movie;
        k = fillArray(array, p, score, k);
    }
    printf("F %d %d\n", userID, score);
    //heapSort(array, numMovies);
    for(int i = 0; i < numMovies; i++){
        printf("%d %d \t", array[i]->movieID, array[i]->sumScore/array[i]->watchedCounter);
    }
    printf("\nDONE\n");
    return 1;
}
 
/**
 * @brief Find movies from categories withn median_score >= score t
 *
 * @param userID The identifier of the user
 * @param category Array with the categories to search.
 * @param score The minimum score the movies we want to have
 *
 * @return 1 on success
 *         0 on failure
 */
userMovie_t *findNextLeaf(userMovie_t *v){
    if(v->rc){
        userMovie_t *next = v->rc;
        while(next->lc){
            next = next->lc;
        }
        ScoreSum += next->score;
        Qcounter++;
        return findNextLeaf(next);
    }
    userMovie_t *current = v;
    while(current->parent && current == current->parent->rc){
        current = current->parent;
    }
    if(current->parent){
        return findNextLeaf(current->parent);
    }else{
        return current;//root of tree
    }
}

userMovie_t *findLeftChild(userMovie_t *u){
    while(u->lc){
        u = u->lc;
    }
    return u;
}

int user_stats(int userID){
    user_t *user_node = find_user_node(userID);
    userMovie_t *u;
    if(user_node->history->movieID == -1){
        printf("No history for user %d\n", userID);
        return -1;
    }else{
        u = findLeftChild(user_node->history);
    }
    ScoreSum = u->score;
    Qcounter = 1;
    u = findNextLeaf(u);
    float avg;
    if(Qcounter != 0){
        avg = (float)ScoreSum/Qcounter;
    }else{
        avg = 0;
    }

    printf("rightmost child %d\n", u->movieID);
    printf("Q %d %f", userID, avg);
    printf("\nDONE\n");
    return 1;
}
 
/**
 * @brief Search for a movie with identification movieID in a specific category.
 *
 * @param movieID The identifier of the movie
 * @param category The category of the movie
 *
 * @return 1 on success
 *         0 on failure
 */

int search_movie(int movieID, int category){
    movie_t *p = categoryArray[category]->movie;
    if(p == NULL){
        printf("\nMovies do not exist\n");
        return -1;
    }
    while(p != sentinel_node){
        if(movieID == p->movieID){
            printf("\nI: %d %d %d\n",movieID, category, p->year);
            printf("\nDONE\n");
            return 1;
        }else if(movieID < p->movieID){
            p = p->lc;
        }else if(movieID > p->movieID){
            p = p->rc;
        }
    }
    printf("\n MovieID: %d, does not exist in category: %d \n",movieID, category);
    return -1;
}

 /**
 * @brief Prints the movies in movies categories array.
 * @return 1 on success
 *         0 on failure
 */

void inOrderTraversal4Movies(movie_t *root){
    if(root != sentinel_node){
        inOrderTraversal4Movies(root->lc);
        printf("%d, ",root->movieID);
        inOrderTraversal4Movies(root->rc);
    }
}

int print_movies(void){
    printf("\nMovie Category Array:\n");
    for(int i = 0; i < 6; i++){
        printf("\n\t%d: ",i);                                         // print the category
        inOrderTraversal4Movies(categoryArray[i]->movie);             // print In Order 
    }
    return 1;
}
 
  /**
 * @brief Prints the users hashtable.
 * @return 1 on success
 *         0 on failure
 */

int print_users(void){
    int len = 1.3 * max_users;
    for (int i = 0; i < len; i++){
        printf("\nChain %d of Users:\n", i);
        user_t *temp = user_hashtable_p[i];
        printf("%d\n", temp->userID);
        if(temp->userID != -1){
            while(temp != NULL){
                printf("\t%d\n", temp->userID);
                printf("\t\t History Tree: \n");
                if(temp->history == NULL){
                    printf("\n\t\t\tEmpty \n");
                }else{
                    printWatchHistory(temp->history);
                }
                temp = temp->next;
            }
        }else{
            printf("\n\tNo users here\n");
        }
    }
    return 1;
}
 
