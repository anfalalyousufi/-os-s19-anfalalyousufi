
/* anfal alyousufi
 * boot1main
 * ASSIGNMENT OVERVIEW:
 * 	 *  - Fill out and complete the TASKS 1-6 below to build the 2nd stage bootloader.
 * 	  *  - It is recommended to start on boot0 and boot1 first.
 */

#include <boot1lib.h>

mboot_info_t * parse_e820(bios_smap_t *smap);
uint32_t load_kernel(uint32_t dkernel);

extern void exec_kernel(uint32_t, mboot_info_t *);

mboot_info_t mboot_info =
        { .flags = (1 << 6), };


/* boot1main
 *  
 *  uint32_t dev: device id
 *  mbr_t * mbr: pointer to the loaded boot sector (i.e. boot0)
 *  bios_smap_t *smap: the detected physical memory map
 *      
 *      
 */      


 
 void
 boot1main (uint32_t dev, mbr_t * mbr, bios_smap_t *smap)
      {
        /* Roll sets the row we will print on for the VGA, this function is defined in the provided, boot1lib files. */
	   roll(3);
	/* Since we can't use the standard C library yet, we have to directly print to the VGA to get printed output.*/
           putline("Start boot1 main ...");

/* TASK 1:
 * - Loop through the parition table and find the first bootable partition.
 * - Call panic("") with an error message if no partition is bootable.
 *                            
 * Hint 1: The partition table is stored in the first boot sector after the boot0 code.
 * Hint 2: We passed in a reference to the MBR, access it like this mbr->partition[i].bootable
 * Hint 3: If the bootable field of a partition is equal to BOOTABLE_PARTITION, then it is bootable.
 * Hint 4: An MBR-style parition table always has 4 entries.
 * Hint 5: The Logical block address (LBA) of the bootable parition is stored in the first_lba field of the partition table entry.
 */
  /* parse the memory map we extracted from the bios on the assembly side*/
     parse_e820 (smap);

     int x;
     //int y;
     int z=0;
     //use for loop, max
     for(x=0; x<=4; x++)
{
	if(mbr->partition[x].bootable == BOOTABLE_PARTITION) //Hint 2&3
		//break
		//y= mbr->partition[x].first_lba;
	{
		z=1;
	        break;
	}
}

	if (z=0)
	 panic ("No partition is bootable");


/*Task 2:
 * 	
 *- Execute the load_kernel function, and supply it the first LBA of the bootable partition.
 *- Use the return of load_kernel and a pointer to the mboot_info variable to call exec_kernel.
 */
        uint32_t header = load_kernel(mbr->partition[x].first_lba);
	//uint32_t header = load_kernel(y);
	//&mboot_info=mboot_info_t* mboot_info_pointer;
	exec_kernel(header,&mboot_info);//call exec_kernel



	/* exec_kernel should never return */
	 panic ("Fail to load kernel.");
}



#define ELFHDR          ((elfhdr *) 0x20000)

uint32_t
load_kernel(uint32_t dkernel)
{
	    

	 // load kernel from the beginning of the first bootable partition
	         proghdr *ph, *eph;

		 readsection((uint32_t) ELFHDR, SECTOR_SIZE * 8, 0, dkernel);


	 // is this a valid ELF?
	         if (ELFHDR->e_magic != ELF_MAGIC)
			   panic ("Kernel is not a valid elf.");
	 // load each program segment (ignores ph flags)
	        ph = (proghdr *) ((uint8_t *) ELFHDR + ELFHDR->e_phoff);
		eph = ph + ELFHDR->e_phnum;
			for (; ph < eph; ph++)
			      {
				      readsection(ph->p_va, ph->p_memsz, ph->p_offset, dkernel);							        }

				        return (ELFHDR->e_entry & 0xFFFFFF);
}

mboot_info_t *
parse_e820 (bios_smap_t *smap)
{
	        bios_smap_t *p;
		uint32_t mmap_len;
		p = smap;
		mmap_len = 0;
		putline ("* E820 Memory Map *");
		while (p->base_addr != 0 || p->length != 0 || p->type != 0)								        {
	                puti (p->base_addr);
			p ++;
			mmap_len += sizeof(bios_smap_t);
																        }
		mboot_info.mmap_length = mmap_len;
		mboot_info.mmap_addr = (uint32_t) smap;
		return &mboot_info;
}

