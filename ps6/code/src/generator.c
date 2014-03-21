#include "generator.h"
extern int outputStage; // This variable is located in vslc.c
static char* currentClass = NULL;
int peephole = 0;

#define ARM 1


/* Registers and opcodes have been moved to generator.h */


/* Start and last element for emitting/appending instructions */
static instruction_t *start = NULL, *last = NULL;

/* Support variables for nested while, if and continue.*/
static int while_count = 0;
static int if_count = 0;

// The counter used for the debug printing.
static int nodeCounter = 0;

/* Provided auxiliaries... */
static void instruction_append ( instruction_t *next )
{
	if ( start != NULL )
	{
		last->next = next;
		last = next;
	}
	else
		start = last = next;
}


void instruction_add ( opcode_t op, char *arg1, char *arg2, int off1, int off2 )
{
	instruction_t *i = (instruction_t *) malloc ( sizeof(instruction_t) );
	i->opcode = op;
	i->offsets[0] = off1; i->offsets[1] = off2;
	i->operands[0] = arg1; i->operands[1] = arg2;
	i->next = NULL;
	instruction_append ( i );
}

void instruction_add3 ( opcode_t op, char* arg1, char* arg2, char* arg3)
{
	instruction_t *i = (instruction_t *) malloc ( sizeof(instruction_t) );
	i->opcode = op;
	i->offsets[0] = 0; i->offsets[1] = 0;
	i->operands[0] = arg1; i->operands[1] = arg2; i->operands[2] = arg3;
	i->next = NULL;
	instruction_append ( i );
}


static void instructions_finalize ( void ) {};


/*
 * Smart wrapper for "printf". 
 * Makes a comment in the assembly to guide.
 * Also prints a copy to the debug stream if needed.
 */
void tracePrint( const char * string, ... )
{
	va_list args;
	char buff[1000];
	char buff2[1000];
	// 
	va_start (args, string);
	vsprintf(buff, string, args);
	va_end (args);
	
	sprintf(buff2, "%d %s", nodeCounter++, buff);
	
	if( outputStage == 10 )
    	fprintf(stderr, "%s", buff2);

	instruction_add ( COMMMENT, STRDUP( buff2 ), NULL, 0, 0 );
}


void gen_default ( node_t *root, int scopedepth)
{
	/* Everything else can just continue through the tree */
	if(root == NULL){
		return;
	}

	for ( int i=0; i<root->n_children; i++ )
		if( root->children[i] != NULL )
			root->children[i]->generate ( root->children[i], scopedepth );
}


void gen_PROGRAM ( node_t *root, int scopedepth)
{
	/* Output the data segment */
	if( outputStage == 12 )
		strings_output ( stderr );
	instruction_add ( STRING, STRDUP( ".text" ), NULL, 0, 0 );

	tracePrint("Starting PROGRAM\n");

	gen_default(root, scopedepth);//RECUR();

	TEXT_DEBUG_FUNC_ARM();
	TEXT_HEAD_ARM();
	
	gp(root,scopedepth);
	

	tracePrint("End PROGRAM\n");

	TEXT_TAIL_ARM();
	
	if( outputStage == 12 )
		instructions_print ( stderr );
	instructions_finalize ();
}

void gen_CLASS (node_t *root, int scopedepth)
{

}

void gen_FUNCTION ( node_t *root, int scopedepth )
{
    // Generating label. This may need to be changed to handle labels for methods
    function_symbol_t* entry = root->function_entry;
	int len = strlen(entry->label);
	char *temp = (char*) malloc(sizeof(char) * (len + 3));
	temp[0] = 0;
	strcat(temp, "_");
	strcat(temp, entry->label);
	strcat(temp, ":");

	instruction_add(STRING, STRDUP(temp), NULL, 0, 0);

    gf(root,scopedepth);
}


void gen_DECLARATION_STATEMENT (node_t *root, int scopedepth)
{
	gd(root,scopedepth);
}


void gen_PRINT_STATEMENT(node_t* root, int scopedepth)
{
	tracePrint("Starting PRINT_STATEMENT\n");

	for(int i = 0; i < root->children[0]->n_children; i++){

		root->children[0]->children[i]->generate(root->children[0]->children[i], scopedepth);

		//Pushing the .INTEGER constant, which will be the second argument to printf,
		//and cause the first argument, which is the result of the expression, and is
		//allready on the stack to be printed as an integer
		base_data_type_t t = root->children[0]->children[i]->data_type.base_type;
		switch(t)
		{
		case INT_TYPE:
			instruction_add(STRING, STRDUP("\tmovw	r0, #:lower16:.INTEGER"), NULL, 0,0);
			instruction_add(STRING, STRDUP("\tmovt	r0, #:upper16:.INTEGER"), NULL, 0,0);
			instruction_add(POP, r1, NULL, 0,0);
			break;

		case FLOAT_TYPE:
			instruction_add(LOADS, sp, s0, 0,0);
			instruction_add(CVTSD, s0, d0, 0,0);
			instruction_add(STRING, STRDUP("\tfmrrd	r2, r3, d0"), NULL, 0,0);
			instruction_add(STRING, STRDUP("\tmovw	r0, #:lower16:.FLOAT"), NULL, 0,0);
			instruction_add(STRING, STRDUP("\tmovt	r0, #:upper16:.FLOAT"), NULL, 0,0);
			
			// And now the tricky part... 8-byte stack alignment :(
			// We have at least 4-byte alignment always.
			// Check if its only 4-byte aligned right now by anding that bit in the stack-pointer.
			// Store the answer in r5, and set the zero flag.
			instruction_add(STRING, STRDUP("\tandS	r5, sp, #4"), NULL, 0,0);
			// Now use the zero flag as a condition to optionally change the stack-pointer
			instruction_add(STRING, STRDUP("\tpushNE	{r5}"), NULL, 0,0);
			break;
			
		case BOOL_TYPE:
			instruction_add(STRING, STRDUP("\tmovw	r0, #:lower16:.INTEGER"), NULL, 0,0);
			instruction_add(STRING, STRDUP("\tmovt	r0, #:upper16:.INTEGER"), NULL, 0,0);
			instruction_add(POP, r1, NULL, 0,0);
			break;

		case STRING_TYPE:
			instruction_add(POP, r0, NULL, 0,0);
			break;

		default:
			instruction_add(PUSH, STRDUP("$.INTEGER"), NULL, 0,0);
			fprintf(stderr, "WARNING: attempting to print something not int, float or bool\n");
			break;
		}
		
		instruction_add(SYSCALL, STRDUP("printf"), NULL,0,0);

		// Undo stack alignment.
		if(t == FLOAT_TYPE) {
			// Redo the zero flag test on r5, as it will give the same answer as the first test on sp.
			instruction_add(STRING, STRDUP("\tandS	r5, #4"), NULL, 0,0);
			// Conditionally remove the alignment. 
			instruction_add(STRING, STRDUP("\tpopNE	{r5}"), NULL, 0,0);
		}
	}

	instruction_add(MOVE32, STRDUP("#0x0A"), r0, 0,0);
	instruction_add(SYSCALL, STRDUP("putchar"), NULL, 0,0);

	tracePrint("Ending PRINT_STATEMENT\n");
}


void gen_EXPRESSION ( node_t *root, int scopedepth )
{
	/*
	 * Expressions:
	 * Handle any nested expressions first, then deal with the
	 * top of the stack according to the kind of expression
	 * (single variables/integers handled in separate switch/cases)
	 */
	tracePrint ( "Starting EXPRESSION of type %s\n", (char*) root->expression_type.text);

	switch(root->expression_type.index){

	case FUNC_CALL_E:
		ge(root,scopedepth);
		break;

    /* Add cases for other expressions here */
	}

	tracePrint ( "Ending EXPRESSION of type %s\n", (char*) root->expression_type.text);
}



void gen_VARIABLE ( node_t *root, int scopedepth )
{
	gv(root,scopedepth);
}

void gen_CONSTANT (node_t * root, int scopedepth)
{
	gc(root,scopedepth);
}

void gen_ASSIGNMENT_STATEMENT ( node_t *root, int scopedepth )
{
	 tracePrint ( "Starting ASSIGNMENT_STATEMENT\n");

	//Generating the code for the expression part of the assignment. The result is
	//placed on the top of the stack
	root->children[1]->generate(root->children[1], scopedepth);

    // Left hand side may be a class field, which should be handled in this assignment
	if(root->children[0]->expression_type.index == CLASS_FIELD_E){

	}
	// or a variable, handled in previous assignment
	else{
		ga(root,scopedepth);
	}

	tracePrint ( "End ASSIGNMENT_STATEMENT\n");
}

void gen_RETURN_STATEMENT ( node_t *root, int scopedepth )
{
	gr(root,scopedepth);
}


void gen_WHILE_STATEMENT ( node_t *root, int scopedepth )
{

}


void gen_IF_STATEMENT ( node_t *root, int scopedepth )
{

}


static void
instructions_print ( FILE *stream )
{
    instruction_t *this = start;

		while ( this != NULL )
		{
		    switch ( this->opcode ) // ARM
		    {
		        case PUSH:
		            if ( this->offsets[0] == 0 )
		                fprintf ( stream, "\tpush\t{%s}\n", this->operands[0] );
		            else
		                fprintf ( stream, "\tpushl TODO\t%d(%s)\n",
		                        this->offsets[0], this->operands[0]
		                        );
		            break;
		        case POP:
		            if ( this->offsets[0] == 0 )
		                fprintf ( stream, "\tpop\t{%s}\n", this->operands[0] );
		            else
		                fprintf ( stream, "\tpopl TODO\t%d(%s)\n",
		                        this->offsets[0], this->operands[0]
		                        );
		            break;

		        case MOVE32:
		        	if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
		        		fprintf ( stream, "\tmovw\t%s, #:lower16:%s\n",
		        		          this->operands[1], this->operands[0]+1
		        		          );
		        		fprintf ( stream, "\tmovt\t%s, #:upper16:%s\n",
		        			      this->operands[1], this->operands[0]+1
		        		          );
		        	break;

		        case MOVE:
		            if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tmov\t%s, %s\n",
		                        this->operands[0], this->operands[1]
		                        );
		            //Should not be used, for legacy support only
		            else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tldr\t%s, [%s, #%d]\n", 
		                        this->operands[1], this->operands[0], this->offsets[0]
		                        );
		            else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
		                fprintf ( stream, "\tstr\t%s, [%s, #%d]\n",
		                        this->operands[0], this->operands[1], this->offsets[1]
		                        );
		            break;
		        
		        
		        case LOAD:
		            if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tldr\t%s, [%s]\n",
		                        this->operands[0], this->operands[1]
		                        );
		            else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
		                fprintf ( stream, "\tldr\t%s, [%s, #%d]\n", 
		                        //this->offsets[0], this->operands[0], this->operands[1] "\tmovl\t%d(%s),%s\n",
		                        this->operands[0], this->operands[1], this->offsets[1]
		                        );
		            else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "ERROR, LOAD format not correct\n",
		                        //this->operands[0], this->offsets[1], this->operands[1] "\tmovl\t%s,%d(%s)\n"
		                        this->operands[0], this->operands[1], this->offsets[1]
		                        );
		            break;
		        
		        case LOADS:
		            if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tflds\t%s, [%s]\n",
		                        this->operands[1], this->operands[0]
		                        );
		            else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tflds\t%s, [%s, #%d]\n", 
		                        //this->offsets[0], this->operands[0], this->operands[1] "\tmovl\t%d(%s),%s\n",
		                        this->operands[1], this->operands[0], this->offsets[0]
		                        );
		            else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
		                fprintf ( stream, "ERROR, LOAD format not correct\n",
		                        //this->operands[0], this->offsets[1], this->operands[1] "\tmovl\t%s,%d(%s)\n"
		                        this->operands[0], this->operands[1], this->offsets[1]
		                        );
		            break;
		        
		        
		        case STORE:
		            if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tstr\t%s, [%s]\n",
		                        this->operands[0], this->operands[1]
		                        );
		            else if ( this->offsets[1] != 0 && this->offsets[0] == 0 )
		                fprintf ( stream, "\tstr\t%s, [%s, #%d]\n", 
		                        //this->offsets[0], this->operands[0], this->operands[1] "\tmovl\t%d(%s),%s\n",
		                        this->operands[0], this->operands[1], this->offsets[1]
		                        );
		            else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
		                fprintf ( stream, "ERROR, STORE format not correct\n",
		                        //this->operands[0], this->offsets[1], this->operands[1] "\tmovl\t%s,%d(%s)\n"
		                        this->operands[0], this->operands[1], this->offsets[1]
		                        );
		            break;
		        
		        case STORES:
		            if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tfsts\t%s, [%s]\n",
		                        this->operands[0], this->operands[1]
		                        );
		            else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tfsts\t%s, [%s, #%d]\n", 
		                        //this->offsets[0], this->operands[0], this->operands[1] "\tmovl\t%d(%s),%s\n",
		                        this->operands[0], this->operands[1], this->offsets[0]
		                        );
		            else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
		                fprintf ( stream, "ERROR, STORE format not correct\n",
		                        //this->operands[0], this->offsets[1], this->operands[1] "\tmovl\t%s,%d(%s)\n"
		                        this->operands[0], this->operands[1], this->offsets[1]
		                        );
		            break;
		        
		        case SET:
		            if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "SET ERROR: set\t%s, %s\n",
		                        this->operands[1], this->operands[0]
		                        );
		            else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tmov\t%s, #%d\n", 
		                        //this->offsets[0], this->operands[0], this->operands[1] "\tmovl\t%d(%s),%s\n",
		                        this->operands[0], this->offsets[0]
		                        );
		            else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
		                fprintf ( stream, "\tmovl\t%s,%d(%s)\n",
		                        this->operands[0], this->offsets[1], this->operands[1]
		                        );
		            break;

		        case MOVES:
					if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
						fprintf ( stream, "\tmfcpys\t%s, %s\n",
								this->operands[1], this->operands[0]
								);
					else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
						fprintf ( stream, "\tError: NOT possible for ARM, use load/store\t%d(%s),%s\n",
								this->offsets[0], this->operands[0], this->operands[1]
								);
					else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
						fprintf ( stream, "\tError: NOT possible for ARM, use load/store\t%s,%d(%s)\n",
								this->operands[0], this->offsets[1], this->operands[1]
								);
					break;

		        case MOVED:
					if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
						fprintf ( stream, "\tmfcpyd TODO\t%s,%s\n",
								this->operands[1], this->operands[0]
								);
					else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
						fprintf ( stream, "\tError: NOT possible for ARM, use load/store\t%s, [%s,#%d]\n",  
								//this->offsets[0], this->operands[0], this->operands[1]   "\ldr\t%d(%s),%s\n",
								this->operands[1], this->operands[0], this->offsets[0]
								);
					else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
						fprintf ( stream, "\tError: NOT possible for ARM, use load/store\t%s,%d(%s)\n",
								this->operands[0], this->offsets[1], this->operands[1]
								);
					break;

		        case CVTSD:
					if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
						fprintf ( stream, "\tfcvtds\t%s,%s\n",
								this->operands[1], this->operands[0]
								);
					else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
						fprintf ( stream, "\tfcvtds TODO\t%d(%s),%s\n",
								this->offsets[0], this->operands[0], this->operands[1]
								);
					else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
						fprintf ( stream, "\tfcvtds TODO\t%s,%d(%s)\n",
								this->operands[0], this->offsets[1], this->operands[1]
								);
					break;



		        case ADD:
		        	if ( this->operands[2] == NULL){
		        		//Legacy support
		                fprintf ( stream, "\tadd\t%s, %s\n",
		                        this->operands[1], this->operands[0]
		                        );
		        	}
		            else{
		            	fprintf ( stream, "\tadd\t%s, %s, %s\n",
		            			this->operands[0], this->operands[1], this->operands[2]
		            			);
		            }
		            break;
		        
		        case FADD:
		            if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tfadds\t%s, %s, %s\n",
		                 		this->operands[1], this->operands[1], this->operands[0]
		                	   );
		            break;
		        
		        case SUB:
		            if ( this->operands[2] == NULL )
		                fprintf ( stream, "\tsub\t%s, %s\n",
		                        this->operands[1], this->operands[0]
		                        );
		            else{
		            	fprintf ( stream, "\tsub\t%s, %s, %s\n",
		            			this->operands[0], this->operands[1], this->operands[2]
		            			);
		            }
		            break;
		        
		         case FSUB:
		            if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tfsubs\t%s, %s, %s\n",
		                        this->operands[1], this->operands[1], this->operands[0]
		                        );
		            else
		            	fprintf ( stream, "Not supported...\tfsub\t%s, %s\n",
		            			this->operands[1], this->operands[0]
		                        );
		        	break;
		        
		        case MUL:
		        		fprintf(stream, "\tmul\t%s,%s,%s\n",
		        				this->operands[0], this->operands[1], this->operands[2]);

		            break;
		         case FMUL:
		            if ( this->offsets[0] == 0 )
		                fprintf ( stream, "\tfmuls\t %s, %s, %s\n", this->operands[1], this->operands[1], this->operands[0] ); 
		            else
		                fprintf ( stream, "Not supported...\tfmul\t%d(%s)\n",
		                        this->offsets[0], this->operands[0]
		                        );
		            break;
		        case DIV:

		                fprintf ( stream, "\tsdiv\t%s, %s, %s\n",
		                		this->operands[0], this->operands[1], this->operands[2] );

		            break;
		        case FDIV:
		            if ( this->offsets[0] == 0 &&  this->operands[1] == NULL)
		                fprintf ( stream, "\tfdivs\ts0, s0, %s\n", this->operands[0] );
		            else  if ( this->offsets[0] == 0)
		                fprintf ( stream, "\tfdivs\t%s, %s, %s\n", this->operands[1], this->operands[1], this->operands[0] );
		            else
		                fprintf ( stream, "\tidivl TODO\t%d(%s)\n",
		                        this->offsets[0], this->operands[0]
		                        );
		            break;
		        case NEG:
		            if ( this->offsets[0] == 0 )
		                fprintf ( stream, "\tnegl\t%s\n", this->operands[0] );
		            else
		                fprintf ( stream, "\tnegl\t%d(%s)\n",
		                        this->offsets[0], this->operands[0]
		                        );
		            break;

		        case DECL:
		            fprintf ( stream, "\tsubs\t%s, #1\n", this->operands[0]); // The s turn on flag updates
		            break;
		        case CMP:
		            if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
		                fprintf ( stream, "\tcmp\t%s,%s\n",
		                        this->operands[0], this->operands[1]
		                        );
		            break;
		        case FCMP:
		        	fprintf( stream, "\tfcmps\t%s,%s\n", this->operands[0], this->operands[1]);
		        	fprintf( stream, "\tvmrs APSR_nzcv, FPSCR\n");
		        	break;

		        case MOVGT:
		        	fprintf(stream, "\tmovgt\t %s, %s\n", this->operands[0], this->operands[1]);
		        	break;
		        case MOVGE:
		        	fprintf(stream, "\tmovge\t %s, %s\n", this->operands[0], this->operands[1]);
		        	break;
		        case MOVLT:
		        	fprintf(stream, "\tmovlt\t %s, %s\n", this->operands[0], this->operands[1]);
		        	break;
		        case MOVLE:
		        	fprintf(stream, "\tmovle\t %s, %s\n", this->operands[0], this->operands[1]);
		        	break;
		        case MOVEQ:
		        	fprintf(stream, "\tmoveq\t %s, %s\n", this->operands[0], this->operands[1]);
		        	break;
		        case MOVNE:
		        	fprintf(stream, "\tmovne\t %s, %s\n", this->operands[0], this->operands[1]);
		        	break;

		        case CALL: case SYSCALL:
		            fprintf ( stream, "\tbl\t" );
		            if ( this->opcode == CALL )
		                fputc ( '_', stream );
		            fprintf ( stream, "%s\n", this->operands[0] );
		            break;
		        case LABEL: 
		            fprintf ( stream, "_%s:\n", this->operands[0] );
		            break;

		        case JUMP:
		            fprintf ( stream, "\tb\t%s\n", this->operands[0] );
		            break;
		        case JUMPZERO:
		            fprintf ( stream, "\tbeq\t%s\n", this->operands[0] );
		            break;
		        case JUMPEQ:
		            fprintf ( stream, "\tbeq\t%s\n", this->operands[0] );
		            break;
		        case JUMPNE:
		            fprintf ( stream, "\tbne\t%s\n", this->operands[0] );
		            break;
		        case JUMPNONZ:
		            fprintf ( stream, "\tbne\t%s\n", this->operands[0] );
		            break;

		        case LEAVE: 
		        	// Same as "leave"
		        	fprintf ( stream, "\tmov\tsp, fp\n");
		        	fprintf ( stream, "\tpop\t{fp}\n");
		        	break;
		        case RET:   
		        	fprintf ( stream, "\tpop\t{pc}\n");
		        	break;

		        case STRING:
		                    fprintf ( stream, "%s\n", this->operands[0] );
		                    break;
		        
		        case COMMMENT:
		                    fprintf ( stream, "#%s", this->operands[0] );
		                    break;

		        case NIL:
		                    break;

		        default:
		                    fprintf ( stderr, "Error in instruction stream\n" );
		                    break;
		    }
		    this = this->next;

    }
}

