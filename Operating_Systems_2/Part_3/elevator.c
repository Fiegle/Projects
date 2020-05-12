#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");

//procfs global variables
#define ENTRY_NAME "elevator"
#define ENTRY_SIZE 150
#define PERMS 0644
#define PARENT NULL
static struct file_operations fops;

//global chars for procfs
static char *finalmessage;
static char *message;
static char *message2;
static char *message3;
static char *message4;
static char *message5;
static int read_p;

//extra functions necessary for ease of programming
void printqueue(void);
void printelevator(void);
void leave_elevator(void);
void load_elevator(int);
int get_next_floor(int);
const char* getState(int);
const char* getPet(int s);
const char* getFloor(int s);
int getWaiting(int s);

typedef struct passenger
{
	//passenger type, allows passengers to be unique instances
	int pet_type;
	int num_pets;
	int start;
	int dest;
	int weight;
	struct list_head list;
} Passenger;

struct
{
	//single static struct for all passengers, uses list to store them
	int total_length;
	struct list_head list;
} passengers;

struct
{
	//single static struct for elevator, contains pointer to list of passengers in elevator
	int STATE;
	int floor;
	int dest;
	int weight;
	int pet_type;
	struct list_head list;
	int shouldstop;
	int length;
	int total;

} elevator;

//absolute value function because there is no standard library containg abs() in kernel
int abso(int s){
	if (s < 0)
		return s * -1;
	return s;
}

struct thread_parameter
{
	//parametrs for a thread
	int id;
	int cnt;
	struct task_struct *kthread;
	struct mutex mutex;
};

//initialize the thread parameter object
struct thread_parameter thread1;

//runs the thread, main elevator loop and stopping eleavtor loop
int thread_run(void *data){
	struct thread_parameter *parm = data;
	int nextfloor;
	int tempstate;
	int diff;
	int movestate;

	elevator.shouldstop = 0;
	ssleep(1);
	while(!kthread_should_stop()){
			parm->cnt++;
			//loop for when elevator is on and no stopped
			printk(KERN_NOTICE "\n------------------------------\n");
			printqueue();
			printelevator();
			tempstate = elevator.STATE;
			elevator.STATE = 2;
			ssleep(1);
			leave_elevator();
			load_elevator(tempstate);
			printk(KERN_NOTICE "current floor is %d", elevator.floor);
			nextfloor = get_next_floor(elevator.STATE);
			diff = elevator.floor - nextfloor;
			movestate = elevator.STATE;
			if (diff == 0)
				ssleep(2);
			else if (diff > 0){
				while (diff != 0){
					elevator.STATE = 4;
					ssleep(2);
					diff--;
					elevator.floor--;
				}
			}else if (diff < 0){
				while (diff != 0){
					elevator.STATE = 3;
					ssleep(2);
					diff++;
					elevator.floor++;
				}
			}
			elevator.STATE = movestate;
			printk(KERN_NOTICE "next floor is %d", nextfloor);
			elevator.floor = nextfloor;
			//printk(KERN_NOTICE "Elevator weight is %d\n", elevator.weight);
			printk(KERN_NOTICE "elevator state is %d\n", elevator.STATE);
	}
	printk(KERN_NOTICE "thread should stop, emptying elevator\n");
	elevator.shouldstop = 1;
	while(elevator.weight != 0){
			printk(KERN_NOTICE "\n------------------------------\n");
			printqueue();
			printelevator();
			printk(KERN_NOTICE "current floor is %d", elevator.floor);
			tempstate = elevator.STATE;
			elevator.STATE = 2;
			ssleep(1);
			leave_elevator();
			nextfloor = get_next_floor(tempstate);
			diff = elevator.floor - nextfloor;
			movestate = elevator.STATE;
			if (diff == 0)
				ssleep(2);
			else if (diff > 0){
				while (diff != 0){
					elevator.STATE = 4;
					ssleep(2);
					diff--;
					elevator.floor--;
				}
			}else if (diff < 0){
				while (diff != 0){
					elevator.STATE = 3;
					ssleep(2);
					diff++;
					elevator.floor++;
				}
			}
			elevator.STATE = movestate;
			printk(KERN_NOTICE "next floor is %d", nextfloor);
			elevator.floor = nextfloor;
			printk(KERN_NOTICE "elevator state is %d\n", elevator.STATE);
	}
		printk(KERN_NOTICE "elevator weight is 0, now ending elevator \n");
		elevator.STATE = 0;
	printqueue();	
	printelevator();
	return 0;
}

//function that determines what the elevators next floor should be
int get_next_floor(int s){

	Passenger *p;
	struct list_head *temp;
	struct list_head *temp2;
	int nextfloor = 0;
	int state;
	int i;
	state = s;
	
	if (state == 3){
		nextfloor = 10; 
		list_for_each(temp, &elevator.list){
			p = list_entry(temp, Passenger, list);
			if (p->dest - elevator.floor < nextfloor - elevator.floor){
				nextfloor = p->dest;
			}
		}
		if (elevator.shouldstop != 1){
			list_for_each(temp2, &passengers.list){
				p = list_entry(temp2, Passenger, list);
				if (p->start > elevator.floor && p->start < nextfloor && p->start < p->dest){
					if (p->pet_type == elevator.pet_type && elevator.weight + p->weight <= 15){
						nextfloor = p->start;
					}
				}
			}
		}
	}else if (state == 4){
		nextfloor = 1;
		list_for_each(temp, &elevator.list){
			p = list_entry(temp, Passenger, list);
			if (elevator.floor - p->dest < elevator.floor - nextfloor){
				nextfloor = p->dest;
			}
		}
		if (elevator.shouldstop != 1){
			list_for_each(temp2, &passengers.list){
				p = list_entry(temp2, Passenger, list);
				if (p->start < elevator.floor && p->start > nextfloor && p->start > p->dest){
					if (p->pet_type == elevator.pet_type && elevator.weight + p->weight <= 15){
						nextfloor = p->start;
					}
				}
			}
		}
	}else if (state == 1 && elevator.shouldstop != 1){
		i = 0;
		list_for_each(temp, &passengers.list){
			p = list_entry(temp, Passenger, list);
			if (nextfloor == 0){
				nextfloor = p->start;
				i = 1;
			}
				printk(KERN_NOTICE "state is idle, looking for passenger, found on floor %d\n", p->start);
				if (abso(p->start - elevator.floor) < abso(nextfloor - elevator.floor)){
					nextfloor = p->start;
					i = 1;
				}
		}
		if (i == 0){
			nextfloor = 1;
		}
	}

	elevator.STATE = state;
	return nextfloor;

}

//function which transfers people from passengers list to elevator list if applicable
void load_elevator(int s){
        int state;
        int i;
        Passenger *p;
        struct list_head *temp;
	struct list_head *dummy;
        int pfloor;

	state = s;

        if (elevator.weight == 0){
	i = 0; //0 if person hasnt beek picked, 1 if they have, used to determine initial direction
	        list_for_each_safe(temp, dummy, &passengers.list){
			p = list_entry(temp, Passenger, list);
			if (p->start == elevator.floor && i == 0){
				elevator.weight += p->weight;
				printk(KERN_NOTICE "Changing pet type\n");
				elevator.pet_type = p->pet_type;
				printk(KERN_NOTICE "pet type changed\n");
				list_move_tail(temp, &elevator.list);
				elevator.length += 1;
				passengers.total_length--;
				if (p->dest - p->start > 0)
					state = 3;
				else
					state = 4;
				//list_del(temp);
				//kfree(p);
				i = 1;
			}
		}
        }
	
        if (state == 3){
                i = 0;
                list_for_each_safe(temp, dummy, &passengers.list){
                        p = list_entry(temp, Passenger, list);
			if (p->start == elevator.floor){
                	        pfloor = p->dest - elevator.floor;
                	        if (p->pet_type == elevator.pet_type && pfloor > 0){
                	                if (elevator.weight + p->weight <= 15){
						elevator.weight += p->weight;
                	                        list_move_tail(temp, &elevator.list);
						elevator.length++;
						passengers.total_length--;
                	                }
                	        }
                	}
		}
		elevator.STATE = state;
        }else if (state == 4){
                i = 0;
                list_for_each_safe(temp, dummy, &passengers.list){
                        p = list_entry(temp, Passenger, list);
			if (p->start == elevator.floor){
                        	pfloor = elevator.floor - p->dest;
        	                if (p->pet_type == elevator.pet_type && pfloor > 0){
        	                        if (elevator.weight + p->weight <= 15){
						elevator.weight += p->weight;
        	                                list_move_tail(temp, &elevator.list);
						elevator.length++;
						passengers.total_length--;
         	                       }
         	               }
         	       }
        	}
		elevator.STATE = state;
	}
	if (elevator.weight == 0)
		elevator.STATE = 1;
}

//called when elevator is stopped, removes people from elevator list
void leave_elevator(){
	int i;
	Passenger *p;
	struct 	list_head *temp;
	struct list_head *dummy;

	//elevator.STATE = 2;
	i = 0;
	list_for_each_safe(temp, dummy, &elevator.list){
		p = list_entry(temp, Passenger, list); 
		if (p->dest == elevator.floor){
			elevator.weight = elevator.weight - p->weight;
			list_del(temp);
			elevator.length--;
			elevator.total+= 1 + p->num_pets;
			kfree(p);	
		}
	}
	if (elevator.weight == 0)
		elevator.pet_type = 0;	
}

//initialize the thread with the lock
void thread_init_parameter(struct thread_parameter *parm){
	static int id = 1;

	parm->id = id++;
	parm->cnt = 0;
	mutex_init(&parm->mutex);

	parm->kthread = kthread_run(thread_run, parm, "elevator thread %d\n", parm->id);
}

//opens the proc file for giving information to user, allocate space for necessary messages
int thread_proc_open(struct inode *sp_inode, struct file *sp_file){
	read_p = 1;

	message = kmalloc(sizeof(char) * ENTRY_SIZE, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
	message2 = kmalloc(sizeof(char) * ENTRY_SIZE, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
	message3 = kmalloc(sizeof(char) * 500, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
	message4 = kmalloc(sizeof(char) * 500, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
	message5 = kmalloc(sizeof(char) * 500, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
	
	finalmessage = kmalloc(sizeof(char) * 2000, __GFP_RECLAIM | __GFP_IO | __GFP_FS);

	if (message == NULL || message2 == NULL || message3 == NULL || finalmessage == NULL){
		printk(KERN_WARNING "Could not allocate memory for proc \n");
		return -ENOMEM;
	}

	return 0;
}

//convert numeric state to character representation 
const char* getState(int s){
	if (s == 0)
		return "OFFLINE";
	else if (s == 1)
		return "IDLE";
	else if (s == 2)
		return "LOADING";
	else if (s == 3)
		return "UP";
	else if (s == 4)
		return "DOWN";

	return "ERROR";
}

//convert numeric pet type to character representation
const char* getPet(int s){
	if (s == 0)
		return "none";
	else if (s == 1)
		return "cat";
	else if (s == 2)
		return "dog";

	return "error";
}

//converts a floor to a string representation as specified in project writeup
const char* getFloors(int s){
	char *str;
	char *waiting;
	int wait;
	char fnuml[2];
	char fnum[1];
	Passenger *p;
	struct list_head *temp;

	str = kmalloc(sizeof(char) * ENTRY_SIZE, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
	waiting = kmalloc(sizeof(char) * 10, __GFP_RECLAIM | __GFP_IO | __GFP_FS);

	if (s == elevator.floor)
		strcpy(str, "[*] Floor ");
	else
		strcpy(str, "[ ] Floor ");

	if (s == 10){
		sprintf(fnuml, "%d", s);	
		strcat(str, fnuml);
	}
	else{	
		sprintf(fnum, " %d", s);	
		strcat(str, fnum);
	}	

	strcat(str, ":  ");
	wait = getWaiting(s);
	if (wait < 10)
		sprintf(waiting, " %d  ", wait);
	else 
		sprintf(waiting, "%d  ", wait);
	strcat(str, waiting);

	list_for_each(temp, &passengers.list){
        	p = list_entry(temp, Passenger, list);
		if(p->start == s){
			strcat(str, " |");
			if(p->pet_type == 1){
				if (p->num_pets == 1)
					strcat(str, " o");	
				else if (p->num_pets == 2)
					strcat(str, " o o");	
				else if (p->num_pets == 3)
					strcat(str, " o o o");	
			}else if(p->pet_type == 2){
				if (p->num_pets == 1)
					strcat(str, " x");	
				else if (p->num_pets == 2)
					strcat(str, " x x");	
				else if (p->num_pets == 3)
					strcat(str, " x x x");	
			}
		}
	}
	return str;
}

//calculate how many people are waiting at a given floor
int getWaiting(int s){
	int number = 0;
	Passenger *p;
	struct list_head *temp;

	list_for_each(temp, &passengers.list){
		p = list_entry(temp, Passenger, list);
		if (p->start ==s){
			number += 1;
			number += p->num_pets;
		}	
	}
	return number;
}

//called when you do a read operation on /proc/elevator, outputs to user message which is stored in finalmessage
ssize_t thread_proc_read(struct file *sp_file, char __user *buf, size_t size, loff_t *offset){
	int len = 0;
	//mutex_lock(&thread1.mutex);;
	sprintf(message, "Elevator state: %s\nElevator animals: %s\nCurrent floor: %d\nNumber of passengers %d\n", getState(elevator.STATE), getPet(elevator.pet_type), elevator.floor, elevator.length);
	sprintf(message2, "Current weight: %d\nNumber of passengers waiting: %d\nNumber passengers serviced: %d\n", elevator.weight, passengers.total_length, elevator.total);
	sprintf(message3, "%s\n%s\n%s\n", getFloors(10), getFloors(9), getFloors(8));
	sprintf(message4, "%s\n%s\n%s\n%s\n", getFloors(7), getFloors(6), getFloors(5), getFloors(4));
	sprintf(message5, "%s\n%s\n%s\n", getFloors(3), getFloors(2), getFloors(1));
	
	sprintf(finalmessage, "%s%s\n\n%s%s%s", message, message2, message3, message4, message5);
	len = strlen(finalmessage);
	
	read_p = !read_p;
	if (read_p){
		//mutex_unlock(&thread1.mutex);
		return 0;
	}
	
	copy_to_user(buf, finalmessage, len);

	*offset = len;

	//mutex_unlock(&thread1.mutex);
	return len;

	//return len;
}

//deallocate all the dynamically allocated messages
int thread_proc_release(struct inode *sp_inode, struct file *sp_file){
	kfree(message);
	kfree(message2);
	kfree(message3);
	kfree(message4);
	kfree(message5);
	kfree(finalmessage);
	return 0;
}

//definition for start_elevator system call
extern long (*STUB_start_elevator)(void);
long start_elevator(void) {
//	while(true){
//		if(mutex_lock_interruptible(&thread1.mutex) == 0){
		mutex_lock(&thread1.mutex);
		printk(KERN_NOTICE "Starting Elevator\n");
			if(elevator.STATE != 0){
				printk(KERN_NOTICE "Elevator already started\n");
				mutex_unlock(&thread1.mutex);
				return 1;
			}
			elevator.STATE = 1;
			elevator.floor = 1;
			elevator.weight = 0;
			elevator.length = 0;
	        	thread_init_parameter(&thread1);
		        if(IS_ERR(thread1.kthread)){
			        printk(KERN_WARNING "error spawning thread\n");
				mutex_unlock(&thread1.mutex);
	                	return PTR_ERR(thread1.kthread);
        		}
	//	}break;
	//}
	mutex_unlock(&thread1.mutex);
	return 0;

}

//definition for stop elevator system call
extern long (*STUB_stop_elevator)(void);
long stop_elevator(void) {
		printk(KERN_NOTICE "Waiting for lock\n");
		//mutex_lock(&thread1.mutex);	
			if(elevator.shouldstop == 1){
				printk(KERN_NOTICE "Elevator already stopping\n");
				mutex_unlock(&thread1.mutex);
				return 1;
			}	
			kthread_stop(thread1.kthread);
			printk(KERN_NOTICE "Stopping Elevator\n");
		//	elevator.STATE = 0;
		
	mutex_unlock(&thread1.mutex);
	return 0;
}

//definition for issue request system call
extern long (*STUB_issue_request)(int,int,int,int);
long issue_request(int num_pets,int pet_type,int start_floor,int destination_floor) {
        Passenger *p;
	if(num_pets <0 || num_pets >3){
		printk(KERN_NOTICE "NUMBER OF PETS OUTSIDE OF RANGE\n");
		return 1;
	}
        if(pet_type <0 || pet_type >2){
                printk(KERN_NOTICE "PET TYPE IS NOT 0,1,2\n");
                return 1;
        }
	if(start_floor <1 || start_floor >10){
		printk(KERN_NOTICE "STARTING FLOOR CHOICE EXCEEDS RANGE\n");
		return 1;
	}
	if(destination_floor <1 || destination_floor >10){
		printk(KERN_NOTICE "DESTINATION FLOOR CHOICE EXCEEDS RANGE\n");
		return 1;
	}
	printk(KERN_NOTICE "Issuing Request\n");
	p = kmalloc(sizeof(Passenger), __GFP_RECLAIM);
	p->pet_type = pet_type;		//1 is cat 2 is dog;
	p->num_pets = num_pets;
	p->start = start_floor;
	p->dest = destination_floor;
	if(pet_type == 2)
	{
		p->weight = 3+(num_pets * 2);	//weight of person and their dog(s)
	}
	else if(pet_type == 1)
	{
		p->weight = 3 + (num_pets);	//weight of person and their cat(s)
	}
	else					//pet type not 1 or 2 might be error
	{
		p->weight = 3;
	}
	mutex_lock(&thread1.mutex);
	list_add_tail(&p->list, &passengers.list);	
	passengers.total_length++;	
	mutex_unlock(&thread1.mutex);

	return 0;
}


//kernel module initiliazation function
static int elevator_init(void) {
	STUB_start_elevator = start_elevator;
	STUB_issue_request = issue_request;
	STUB_stop_elevator = stop_elevator;
	
	elevator.STATE = 0;	//0 is offline 1 is idle 2 is loading 3 is up 4 is down
	elevator.weight = 0;
	elevator.floor = 1;
	elevator.dest = 0;	//0 is no dest
	elevator.total = 0;
	INIT_LIST_HEAD(&elevator.list);
	passengers.total_length = 0;
	INIT_LIST_HEAD(&passengers.list);	

	fops.open = thread_proc_open;
	fops.read = thread_proc_read;
	fops.release = thread_proc_release;

	if (!proc_create(ENTRY_NAME, PERMS, NULL, &fops)){
		printk(KERN_WARNING "proc initilization error\n");
		remove_proc_entry(ENTRY_NAME, NULL);
		return -ENOMEM;
	}	

	return 0;
}
module_init(elevator_init);

//kernel module removal function
static void elevator_exit(void) {
	STUB_start_elevator = NULL;
	STUB_issue_request = NULL;
	STUB_stop_elevator = NULL;

	remove_proc_entry(ENTRY_NAME, NULL);
	printk(KERN_NOTICE "destroying mutex\n");
	mutex_destroy(&thread1.mutex);

	printk(KERN_NOTICE "Proc removed, mutex destroyed,  removing Module\n");
}
module_exit(elevator_exit);

//print elevator, used only for debugging and ease of development
void printelevator(void){
	int i;
	Passenger *p;
	struct list_head *temp;
	
	printk(KERN_NOTICE "printing Elevator list\n");
	i = 0;
	list_for_each(temp, &elevator.list){
		p = list_entry(temp, Passenger, list);
		printk(KERN_NOTICE "elevator : %d : Num_pets: %d\n Pet_type: %d\n Start_floor: %d\n Destination_floor: %d\n Weight: %d\n",i,p->num_pets,p->pet_type,p->start,p->dest,p->weight);
		i++;
	}
}

//print queue, used only for development 
void printqueue(void){
//Passenger *iterator = passengers.head;
//printk(KERN_NOTICE "Num_pets: %d\n Pet_type: %d\n Start_floor: %d\n Destination_floor: %d\n Weight: %d\n",iterator->num_pets,iterator->pet_type,iterator->start,p->dest,p->weight);
	int i;
	Passenger *p;
	struct list_head *temp;

	printk(KERN_NOTICE "printing passengers list\n");
	i = 0;
	list_for_each(temp, &passengers.list){
		p = list_entry(temp, Passenger, list);
		printk(KERN_NOTICE "queue : %d : Num_pets: %d\n Pet_type: %d\n Start_floor: %d\n Destination_floor: %d\n Weight: %d\n",i,p->num_pets,p->pet_type,p->start,p->dest,p->weight);
		i++;
		//if(i == 4)
		//	break;
	}
}
