#include <exports.h>

#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <bits/swi.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <arm/reg.h>

#define SWI_VECTOR_ADDR 0x00000008
#define IRQ_VECTOR_ADDR 0x00000018
#define PC_OFFSET       8

#define LDR_PC_BASE  0xe51ff000 /* ldr pc, [pc, #imm12] */
#define LDR_U_FLAG   0x00800000
#define LDR_IMM_MASK 0x00000fff
#define LDR_PC_NEXT  0xe51ff004 /* ldr pc, [pc, #-4] */


//Global Variables
uint32_t global_data;
unsigned long old_swi_inst0;
unsigned long old_swi_inst1;
unsigned long old_irq_inst0;
unsigned long old_irq_inst1;
unsigned long* old_swi_handler;
unsigned long* old_irq_handler;
//volatile size_t systemTime;
volatile size_t current_time;
volatile size_t start_time;
volatile unsigned int old_icmr;
volatile unsigned int old_iclr;
volatile unsigned int old_oier;
volatile unsigned int old_osmr;


//Function Definitions
void install_swi_handler(void);
void install_irq_handler(void);
unsigned long* getAddr(unsigned int vector_addr);
extern int toUSER(int argc, char *argv[]);
extern void handleSWI(void);
extern void handleIRQ(void);
extern void exit_kernel(int status);
ssize_t read(int fd, void* buf, size_t count);
ssize_t write(int fd, const void* buf, size_t count);
size_t time(void);
void sleep(size_t time);
void setup_timer(void);
void setup_irq(void);
void restore_uboot(void);

int kmain(int argc, char** argv, uint32_t table)
{
	app_startup(); /* bss is valid after this point */
	global_data = table;

	install_swi_handler();
	install_irq_handler();
	
	//Interrupt setup
	setup_irq();

	//Oscillator setup
	setup_timer();	


	printf("MAKING USER SWITCH\n");
	int status = toUSER(argc, argv);

	// restore uboot
	restore_uboot();
	
	return status;
}



void install_swi_handler()
{
	old_swi_handler = getAddr(SWI_VECTOR_ADDR);
	
	/* Save the first two instructions so they may be restored later. */
	old_swi_inst0 = *(old_swi_handler  );
	old_swi_inst1 = *(old_swi_handler+1);

	/* Redirect U-Boot's SWI handler to ours. */
	*(old_swi_handler  ) = LDR_PC_NEXT;
	*(old_swi_handler+1) = (unsigned long) &handleSWI;
}

void install_irq_handler()
{
	old_irq_handler = getAddr(IRQ_VECTOR_ADDR);
	
	/* Save the first two instructions so they may be restored later. */
	old_irq_inst0 = *(old_irq_handler  );
	old_irq_inst1 = *(old_irq_handler+1);

	/* Redirect U-Boot's IRQ handler to ours. */
	*(old_irq_handler  ) = LDR_PC_NEXT;
	*(old_irq_handler+1) = (unsigned long) &handleIRQ;
}







// interrupt setup
void setup_irq(void) {
	old_icmr = reg_read(INT_ICMR_ADDR);
	old_iclr = reg_read(INT_ICLR_ADDR);
	reg_write(INT_ICMR_ADDR, (0x1 << INT_OSTMR_0));
	reg_write(INT_ICLR_ADDR, (0x0 << INT_OSTMR_0));
}


// timer setup
void setup_timer(void) {
	old_oier = reg_read(OSTMR_OIER_ADDR);
	old_osmr = reg_read(OSTMR_OSMR_ADDR(0));

	reg_clear(OSTMR_OIER_ADDR, OSTMR_OIER_E1 | OSTMR_OIER_E2 | OSTMR_OIER_E3);
	reg_write(OSTMR_OIER_ADDR, OSTMR_OIER_E0);

	
	reg_write(OSTMR_OSCR_ADDR, 0x0);
	start_time = reg_read(OSTMR_OSCR_ADDR);
	reg_write(OSTMR_OSMR_ADDR(0), current_time + (OSTMR_FREQ/100));
	

//	systemTime = 0;
//	reg_write(OSTMR_OSCR_ADDR, 0x0);
//	reg_write(OSTMR_OSMR_ADDR(0), OSTMR_FREQ/100); 
}


// restore uboot
void restore_uboot(void) {
	/* Restore the SWI handler */
	*(old_swi_handler  ) = old_swi_inst0;
    *(old_swi_handler+1) = old_swi_inst1;
	
	/* Restore the IRQ handler */
	*(old_irq_handler  ) = old_irq_inst0;
    *(old_irq_handler+1) = old_irq_inst1;
	
	/* Restore ICMR, ICLR, and OIER */
	reg_write(INT_ICMR_ADDR, old_icmr);
	reg_write(INT_ICLR_ADDR, old_iclr);
	reg_write(OSTMR_OIER_ADDR, old_oier);
	reg_write(OSTMR_OSMR_ADDR(0), old_osmr);
}








// IRQ interrupt
void interrupt(void){

	current_time = reg_read(OSTMR_OSCR_ADDR);
	reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
	reg_write(OSTMR_OSMR_ADDR(0), current_time + (OSTMR_FREQ/100));
	
/*
	volatile size_t curTime = reg_read(OSTMR_OSCR_ADDR);
	systemTime += 10;
	reg_write(OSTMR_OSMR_ADDR(0), curTime + (OSTMR_FREQ/100)); 
	reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
*/
}



// helper function:
// getAddr returns the address of the handler given the swi vector address
unsigned long* getAddr(unsigned int vector_addr)
{
	unsigned long vector = *(unsigned long *)vector_addr;
	long offset;

	/* Check if the SWI/IRQ vector addr contains a "ldr pc, [pc, #imm12] instruction. */
	if ((vector & ~(LDR_U_FLAG | LDR_IMM_MASK)) != LDR_PC_BASE)
	{
		printf("Kernel panic: unrecognized vector: %08lx\n", vector);
		exit_kernel(0x0badc0de);
	}

	/* Determine the literal offset. */
	if (vector & LDR_U_FLAG) {
		offset = vector & LDR_IMM_MASK;
	}
	else {
		offset = -(vector & LDR_IMM_MASK);
	}
	
	/* Find the location of U-Boot's SWI/IRQ handler. */
	return *(unsigned long **)(vector_addr + PC_OFFSET + offset);
}





/*
 * swi dispatcher takes two parameters: swi number and a pointer to the 
 * corresponding argument 
 */ 
int my_swi_dispatcher(int swi_number, int* args_ptr)
{
	// return value
	int result = 0;
	
	switch (swi_number)

	{
		case EXIT_SWI:
			// exit takes one parameter: int status
			exit_kernel((int)args_ptr[0]);
			break;
			
		case READ_SWI:
			// read takes three parameters: int fd, void* buf, size_t count
			result = (int) read((int)args_ptr[0], (void *)args_ptr[1], (size_t)args_ptr[2]);
			break;
			
		case WRITE_SWI:
			// write takes three parameters: int fd, void* buf, size_t count
			result = (int) write((int)args_ptr[0], (void *)args_ptr[1], (size_t)args_ptr[2]);
			break;

		case TIME_SWI:
			result = (int) time();
			break;

		case SLEEP_SWI:
			sleep(((size_t) args_ptr[0]));
			break;
		
		default:
			printf("swi dispatcher default \n");
			exit_kernel(0x0badc0de);
	}
	return result;
}



//system calls

void sleep(size_t duration)
{
	/* Enable IRQ */
	asm("mrs r12, cpsr");
	asm("bic r12, r12, #0x80");
	asm("msr cpsr, r12");

	const size_t endTime = current_time + duration * (OSTMR_FREQ/1000);
	while (current_time < endTime);

//	const size_t endTime = systemTime + duration * (OSTMR_FREQ/1000);
//	while(systemTime < endTime);
}

size_t time()
{
	//return systemTime;
	return (current_time - start_time) / (OSTMR_FREQ/1000);
}


/* Read bytes from STDIN, store them in buf and echo them to STDOUT */
ssize_t read(int fd, void *buf, size_t count)
{

	/* U-Boot can only write to stdout */
	if (fd != STDIN_FILENO) {
		return -EBADF;
	}
	// Check buffer's memory range to make sure it's inside readable memory
	if (((unsigned int)buf  <  (unsigned int)0xa0000000) || (((unsigned int)(((unsigned int*) buf) + count)) > (unsigned int)0xa3ffffff)) {
	return -EFAULT;
	}
	ssize_t num_read = 0;
	/*
	 * the buffer has to be null terminated, so last char must be '\0'
	 * hence at most can read in count - 1 characters from stdin
	 */
	if (count == 0) return num_read;
	while (((size_t)num_read) < count )
	{
		int character = getc();
		switch (character)
		{
			/* EOT character */
			case 4:
				return num_read;
			
			/* Backspace or Delete */
			case 8:
			case 127:
				if (num_read > 0)
				{
					num_read --;
					puts("\b \b");
				}
				
			
			/* Newline or carriage return*/
			case 10:
			case 13:
				((char*) buf)[num_read] = '\n';
				putc('\n');
				num_read ++;
				return num_read;
			
			/* Normal characters */
			default:
				((char*) buf)[num_read] = character;
				putc(character);
				num_read++;
		}
	}
	
	
	return num_read;
}



/* Write from a buffer to STDOUT */
ssize_t write(int fd, const void *buf, size_t count)
{
	// U-Boot can only write to stdout
	if (fd != STDOUT_FILENO) {
	  return -EBADF;
	}
	
	unsigned int bufLow = (unsigned int) buf;
	unsigned int bufHi = (unsigned int) (((unsigned int*)buf) + count);
	// Check buffer's memory range to make sure it's inside readable memory
	if (!((  ((bufLow > (unsigned int) 0x0) && (bufHi < (unsigned int) 0x00ffffff)) ||
		(bufLow > (unsigned int) 0xa0000000 && (bufHi < (unsigned int) 0xa3ffffff))	))){
	  return -EFAULT;
	}
	
	ssize_t wri_num = 0;
	
	int whileloopCounter = 0;
	while (((size_t)wri_num) < count)
	{	
		whileloopCounter ++;
		// make sure buffer is not empty
		if (((char*)buf)[wri_num] == '\0') break;
		else{
			putc(((char*)buf)[wri_num]);
			wri_num ++;
		}
	}
	return wri_num;
}
