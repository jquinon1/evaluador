# Evaluador
This is the final project for Operating Systems subject at EAFIT University.

Subject given by: [Juan Francisco Cardona McCormick](https://www.linkedin.com/in/juan-francisco-cardona-mccormick-71baa136/)

# Considerations

* At this moment even though the project accepts all parameters for the command init this only works correctly with the default values for stacks, this is inputs, inboxes, output and intern queues. Other values such as name for the shared memory space, number of blood, dentritus and skin can be modified from command line arguments.
This is because I can't find a way to store dynamic arrays in a shared memory.
* The project could be executed from root folder or form bin folder.
* Definition of states:
  * Waiting: Samples in inbox queues
  * Processing: Samples on intern queues or in processing threads
  * Reported: Samples on output queue
* The time shown in the list processing command is real time.

# Requirements
- C++11

# How to run

To run this project follow these steps.

1. Build the binaries
```bash
make
```

2. Launch the main Executable to see the usage menu
```bash
bin/evaluador
```

3. Start the project with some custom values, this is an examples of how the command should be.
```bash
bin/evaluador init -n mySharedMemory -b 90 -d 110 -s 150
```

4. In another consoles start the register and the controller and the reported or what you need givien the share memory space name you indicated in step 3
```bash
bin/evaluador reg -n mySharedMemory # For interactive
bin/evaluador reg -n mySharedMemory file1 /path/to/file2 # For register from file
```
```bash
bin/evaluador ctrl -n mySharedMemory
```
```bash
bin/evaluador rep -n mySharedMemory -i 20 # To get reports for 20 seconds
bin/evaluador rep -n mySharedMemory -m 20 # To get 20 reports
```

5. Remove the resource created by the init process
```bash
bin/evaluador stop -n mySharedMemory
```

6. Remove binaries
```bash
make clean
```

# Members
* Jhonatan Quiñonez Avila - 201510110010
