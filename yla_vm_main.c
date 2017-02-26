/*
    Virtual Machine main

    This file is part of YLA VM (Yet another Language for Academic purpose: Virtual Machine).

    YLA VM is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include "yla_vm.h"
#include "yla_cop.h"
#include "yla_type.h"
#include "yla_test.h"
#include "yla_test_gencode.c"	// >_<
#include "yla_compliance_table.h"
#include "yla_multiplicity.h"

int main(int argc, char argv[])
{
	// Вот здесь я даю максимальную свободу действий программисту ВМ.
	// Поэтому здесь есть лишь один return.
	return 0;
}

void mult_union()
{
	yla_int_type stack_size = 30;
	yla_int_type vartable_size = 60;
	yla_int_type prg_size = 1440;
   yla_cop_type prg[HEADER_SIZE + prg_size];
   yla_cop_type *prog_ptr = prg;
	
	yla_int_type prog_counter = 0;
	yla_int_type *prog_counter_ptr = prog_counter;
	
	compliance_table subprog_addr;
	compliance_table_init(&subprog_addr, 40);
	
	// Global variables addresses
	yla_int_type global_var[8];
	// Mults counts
	global_var[0] = 0x000d;		// M1
	global_var[1] = 0x000e;		// M2
	global_var[2] = 0x000f;		// Mres
	// Local variables for subprogram 'multiplicity_intersection'
	// Yes, this is logically incorrect, but... but that is.
	global_var[3] = 0x0010;		//	local_dx 
	global_var[4] = 0x0011;		// local_cx
	global_var[5] = 0x0012;		// local_ret
	// Offset global variable address
	global_var[6] = 0x0013;	
	// Local variables for subprogram 'num_in_multiplicity'
	// And yes, this is logically incorrect, i know
	global_var[7] = 0x0014;		// cx
	
	// Answers addresses in vartable for tester
	yla_int_type answer1 = 0x0021;
	yla_int_type answer2 = 0x0022;
	yla_int_type answer3 = 0x0023;
	
	put_header(&prog_ptr, stack_size, vartable_size, prg_size);
	
	// Multiplicity 1: {}, count: 0
	//	[count]
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	
	// Multiplicity 2: {7,8,9}, count: 3
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0007);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0008);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0009);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x000a);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x000e);				prog_counter += 2;
	//	[count]
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0003);				prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	
	// Call mult_union
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0400);				prog_counter += 2;	/*mult_union(400)*/
	
	// Multiplicity out: {7,8,9}, count: 3
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	// Save answer to vartable
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, answer1);			prog_counter += 2;
	// Set to zero result mult count
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	
	// Multiplicity 2: {}, count: 0
	//	[count]
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	
	// Call mult_union
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0400);				prog_counter += 2;	/*mult_union(400)*/
	
	// Multiplicity out: {7,8,9}, count: 3
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	// Save answer to vartable
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, answer2);			prog_counter += 2;
	// Set to zero result mult count
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	
	// Multiplicity 2: {12,15}, count: 2
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x000c);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x000f);				prog_counter += 2;
	// [count]
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0002);				prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	
	// Call mult_union
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0400);				prog_counter += 2;	/*mult_union(400)*/
	
	// Multiplicity out: {7,8,9,12,15}, count: 5
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	// Save answer to vartable
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, answer3);			prog_counter += 2;
	// Set to zero result mult count
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	
	put_commd(&prog_ptr, CHALT);				prog_counter++;
	
	put_multiplicity_union(&prog_ptr, 0x0400, &subprog_addr, &prog_counter, global_var);
	
   yla_vm vm;
	
	yla_cop_type *prog_linker_ptr = prg;
	int link_state = link_program(prog_linker_ptr, prog_counter, &subprog_addr);
	
	yla_vm_init(&vm, &prg, (prg_size + HEADER_SIZE));
	
	if (link_state == 1) {
		yla_vm_run(&vm);
	} else if (link_state == -8) {
		vm.last_error = YLA_VM_LINKER_ERROR_TABLE_EMPTY;
	} else if (link_state == -9) {
		vm.last_error = YLA_VM_LINKER_ERROR_ADDRESS_NOT_FOUND;
	}
	
	yla_vm_done(&vm);
	
}

void num_in_mult()
{
	yla_int_type stack_size = 30;
	yla_int_type vartable_size = 60;
	yla_int_type prg_size = 1440;
   yla_cop_type prg[HEADER_SIZE + prg_size];
   yla_cop_type *prog_ptr = prg;
	
	yla_int_type prog_counter = 0;
	yla_int_type *prog_counter_ptr = prog_counter;
	
	compliance_table subprog_addr;
	compliance_table_init(&subprog_addr, 20);
	
	// Global variables addresses
	yla_int_type global_var[5];
	// Mults counts
	global_var[0] = 0x000d;		// M1
	global_var[1] = 0x000e;		// M2
	global_var[2] = 0x000f;		// Mres
	// Local variables for subprogram 'multiplicity_intersection'
	// Yes, this is logically incorrect, but... but that is.
	global_var[3] = 0x0010;		//	local_dx 
	global_var[4] = 0x0011;		// local_cx
	global_var[5] = 0x0012;		// local_ret
	// Offset global variable address
	global_var[6] = 0x0013;	
	// Local variables for subprogram 'num_in_multiplicity'
	// And yes, this is logically incorrect, i know
	global_var[7] = 0x0014;		// cx
	
	// Answer addreses in vartable
	yla_int_type answer1 = 0x0021;
	yla_int_type answer2 = 0x0022;
	
	put_header(&prog_ptr, stack_size, vartable_size, prg_size);
	// Multiplicity 1: {7,8,9,10,14}, count: 5
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0007);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0008);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0009);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x000a);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x000e);				prog_counter += 2;
	//
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0005);				prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;	// M1
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	
	// Need to find value: 8
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0008);				prog_counter += 2;
	
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0300);				prog_counter += 2;	/*num_in_mult(300)*/
	
	// Save result in answer 1
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, answer1);			prog_counter += 2;
	// Delete previous subprogram call address
	put_commd(&prog_ptr, CSTK);				prog_counter++;
	put_value(&prog_ptr, 0x0001);				prog_counter += 2;
	
	// Need to find value: 4
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0004);				prog_counter += 2;
	
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0300);				prog_counter += 2;	/*num_in_mult(300)*/
	
	// Save result in answer 2
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, answer2);			prog_counter += 2;
	// Delete previous subprogram call address
	put_commd(&prog_ptr, CSTK);				prog_counter++;
	put_value(&prog_ptr, 0x0001);				prog_counter += 2;
	
	put_commd(&prog_ptr, CHALT);				prog_counter++;
	
	put_num_in_multiplicity(&prog_ptr, 0x0300, &subprog_addr, &prog_counter, global_var);
	
   yla_vm vm;
	
	yla_cop_type *prog_linker_ptr = prg;
	int link_state = link_program(prog_linker_ptr, prog_counter, &subprog_addr);
	
	yla_vm_init(&vm, &prg, (prg_size + HEADER_SIZE));
	
	if (link_state == 1) {
		yla_vm_run(&vm);
	} else if (link_state == -8) {
		vm.last_error = YLA_VM_LINKER_ERROR_TABLE_EMPTY;
	} else if (link_state == -9) {
		vm.last_error = YLA_VM_LINKER_ERROR_ADDRESS_NOT_FOUND;
	}
	
	yla_vm_done(&vm);
}

void mult_intersect()
{
	yla_int_type stack_size = 30;
	yla_int_type vartable_size = 60;
	yla_int_type prg_size = 1440;
   yla_cop_type prg[header_size + prg_size];
   yla_cop_type *prog_ptr = prg;
	
	yla_int_type prog_counter = 0;
	yla_int_type *prog_counter_ptr = prog_counter;
	
	compliance_table subprog_addr;
	compliance_table_init(&subprog_addr, 40);
	
	// global variables addresses
	yla_int_type global_var[5];
	// mults counts
	global_var[0] = 0x000d;		// m1
	global_var[1] = 0x000e;		// m2
	global_var[2] = 0x000f;		// mres
	// local variables for subprogram 'multiplicity_intersection'
	// yes, this is logically incorrect, but... but that is.
	global_var[3] = 0x0010;		//	local_dx 
	global_var[4] = 0x0011;		// local_cx
	global_var[5] = 0x0012;		// local_ret
	// offset global variable address
	global_var[6] = 0x0013;	
	// local variables for subprogram 'num_in_multiplicity'
	// and yes, this is logically incorrect, i know
	global_var[7] = 0x0014;		// cx
	
	// answers addresses in vartable for tester
	yla_int_type answer1 = 0x0021;
	yla_int_type answer2 = 0x0022;
	yla_int_type answer3 = 0x0023;
	
	put_header(&prog_ptr, stack_size, vartable_size, prg_size);
	// multiplicity 1: {7,8,9,10,14}, count: 5
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0007);				prog_counter += 2;
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0008);				prog_counter += 2;
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0009);				prog_counter += 2;
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x000a);				prog_counter += 2;
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x000e);				prog_counter += 2;
	//
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0005);				prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;	// m1
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	
	// multiplicity 2: {9,10,15}, count: 3
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0009);				prog_counter += 2;
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x000a);				prog_counter += 2;
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x000f);				prog_counter += 2;
	//
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0003);				prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;	// m2
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	
	put_commd(&prog_ptr, calo);				prog_counter++;
	put_value(&prog_ptr, 0x0200);				prog_counter += 2;	/*mult_intersect(200)*/
	
	// multiplicity out: {9,10}, count: 2
	// save result mult count to first mult count
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	// save answer to vartable
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, answer1);			prog_counter += 2;
	// set to zero result mult count
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;	// mres
	
	// multiplicity 2: {}, count: 0
	// [count]
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	
	put_commd(&prog_ptr, calo);				prog_counter++;
	put_value(&prog_ptr, 0x0200);				prog_counter += 2;	/*mult_intersect(200)*/
	
	// multiplicity out: {}, count: 0
	// save result mult count to first mult count
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	// save answer to vartable
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, answer2);			prog_counter += 2;
	// set to zero result mult count
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;	// mres
	
	// multiplicity 1: {5,6}, count: 2
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0009);				prog_counter += 2;
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x000a);				prog_counter += 2;
	// [count]
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0002);				prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;	// m1
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	
	// multiplicity 2: {}, count: 0
	// [count]
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;	// m2
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	
	put_commd(&prog_ptr, calo);				prog_counter++;
	put_value(&prog_ptr, 0x0200);				prog_counter += 2;	/*mult_intersect(200)*/
	
	// multiplicity out: {}, count: 0
	// save result mult count to first mult count
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	// save answer to vartable
	put_commd(&prog_ptr, cload);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, answer3);			prog_counter += 2;
	// set to zero result mult count
	put_commd(&prog_ptr, cpush);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, csave);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;	// mres
	
	put_commd(&prog_ptr, chalt);				prog_counter++;
	
	put_multiplicity_intersect(&prog_ptr, 0x0200, &subprog_addr, &prog_counter, global_var, 0x0300);
	put_num_in_multiplicity(&prog_ptr, 0x0300, &subprog_addr, &prog_counter, global_var);
	
   yla_vm vm;
	
	yla_cop_type *prog_linker_ptr = prg;
	int link_state = link_program(prog_linker_ptr, prog_counter, &subprog_addr);
	
	yla_vm_init(&vm, &prg, (prg_size + header_size));
	
	if (link_state == 1) {
		ylatest_assert_true(yla_vm_run(&vm), "normal");
	} else if (link_state == -8) {
		vm.last_error = yla_vm_linker_error_table_empty;
	} else if (link_state == -9) {
		vm.last_error = yla_vm_linker_error_address_not_found;
	}
	
	yla_vm_done(&vm);
}
