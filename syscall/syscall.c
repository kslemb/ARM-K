#include "main.h"
#include "device.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "../uart/uart.h"

#include "../generic/console.h"

/* Use 'c' alternative of sp */
register char *stack_ptr asm ("sp");



int _open_r (char *path, int flags, int mode)
{
	int f_handle;
    DEVICE_PTR dev_ptr;

//	printf ("Search for %s in Device List\n\r", path);

    for (f_handle = 0; f_handle < Device_List_Count (); f_handle++) {
		dev_ptr = Device_List[f_handle];
		if(dev_ptr == 0) {
			continue;
		}

//		printf ("%d --> %s\n\r", f_handle, dev_ptr->name);

		if (__builtin_strcmp (path, dev_ptr->name) == 0) {
//			printf ("%s handle is %x\n\r", dev_ptr->name, f_handle);
		    return f_handle;
		}
    }
//	printf ("Device %s not found\n\r", path);
	return	-1;
}

caddr_t _sbrk_r (int incr)
{
	extern char end asm ("end");  /* Defined by the linker.  */
	static char *heap_end;
	char *prev_heap_end;

	/* Heap starts from the end of the .bss section marked in ld as _end*/
	if (heap_end == 0)
		heap_end = &end;
	/* Save current heap end befor add new amount to the heap*/
	prev_heap_end = heap_end;

	/* If new chunk is greater then stack pointer set error condition*/
	if (heap_end + incr > stack_ptr) {
		errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

int _write_r (struct _reent *r, int fd, char *ptr, int len)
{
	int bytesWritten = 0;
	DEVICE_PTR dev_ptr;

	dev_ptr = Device_List[fd];
	if(dev_ptr != 0) {
		if(dev_ptr->write != 0)
			bytesWritten = (*dev_ptr->write)((unsigned char *)ptr, len);
	 }
	else {
		errno = EBADF;
		bytesWritten = -1;
	}

	return bytesWritten;
}
