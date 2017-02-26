/*
	Multiplicity operations: union, intersection, find value in multiplicity
*/

#include "yla_multiplicity.h"

/* Subprogram: num_in_mult_finder 
	This program needs 1 global variable for counter flag (CX)
	and 6 jumps for linker
	Reductions: CX -- counter, 
		equals to multiplicity dx in whole stack;
		DX -- multiplicity dx + multiplicity length 
		S -- stack; VT -- vartable */
void put_num_in_multiplicity(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter, yla_int_type global_var[])
{
	// >_<
	yla_int_type prog_count = *prog_counter;
	
	yla_int_type offset = global_var[6];
	yla_int_type cx = global_var[7];
	
	compliance_table_set_addr(compliance, subprog_start_addr, prog_count);
	
	// Local vars section
	// offset += 6
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, offset);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0006);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, offset);			prog_count += 2;
	// CX = offset
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, offset);			prog_count += 2;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;	// [CX]
	// End local vars section
	
	compliance_table_set_addr(compliance, 0x0111, prog_count);	/*num_in_mult(111)*/
	
	// 1. Check "flags" DX[DX = M1.count + CX] and CX
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;	// [CX]
	put_commd(prog_ptr, CDUP);				prog_count++;
	put_value(prog_ptr, 0x0003);			prog_count += 2;
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, offset);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;		// [DX]
	put_commd(prog_ptr, CCMP);				prog_count++;
	
	// If they are equal -- ret subprogram
	put_commd(prog_ptr, CJNZ);				prog_count++;
	put_value(prog_ptr, 0x011f);			prog_count += 2;	/*ret_unsuccessful(11f)*/
	
	// Else call comparator
	put_commd(prog_ptr, CALO);				prog_count++;
	put_value(prog_ptr, 0x0112);			prog_count += 2;	/*comparator(112)*/
	
	// If value exists in multiplicity -- ret subprogram
	put_commd(prog_ptr, CJNZ);				prog_count++;
	put_value(prog_ptr, 0x011e);			prog_count += 2;	/*ret_successful(11e)*/	
	
	// 2. CX++
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;
	
	// 3. Erase prevoius calculations from top of stack
	put_commd(prog_ptr, CSTK);				prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	
	// 4. Jump to start of subprogram
	put_commd(prog_ptr, CJMP);				prog_count++;
	put_value(prog_ptr, 0x0111);			prog_count += 2;	/*num_in_mult(111)*/
	
	/*ret_successful(11e)*/
	compliance_table_set_addr(compliance, 0x011e, prog_count);
	put_commd(prog_ptr, CALO);				prog_count++;
	put_value(prog_ptr, 0x011d);			prog_count += 2;	/*reset_offset(11d)*/
	put_commd(prog_ptr, CDUP);				prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CRET);				prog_count++;
	
	/*ret_unsuccessful(11f)*/
	compliance_table_set_addr(compliance, 0x011f, prog_count);
	put_commd(prog_ptr, CALO);				prog_count++;
	put_value(prog_ptr, 0x011d);			prog_count += 2;	/*reset_offset(11d)*/
	put_commd(prog_ptr, CDUP);				prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	put_commd(prog_ptr, CDEEP);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CRET);				prog_count++;
	
	/*reset_offset(11d)*/
	compliance_table_set_addr(compliance, 0x011d, prog_count);
	// offset -= 6
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, offset);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0006);			prog_count += 2;
	put_commd(prog_ptr, CSUB);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, offset);			prog_count += 2;
	put_commd(prog_ptr, CRET);				prog_count++;
	
	/* comparator(112) */
	compliance_table_set_addr(compliance, 0x0112, prog_count);
	// 1. Get value that we are looking for in multiplicity(further "?-value") 
	// and value by CX offset from stack(further "CX-dx value")
	put_commd(prog_ptr, CDUP);				prog_count++;
	put_value(prog_ptr, 0x0003);			prog_count += 2;	// ?-value

	put_commd(prog_ptr, CGDUP);			prog_count++;
	put_value(prog_ptr, cx);				prog_count += 2;	// CX-offset value
	// 2. result = (CX-offset value == ?-value)
	put_commd(prog_ptr, CCMP);				prog_count++;
	put_commd(prog_ptr, CDEEP);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;	// push in res
	// 3. Ret subprogram
	put_commd(prog_ptr, CRET);				prog_count++;
	
	*prog_counter = prog_count;
}

/* Multiplicity union 
	This program needs 4 jumps for linker	
*/
void put_multiplicity_union(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter, yla_int_type global_var[])
{
	// >_<
	yla_int_type prog_count = *prog_counter;
	
	compliance_table_set_addr(compliance, subprog_start_addr, prog_count);
	
	yla_int_type local_dx = global_var[3];
	yla_int_type local_cx = global_var[4];
	yla_int_type local_ret = global_var[5];
	yla_int_type M1 = global_var[0];
	yla_int_type M2 = global_var[1];
	yla_int_type Mres = global_var[2];

	// Local var section
	// local_ret
	put_commd(prog_ptr, CSAVE);		prog_count++;
	put_value(prog_ptr, local_ret);	prog_count += 2;
	// local_dx = 0
	put_commd(prog_ptr, CPUSH);		prog_count++;
	put_value(prog_ptr, 0x0000);		prog_count += 2;
	put_commd(prog_ptr, CSAVE);		prog_count++;
	put_value(prog_ptr, local_dx);	prog_count += 2;
	// local_cx = M2
	put_commd(prog_ptr, CLOAD);		prog_count++;
	put_value(prog_ptr, M2);			prog_count += 2;
	put_commd(prog_ptr, CSAVE);		prog_count++;
	put_value(prog_ptr, local_cx);	prog_count += 2;
	// End local var section
	
	/*continue(150)*/
	compliance_table_set_addr(compliance, 0x0150, prog_count);
	// 1. Check decremently changing CX and static DX values
	put_commd(prog_ptr, CLOAD);		prog_count++;
	put_value(prog_ptr, local_cx);	prog_count += 2;
	put_commd(prog_ptr, CLOAD);		prog_count++;
	put_value(prog_ptr, local_dx);	prog_count += 2;
	put_commd(prog_ptr, CCMP);			prog_count++;
	
	// If they are not equal -- dup and deep values
	put_commd(prog_ptr, CJZ);			prog_count++;
	put_value(prog_ptr, 0x0151);		prog_count += 2;	/*dup_and_deep(151)*/
	
	// If they are equal -- save result mult count and ret subprogram
	put_commd(prog_ptr, CJNZ);			prog_count++;
	put_value(prog_ptr, 0x0152);		prog_count += 2;	/*save_and_ret(152)*/
	
	/*dup_and_deep(151)*/
	compliance_table_set_addr(compliance, 0x0151, prog_count);
	// Delete comparator value
	put_commd(prog_ptr, CSTK);		prog_count++;
	put_value(prog_ptr, 0x0001);	prog_count += 2;
	// Dup and deep
	// 1. CGDUP CX
	put_commd(prog_ptr, CGDUP);		prog_count++;
	put_value(prog_ptr, local_cx);	prog_count += 2;
	// 2.1 local_cx++
	put_commd(prog_ptr, CLOAD);		prog_count++;
	put_value(prog_ptr, local_cx);	prog_count += 2;
	put_commd(prog_ptr, CPUSH);		prog_count++;
	put_value(prog_ptr, 0x0001);		prog_count += 2;
	put_commd(prog_ptr, CADD);			prog_count++;
	put_commd(prog_ptr, CSAVE);		prog_count++;
	put_value(prog_ptr, local_cx);	prog_count += 2;
	// 2.2 CGDEEP CX
	put_commd(prog_ptr, CGDEEP);		prog_count++;
	put_value(prog_ptr, local_cx);	prog_count += 2;
	// 2.3 local_cx - 2
	put_commd(prog_ptr, CLOAD);		prog_count++;
	put_value(prog_ptr, local_cx);	prog_count += 2;
	put_commd(prog_ptr, CPUSH);		prog_count++;
	put_value(prog_ptr, 0x0002);		prog_count += 2;
	put_commd(prog_ptr, CSUB);			prog_count++;
	put_commd(prog_ptr, CSAVE);		prog_count++;
	put_value(prog_ptr, local_cx);	prog_count += 2;
	// 3. Continue subprogram
	put_commd(prog_ptr, CJMP);			prog_count++;
	put_value(prog_ptr, 0x0150);		prog_count += 2;	/*continue(150)*/
	
	/*save_and_ret(152)*/
	compliance_table_set_addr(compliance, 0x0152, prog_count);
	// 1. Delete comparator value and two extra variables
	put_commd(prog_ptr, CSTK);			prog_count++;
	put_value(prog_ptr, 0x0003);		prog_count += 2;
	// 2. Mres = M1 + M2
	put_commd(prog_ptr, CLOAD);		prog_count++;
	put_value(prog_ptr, M1);			prog_count += 2;
	put_commd(prog_ptr, CLOAD);		prog_count++;
	put_value(prog_ptr, M2);			prog_count += 2;
	put_commd(prog_ptr, CADD);			prog_count++;
	put_commd(prog_ptr, CSAVE);		prog_count++;
	put_value(prog_ptr, Mres);			prog_count += 2;
	put_commd(prog_ptr, CLOAD);		prog_count++;
	put_value(prog_ptr, Mres);			prog_count += 2;
	// 3. M1 = M2 = 0
	put_commd(prog_ptr, CPUSH);		prog_count++;
	put_value(prog_ptr, 0x0000);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);		prog_count++;
	put_value(prog_ptr, 0x0000);		prog_count += 2;
	put_commd(prog_ptr, CSAVE);		prog_count++;
	put_value(prog_ptr, M1);			prog_count += 2;
	put_commd(prog_ptr, CSAVE);		prog_count++;
	put_value(prog_ptr, M2);			prog_count += 2;
	// 4. Ret subprogram and clean ret variable
	put_commd(prog_ptr, CLOAD);		prog_count++;
	put_value(prog_ptr, local_ret);	prog_count += 2;
	put_commd(prog_ptr, CPUSH);		prog_count++;
	put_value(prog_ptr, 0x0000);		prog_count += 2;
	put_commd(prog_ptr, CSAVE);		prog_count++;
	put_value(prog_ptr, local_ret);	prog_count += 2;
	put_commd(prog_ptr, CRET);			prog_count++;
	
	*prog_counter = prog_count;
}

/* Subprogram: mult_intersect 
	This program needs 8 jumps for linker	
*/
void put_multiplicity_intersect(yla_int_type **prog_ptr, yla_int_type subprog_start_addr, compliance_table *compliance, yla_int_type *prog_counter, yla_int_type global_var[], yla_int_type num_in_mult_addr)
{
	// >_<
	yla_int_type prog_count = *prog_counter;
	
	compliance_table_set_addr(compliance, subprog_start_addr, prog_count);
	// Addresses of global variables
	yla_int_type M1 = global_var[0];
	yla_int_type M2 = global_var[1];
	yla_int_type Mres = global_var[2];
	yla_int_type local_dx = global_var[3];
	yla_int_type local_cx = global_var[4];
	yla_int_type local_ret = global_var[5];
	yla_int_type offset = global_var[6];

	// Local var section
	// ret
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_ret);		prog_count += 2;
	// offset = (1[M2.count] + M2)
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, M2);				prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;	/* M2.count */
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, offset);			prog_count += 2;
	// DX = M2 + 1
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, M2);				prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	// CX = 1
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	// End Local var section
	
	// 1. Check DX and CX
	/*continue(203)*/
	compliance_table_set_addr(compliance, 0x0203, prog_count);
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	put_commd(prog_ptr, CCMP);				prog_count++;
	put_commd(prog_ptr, CJNZ);				prog_count++;
	put_value(prog_ptr, 0x0204);			prog_count += 2;	/*put_result_mult(204)*/
	put_commd(prog_ptr, CSTK);				prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;	// delete result of CMP
	
	// 2. Dup CX-offset value and load M2 value for num_in_mult subprogram
	// LOAD M1
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, M1);				prog_count += 2;
	//
	// CX++
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	// CGDUP CX
	put_commd(prog_ptr, CGDUP);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	// CX--
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CSUB);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	
	put_commd(prog_ptr, CALO);				prog_count++;
	put_value(prog_ptr, num_in_mult_addr);	prog_count += 2;	/*num_in_mult_finder(addr)*/
	
	put_commd(prog_ptr, CJZ);				prog_count++;
	put_value(prog_ptr, 0x0202);			prog_count += 2;	/*num_not_found(202)*/
	put_commd(prog_ptr, CJNZ);				prog_count++;
	put_value(prog_ptr, 0x0201);			prog_count += 2;	/*num_found(201)*/
	
	/*num_found(201)*/
	compliance_table_set_addr(compliance, 0x0201, prog_count);
	put_commd(prog_ptr, CSTK);				prog_count++;
	put_value(prog_ptr, 0x0002);			prog_count += 2;
	// replace mult count by finded value
	put_commd(prog_ptr, CDEEP);			prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	// offset++
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, offset);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, offset);			prog_count += 2;
	// local_dx++
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	// local_cx += 2
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0002);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	// Mres++
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, Mres);				prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, Mres);				prog_count += 2;
	// Continue program
	put_commd(prog_ptr, CJMP);				prog_count++;
	put_value(prog_ptr, 0x0203);			prog_count += 2;	/*continue(203)*/
	
	/*num_not_found(202)*/
	compliance_table_set_addr(compliance, 0x0202, prog_count);
	put_commd(prog_ptr, CSTK);				prog_count++;
	put_value(prog_ptr, 0x0004);			prog_count += 2;
	// local_cx++
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	// Continue program
	put_commd(prog_ptr, CJMP);				prog_count++;
	put_value(prog_ptr, 0x0203);			prog_count += 2;	/*continue(203)*/
	
	// ***************************************************
	/*put_result_mult(204)*/
	/* Replace first and second multiplicity by result multiplicity; 
	also delete tail of extra values */
	compliance_table_set_addr(compliance, 0x0204, prog_count);
	// Delete previous result from top of stack
	put_commd(prog_ptr, CSTK);				prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	
	// Local var section
	// local_dx += M1 - 1[after first CGDUP this value must be decremented]
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, M1);				prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CSUB);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	// local_CX = 0
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	// End local var section
	
	// 1. Check CX and Mres
	/*continue_writing_in(210)*/
	compliance_table_set_addr(compliance, 0x0210, prog_count);
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, Mres);				prog_count += 2;
	put_commd(prog_ptr, CCMP);				prog_count++;
	
	put_commd(prog_ptr, CJZ);				prog_count++;
	put_value(prog_ptr, 0x0211);			prog_count += 2;	/*keep_writing_in(211)*/
	put_commd(prog_ptr, CJNZ);				prog_count++;
	put_value(prog_ptr, 0x0212);			prog_count += 2;	/*stop_writing(212)*/
	
	// 2. If not equal -- keep writing in
	/*keep_writing_in(211)*/
	compliance_table_set_addr(compliance, 0x0211, prog_count);
	// Delete previous comparation result
	put_commd(prog_ptr, CSTK);				prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	// CGDEEP local_dx
	put_commd(prog_ptr, CGDEEP);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	// local_dx - 2
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0002);			prog_count += 2;
	put_commd(prog_ptr, CSUB);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	// CX++
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_cx);		prog_count += 2;
	
	// Continue writing in
	put_commd(prog_ptr, CJMP);				prog_count++;
	put_value(prog_ptr, 0x0210);			prog_count += 2;	/*continue_writing_in(210)*/
	
	// 3. If equal -- stop writing in
	/*stop_writing(212)*/
	compliance_table_set_addr(compliance, 0x0212, prog_count);
	// Delete previous comparation result
	put_commd(prog_ptr, CSTK);				prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	// local_dx++
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0001);			prog_count += 2;
	put_commd(prog_ptr, CADD);				prog_count++;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	// Put result multiplicity count
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, Mres);				prog_count += 2;
	// CGDEEP DX
	put_commd(prog_ptr, CGDEEP);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	// Delete all from stack tail
	put_commd(prog_ptr, CGSTK);			prog_count++;
	put_value(prog_ptr, local_dx);		prog_count += 2;
	
	// 4. 
	// -- Set to zero M1 and M2
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, M1);				prog_count += 2;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, M2);				prog_count += 2;
	// -- Set to zero offset
	put_commd(prog_ptr, CPUSH);			prog_count++;
	put_value(prog_ptr, 0x0000);			prog_count += 2;
	put_commd(prog_ptr, CSAVE);			prog_count++;
	put_value(prog_ptr, offset);			prog_count += 2;
	
	// 5. Ret subprogram
	put_commd(prog_ptr, CLOAD);			prog_count++;
	put_value(prog_ptr, local_ret);		prog_count += 2;
	put_commd(prog_ptr, CRET);				prog_count++;
	
	*prog_counter = prog_count;
}
