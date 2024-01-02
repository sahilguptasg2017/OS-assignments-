## Assignment 4: Concurrency and Synchronization Primitives

### Question 1: Dining Philosophers Problem Modification

**Task:**
Model a modified version of the dining philosophers problem using threads. Each philosopher carries out eating and thinking tasks, requiring forks and bowls. Use Mutexes (locks) and Conditional Variables to ensure synchronization.

**Guidelines:**
- Structure your code with the following functions:
  - `void* philosopher(void* args)`: For running philosopher thread.
  - `void eating()`: For entering the eating state.
  - `void thinking()`: For entering the thinking state.

**Deliverables:**
- C code
- README file explaining:
  - Why deadlocks can occur
  - How your proposed solution avoids deadlock
  - Fairness of the solution (number of philosophers able to eat)

### Question 2: Car and Passengers Problem

**Task:**
Simulate a car ride where passengers board and disembark. Car must load passengers, wait, run the ride, and unload. Use threads and semaphores for synchronization.

**Guidelines:**
- Ensure the following functions:
  - `void* car(void* args)`: Car thread.
  - `void* passenger(void* args)`: Passenger thread.
  - `void load()`: Loading car with passengers.
  - `void unload()`: Unloading passengers.
  - `void board()`: Passenger boards car.
  - `void offboard()`: Passenger gets off car.

**Deliverables:**
- C code
- Writeup explaining code logic and concurrency bug avoidance.

### Question 3: Thin Bridge Problem

**Task:**
Model a thin bridge problem where cars from both sides need to cross without violating constraints. Use semaphores for synchronization.

**Guidelines:**
- Include the following functions:
  - `void* left(void* args)`: Cars on the left.
  - `void* right(void* args)`: Cars on the right.
  - `void passing(int dir)`: Car from some direction is traveling on the bridge.

**Deliverables:**
- C code
- Writeup explaining code logic and concurrency bug avoidance.
