/*
    Virtual machine Test #1

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
#include "yla_test.h"
#include "yla_test_gencode.h"
#include "yla_multiplicity.h"

static int test_gencode()
{
    yla_cop_type buf[100];
    yla_cop_type *ptr = buf;
    
    put_commd(&ptr, 0x12);
    put_value(&ptr, 0x3456);
    put_commd(&ptr, 0x78);
    put_commd(&ptr, 0x9a);

    YLATEST_ASSERT_TRUE(buf[0] == 0x12, "put_commd");
    YLATEST_ASSERT_TRUE(buf[1] == 0x34, "put_value");
    YLATEST_ASSERT_TRUE(buf[2] == 0x56, "put_value");
    YLATEST_ASSERT_TRUE(buf[3] == 0x78, "put_value");
    YLATEST_ASSERT_TRUE(buf[4] == 0x9a, "put_value");
    
    return 0;
}

static int test_init_null()
{
    yla_vm vm;
    yla_cop_type buf[100];

    YLATEST_ASSERT_FALSE(yla_vm_init(&vm, NULL, 100), "init NULL");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm)==YLA_VM_ERROR_NO_PROGRAM_CODE, "init NULL");
    
    return 0;
}

static int test_init_0()
{
    yla_cop_type code[1];
    yla_vm vm;

    YLATEST_ASSERT_FALSE(yla_vm_init(&vm, code, 0), "init 0 size code");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm)==YLA_VM_ERROR_NO_PROGRAM_CODE, "init 0 size code");

    return 0;
}

static int test_init_simple()
{
    yla_cop_type prg[HEADER_SIZE + 1];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 1);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 1), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == -1, "halt expected")
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_init_simple2()
{
    yla_cop_type prg[HEADER_SIZE + 2];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 2);
    put_commd(&ptr, CNOP);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 2), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == 1, "OK expected")
    YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == -1, "halt expected")
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_init_simple_run()
{
    yla_cop_type prg[HEADER_SIZE + 2];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 2);
    put_commd(&ptr, CNOP);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 2), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal")
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_push()
{
    yla_cop_type prg[HEADER_SIZE + 4];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 1, 0, 4);
    put_commd(&ptr, CPUSH);
    put_value(&ptr, 0x1234);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 4), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal")
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_get_stack_full()
{
    yla_cop_type prg[HEADER_SIZE + 4];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 4);
    put_commd(&ptr, CPUSH);
    put_value(&ptr, 0x1234);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 4), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal")
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_STACK_FULL, "incorrect error code");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_code_limit()
{
    yla_cop_type prg[HEADER_SIZE + 1];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 1);
    put_commd(&ptr, CNOP);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 1), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal")
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_CODE_SEG_EXCEED, "incorrect error code");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_save_load()
{
	 yla_int_type stack_size = 1;
	 yla_int_type vartable_size = 261;
	 yla_int_type prg_size = (4 + 3 * 2);
    yla_cop_type prg[HEADER_SIZE + prg_size];
    yla_cop_type *ptr = prg;

    put_header(&ptr, stack_size, vartable_size, prg_size);
    put_commd(&ptr, CPUSH);
	 put_value(&ptr, 0x9696);
	 put_commd(&ptr, CSAVE);
	 put_value(&ptr, 0x0104);
	 put_commd(&ptr, CLOAD);
	 put_value(&ptr, 0x0104);
	 put_commd(&ptr, CHALT);
    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + prg_size), "normal");
	 for (int i = 0; i < 3; i++)
	 {
    	YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == 1, "OK expected")
	 }
    YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == -1, "halt expected")
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_dup_instruction()
{
	 yla_int_type stack_size = 4;
	 yla_int_type vartable_size = 0;
	 yla_int_type prg_size = (6 + 4 * 2);
    yla_cop_type prg[HEADER_SIZE + prg_size];
    yla_cop_type *ptr = prg;

    put_header(&ptr, stack_size, vartable_size, prg_size);
    put_commd(&ptr, CPUSH);
	 put_value(&ptr, 0x000d);
    put_commd(&ptr, CPUSH);
	 put_value(&ptr, 0x000e);
    put_commd(&ptr, CPUSH);
	 put_value(&ptr, 0x000f);
	 put_commd(&ptr, CDUP);
	 put_value(&ptr, 0x0000);
	 put_commd(&ptr, CTEST);
	 put_commd(&ptr, CHALT);
    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + prg_size), "normal");
	 for (int i = 0; i < 4; i++)
	 {
    	YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == 1, "OK expected")
	 }
	 yla_vm_do_command(&vm);
    YLATEST_ASSERT_TRUE(vm.stack.ptr[3] == 15, "DUP 0 must save variable with offset 0")
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_stack_instructions()
{
	 yla_int_type stack_size = 5;
	 yla_int_type vartable_size = 0;
	 yla_int_type prg_size = (9 + 8 * 2);
    yla_cop_type prg[HEADER_SIZE + prg_size];
    yla_cop_type *ptr = prg;

    put_header(&ptr, stack_size, vartable_size, prg_size);
    put_commd(&ptr, CPUSH);
	 put_value(&ptr, 0x000d);
    put_commd(&ptr, CPUSH);
	 put_value(&ptr, 0x000e);
    put_commd(&ptr, CPUSH);
	 put_value(&ptr, 0x000f);
	 put_commd(&ptr, CDUP);
	 put_value(&ptr, 0x0002);
	 put_commd(&ptr, CDUP);
	 put_value(&ptr, 0x0000);
	 put_commd(&ptr, CDEEP);
	 put_value(&ptr, 0x0002);
	 put_commd(&ptr, CSTK);
	 put_value(&ptr, 0x0002);
	 put_commd(&ptr, CHALT);
    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + prg_size), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
	 YLATEST_ASSERT_TRUE(vm.stack.ptr[0] == 13, "last variable must be equal to first variable after execution");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_linker_with_jumps()
{
    yla_vm vm;
	 
	 yla_int_type stack_size = 10;
	 yla_int_type vartable_size = 0;
	 yla_int_type prg_size = 180;
	 
    yla_cop_type prg[HEADER_SIZE + prg_size];
    yla_cop_type *ptr = prg;
	
	 yla_int_type prog_counter = 0;
	 
	 compliance_table subprog_addr;
	 compliance_table_init(&subprog_addr, 40);

    put_header(&ptr, stack_size, vartable_size, prg_size);
	 
    put_commd(&ptr, CPUSH);		prog_counter++;
	 put_value(&ptr, 0x000d);		prog_counter += 2;
    put_commd(&ptr, CPUSH);		prog_counter++;
	 put_value(&ptr, 0x000e);		prog_counter += 2;
    put_commd(&ptr, CPUSH);		prog_counter++;
	 put_value(&ptr, 0x000f);		prog_counter += 2;
    put_commd(&ptr, CJMP);			prog_counter++;	/*jump(11)*/
	 put_value(&ptr, 0x0011);		prog_counter += 2;
	 put_commd(&ptr, CHALT);		prog_counter++;
	 
	 /*jump(11)*/
	 // (0x0d,0x0e,0x0f)
	 compliance_table_set_addr(&subprog_addr, 0x0011, prog_counter);
	 
    put_commd(&ptr, CDUP);			prog_counter++;
	 put_value(&ptr, 0x0000);		prog_counter += 2;
    put_commd(&ptr, CJE);			prog_counter++;	/*jump_equal(12)*/
	 put_value(&ptr, 0x0012);		prog_counter += 2;
	 put_commd(&ptr, CHALT);		prog_counter++;
	 
	 /*jump_equal(12)*/
	 // (0x0d,0x0e,0x0f,0x0f)
	 compliance_table_set_addr(&subprog_addr, 0x0012, prog_counter);
	 
    put_commd(&ptr, CDUP);			prog_counter++;
	 put_value(&ptr, 0x0002);		prog_counter += 2;
    put_commd(&ptr, CJNE);			prog_counter++;	/*jump_not_equal(13)*/
	 put_value(&ptr, 0x0013);		prog_counter += 2;
	 put_commd(&ptr, CHALT);		prog_counter++;
	 
	 /*jump_equal(13)*/
	 // (0x0d,0x0e,0x0f,0x0f,0x0e)
	 compliance_table_set_addr(&subprog_addr, 0x0013, prog_counter);
	 
    put_commd(&ptr, CPUSH);		prog_counter++;
	 put_value(&ptr, 0x0000);		prog_counter += 2;
    put_commd(&ptr, CJZ);			prog_counter++;	/*jump_zero(14)*/
	 put_value(&ptr, 0x0014);		prog_counter += 2;
	 put_commd(&ptr, CHALT);		prog_counter++;
	 
	 /*jump_equal(14)*/
	 // (0x0d,0x0e,0x0f,0x0f,0x0e,0x00)
	 compliance_table_set_addr(&subprog_addr, 0x0014, prog_counter);
	 
    put_commd(&ptr, CPUSH);		prog_counter++;
	 put_value(&ptr, 0x0001);		prog_counter += 2;
    put_commd(&ptr, CJNZ);			prog_counter++;	/*jump_not_zero(17)*/
	 put_value(&ptr, 0x0017);		prog_counter += 2;
	 put_commd(&ptr, CHALT);		prog_counter++;
	 
	 /*jump_not_equal(17)*/
	 // (0x0d,0x0e,0x0f,0x0f,0x0e,0x00,0x01)
	 compliance_table_set_addr(&subprog_addr, 0x0017, prog_counter);
	 
    put_commd(&ptr, CDUP);			prog_counter++;
	 put_value(&ptr, 0x0006);		prog_counter += 2;
	 put_commd(&ptr, CHALT);		prog_counter++;
	 
	 yla_cop_type *prog_linker = prg;
	 int link_state = link_program(prog_linker, prog_counter, &subprog_addr);
	 
	 YLATEST_ASSERT_TRUE(link_state == 1, "link program state OK expected");
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, &prg, HEADER_SIZE + prg_size), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
	 
	 YLATEST_ASSERT_TRUE(vm.stack.ptr[7] == 13, "last variable must be equal to first variable after execution");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_multiplicity_union()
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
		YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
	} else if (link_state == -8) {
		vm.last_error = YLA_VM_LINKER_ERROR_TABLE_EMPTY;
	} else if (link_state == -9) {
		vm.last_error = YLA_VM_LINKER_ERROR_ADDRESS_NOT_FOUND;
	}
	
	YLATEST_ASSERT_TRUE(vm.vartable[answer1] == 3, "multiplicities addition result 3 expected");
	YLATEST_ASSERT_TRUE(vm.vartable[answer2] == 3, "multiplicities addition result 3 expected");
	YLATEST_ASSERT_TRUE(vm.vartable[answer3] == 5, "multiplicities addition result 3 expected");
	
	YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
	
	return 0;
}

static int test_multiplicity_intersect()
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
	
	// Answers addresses in vartable for tester
	yla_int_type answer1 = 0x0021;
	yla_int_type answer2 = 0x0022;
	yla_int_type answer3 = 0x0023;
	
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
	
	// Multiplicity 2: {9,10,15}, count: 3
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0009);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x000a);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x000f);				prog_counter += 2;
	//
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0003);				prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;	// M2
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0200);				prog_counter += 2;	/*mult_intersect(200)*/
	
	// Multiplicity out: {9,10}, count: 2
	// Save result mult count to first mult count
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
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;	// Mres
	
	// Multiplicity 2: {}, count: 0
	// [count]
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0200);				prog_counter += 2;	/*mult_intersect(200)*/
	
	// Multiplicity out: {}, count: 0
	// Save result mult count to first mult count
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
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;	// Mres
	
	// Multiplicity 1: {5,6}, count: 2
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0009);				prog_counter += 2;
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x000a);				prog_counter += 2;
	// [count]
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0002);				prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;	// M1
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[0]);	prog_counter += 2;
	
	// Multiplicity 2: {}, count: 0
	// [count]
	put_commd(&prog_ptr, CPUSH);				prog_counter++;
	put_value(&prog_ptr, 0x0000);				prog_counter += 2;
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;	// M2
	put_commd(&prog_ptr, CLOAD);				prog_counter++;
	put_value(&prog_ptr, global_var[1]);	prog_counter += 2;
	
	put_commd(&prog_ptr, CALO);				prog_counter++;
	put_value(&prog_ptr, 0x0200);				prog_counter += 2;	/*mult_intersect(200)*/
	
	// Multiplicity out: {}, count: 0
	// Save result mult count to first mult count
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
	put_commd(&prog_ptr, CSAVE);				prog_counter++;
	put_value(&prog_ptr, global_var[2]);	prog_counter += 2;	// Mres
	
	put_commd(&prog_ptr, CHALT);				prog_counter++;
	
	put_multiplicity_intersect(&prog_ptr, 0x0200, &subprog_addr, &prog_counter, global_var, 0x0300);
	put_num_in_multiplicity(&prog_ptr, 0x0300, &subprog_addr, &prog_counter, global_var);
	
   yla_vm vm;
	
	yla_cop_type *prog_linker_ptr = prg;
	int link_state = link_program(prog_linker_ptr, prog_counter, &subprog_addr);
	
	yla_vm_init(&vm, &prg, (prg_size + HEADER_SIZE));
	
	if (link_state == 1) {
		YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
	} else if (link_state == -8) {
		vm.last_error = YLA_VM_LINKER_ERROR_TABLE_EMPTY;
	} else if (link_state == -9) {
		vm.last_error = YLA_VM_LINKER_ERROR_ADDRESS_NOT_FOUND;
	}
	
   YLATEST_ASSERT_TRUE(vm.last_error == YLA_VM_ERROR_OK, "linker state OK expected");
	
	YLATEST_ASSERT_TRUE(vm.vartable[answer1] == 2, "2 expected");
	YLATEST_ASSERT_TRUE(vm.vartable[answer2] == 0, "zero expected");
	YLATEST_ASSERT_TRUE(vm.vartable[answer3] == 0, "zero expected");
	
	YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

	return 0;
}

static int test_multiplicity_num()
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
		YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
	} else if (link_state == -8) {
		vm.last_error = YLA_VM_LINKER_ERROR_TABLE_EMPTY;
	} else if (link_state == -9) {
		vm.last_error = YLA_VM_LINKER_ERROR_ADDRESS_NOT_FOUND;
	}
	
   YLATEST_ASSERT_TRUE(vm.last_error == YLA_VM_ERROR_OK, "linker state OK expected");
	
	YLATEST_ASSERT_TRUE(vm.vartable[answer1] == 1, "value must be found inside multiplicity");
	YLATEST_ASSERT_TRUE(vm.vartable[answer2] == 0, "value must not be found inside multiplicity");
	
	YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

	return 0;
}


YLATEST_BEGIN(yla_vm_test1)
  YLATEST_ADD_TEST_CASE(test_gencode)
  YLATEST_ADD_TEST_CASE(test_init_null)
  YLATEST_ADD_TEST_CASE(test_init_0)
  YLATEST_ADD_TEST_CASE(test_init_simple)
  YLATEST_ADD_TEST_CASE(test_init_simple2)
  YLATEST_ADD_TEST_CASE(test_init_simple_run)
  YLATEST_ADD_TEST_CASE(test_push)
  YLATEST_ADD_TEST_CASE(test_get_stack_full)
  YLATEST_ADD_TEST_CASE(test_code_limit)
  YLATEST_ADD_TEST_CASE(test_save_load)
  YLATEST_ADD_TEST_CASE(test_dup_instruction)
  YLATEST_ADD_TEST_CASE(test_stack_instructions)
  YLATEST_ADD_TEST_CASE(test_linker_with_jumps)
  YLATEST_ADD_TEST_CASE(test_multiplicity_union)
  YLATEST_ADD_TEST_CASE(test_multiplicity_intersect)
  YLATEST_ADD_TEST_CASE(test_multiplicity_num)
YLATEST_END
