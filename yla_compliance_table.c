/*
	Virtual Machine code compliance table
*/

#include "yla_compliance_table.h"

void compliance_table_init(compliance_table* array, size_t size)
{
	array->size = size;
	array->count = 0;
	array->high_byte = calloc(array->size, sizeof(yla_cop_type));
	array->low_byte = calloc(array->size, sizeof(yla_cop_type));
	array->mark = calloc(array->size, sizeof(yla_cop_type));
}

yla_int_type compliance_table_get_addr(compliance_table* array, yla_int_type mark, size_t *addr)
{
	if (array->count == 0) {
		return -8;
	}

	yla_int_type i = 0;
	do
	{
		if (array->mark[i++] == mark) {
			*addr = (array->high_byte[i-1] << 8) + array->low_byte[i-1];
			return 1;
		}
	} while(i != array->count);
	
	return -9;
}

void compliance_table_set_addr(compliance_table* array, yla_int_type mark, size_t addr)
{
	addr++;	// vm->pc has numeration from 0
	array->high_byte[array->count] = addr >> 8;
	array->low_byte[array->count] = addr - (array->high_byte[array->count] << 8);
	array->mark[array->count] = mark;
	array->count++;
}

void compliance_table_done(compliance_table* array)
{
	array->size = 0;
	array->count = 0;
	free(array->high_byte);
	free(array->low_byte);
	free(array->mark);
}
