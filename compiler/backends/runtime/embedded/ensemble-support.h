
/**************/
/** libptm.h **/
/**************/

#ifndef LIBPTM_H
#define LIBPTM_H

#define PAGE_SIZE 4096

void set_root_privileges(int flag);
void get_locked_page(void **logical_addr, unsigned long *physical_addr);
void kickoff(int tmNo, char *imageName, unsigned long bootPage , int procNo,
	     char **base_ptr);

#endif /* !LIBPTM_H */
