/*
We will resize:
- up, if load > 0.7
- down, if load < 0.1
To resize, we create a new hash table roughly half or twice as big as the current, 
and insert all non-deleted items into it.
Our new array size should be a prime number roughly double or half the current size. 
Finding the new array size isn't trivial. To do so, we store a base size, 
which we want the array to be, and then define the actual size as the first prime larger than the base size. To resize up, we double the base size, and find the first larger prime, 
and to resize down, we halve the size and find the next larger prime.
Our base sizes start at 50. Instead of storing
We use a brute-force method to find the next prime, 
by checking if each successive number is prime. While brute-forcing anything sounds alarming,
the number of values we actually have to check is low, 
and the time it takes is outweighed by the time spent re-hashing every item in the table.
First, let's define a function for finding the next prime. 
We'll do this in two new files, prime.h and prime.c.
*/
int is_prime(const int x);
int next_prime(int x);