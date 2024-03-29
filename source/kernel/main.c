#include <exports.h>

#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <bits/swi.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <arm/reg.h>


//Global Variables
uint32_t global_data;
unsigned int old_swi_contents_1;
unsigned int old_swi_contents_2;
unsigned int old_swi_contents_3;
unsigned int old_swi_contents_4;
unsigned int* uboot_swi_addr;
unsigned int* uboot_swi;
unsigned int* uboot_irq;
unsigned int* uboot_irq_addr;
unsigned int svc_link_register;
unsigned int svc_stack_register;

//Function Definitions
extern void toUSER(int argc, char *argv[]);
extern void handleSWI(void);
extern void handleIRQ(void);
void exit(int status);
ssize_t read(int fd, void* buf, size_t count);
ssize_t write(int fd, const void* buf, size_t count);

int kmain(int argc, char** argv, uint32_t table)
{
	unsigned int swi_vector_contents = *((int*)0x08);
	unsigned int ldr_opcode = 0xe59FF000;

	unsigned int irq_vector_contents = *((int*)0x18);

	unsigned int slr = svc_link_register;
	unsigned int ssr = svc_stack_register;

	app_startup(); /* bss is valid after this point */
	global_data = table;

	//Interrupt setup
	reg_write(INT_ICMR_ADDR, (0x1 << 26));
	reg_write(INT_ICLR_ADDR, (0x0<< 26));

	//Oscillator setup
	reg_write(OSTMR_OIER_ADDR, OSTMR_OIER_E0);
	reg_write(OSTMR_OSMR_ADDR(0), OSTMR_FREQ);	

	svc_link_register = slr;
	svc_stack_register = ssr;

  if ((swi_vector_contents & 0xFFFFF000) ^ ldr_opcode)
    {
      printf("Instruction was not recognized.\n");
      exit(0x0badc0de);
    }
  else
    {
      int offset = swi_vector_contents ^ ldr_opcode;
      uboot_swi = (unsigned int*) (0x10 + offset);
    }

  if ((irq_vector_contents & 0xFFFFF000) ^ ldr_opcode)
    {
      printf("(IRQ) Instruction was not recognized.\n");
      exit(0x0badc0de);
    }
  else
    {
      int offset = irq_vector_contents ^ ldr_opcode;
      uboot_irq = (unsigned int*) (0x20 + offset);
    }

	
	uboot_swi_addr = (unsigned int *)(*uboot_swi);

	uboot_irq_addr = (unsigned int *)(*uboot_irq);

  unsigned int new_instruction1 = 0xe51FF004;
  unsigned int new_instruction2 = (unsigned int) &handleSWI;

  unsigned int new_instruction3 = (unsigned int) &handleIRQ;

  /* Modify uboot_swi_addr to transfer control, store asm to restore later */
  old_swi_contents_1 = *uboot_swi_addr;
  old_swi_contents_2 = *(uboot_swi_addr + 1);

  old_swi_contents_3 = *uboot_irq_addr;
  old_swi_contents_4 = *(uboot_irq_addr + 1);



  *uboot_swi_addr = new_instruction1;
  *(uboot_swi_addr + 1) = new_instruction2;

  *uboot_irq_addr = new_instruction1;
  *(uboot_irq_addr + 1) = new_instruction3;

	printf("MAKING USER SWITCH\n");

	reg_write(OSTMR_OSCR_ADDR, 0x0);

	toUSER(argc, argv);

	return 0;
}

/* Exit the kernel */
void exit(int status)
{
 *uboot_swi_addr = old_swi_contents_1;
 *(uboot_swi_addr + 1) = old_swi_contents_2; 


 asm
	(	
		"ldr r11, =svc_stack_register \n\t"
		"ldr sp, [r11] \n\t"
		"ldmfd sp!, {r1-r12} \n\t"
		"ldr r11, =svc_link_register \n\t"
		"ldr lr, [r11] \n\t"
		"mov pc, lr"	
	);

}

void temp_int(void){

	printf("INTERRUPT :)\n");

	reg_write(OSTMR_OSCR_ADDR, 0x0);
	reg_write(OSTMR_OSSR_ADDR, 0x1);

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
			exit((int)args_ptr[0]);
			break;
			
		case READ_SWI:
			// read takes three parameters: int fd, void* buf, size_t count
			result = (int) read((int)args_ptr[0], (void *)args_ptr[1], (size_t)args_ptr[2]);
			break;
			
		case WRITE_SWI:
			// write takes three parameters: int fd, void* buf, size_t count
			result = (int) write((int)args_ptr[0], (void *)args_ptr[1], (size_t)args_ptr[2]);
			break;
		
		default:
			exit(0x0badc0de);
	}
	return result;
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
