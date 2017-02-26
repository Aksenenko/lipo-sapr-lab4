/*
	Multiplicity operations header file
*/

#ifndef _yla_multiplicity_h
#define _yla_multiplicity_h

#include "yla_cop.h"
#include "yla_type.h"
#include "yla_test_gencode.h"
#include "yla_compliance_table.h"

void put_num_in_multiplicity(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter, yla_int_type global_var[]);

void put_multiplicity_union(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter, yla_int_type global_var[]);

void put_multiplicity_intersect(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter, yla_int_type global_var[], yla_int_type num_in_mult_addr);

#endif
