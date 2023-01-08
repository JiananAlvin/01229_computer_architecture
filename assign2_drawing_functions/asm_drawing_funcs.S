/*
 * Assembly-language implementations of the drawing functions
 * CSF Assignment 2 MS3
 * Jianan Xu
 * jxu147@jhu.edu
 */

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/* Offsets of struct Rect fields */
#define RECT_X_OFFSET        0
#define RECT_Y_OFFSET        4
#define RECT_WIDTH_OFFSET    8
#define RECT_HEIGHT_OFFSET   12

	.section .text

/***********************************************************************
   Helper functions
   You should declare these as .globl so that
   they can be called from the unit test code.
 ***********************************************************************/
/*
 * Check x and y coordinates to determine whether 
 * they are in bounds in the specified image.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate (pixel column)
 *   %edx     - y coordinate (pixel row)
 *
 * Return: 
 *   %rax     - 1 (x, y) is in bounds in the image
 *            - 0 (x, y) is out bounds of the image
 */
	.globl in_bounds
in_bounds:
    subq    $8, %rbp         /* Stack pointer alignment */
	movl    0(%rdi), %ebx    /* Store image width in %ebx */
	movl    4(%rdi), %ecx    /* Store image length in %ebx */
	movq 	$0, %r8          /* Sets %r8, %r9, %r10, %r11, and %rax to 0 */
	movq 	$0, %r9
	movq 	$0, %r10
	movq 	$0, %r11
	movq 	$0, %rax
	
	cmp      %esi, %ebx       /* Compare x and image width */
	setg     %r8b             /* Stores comparison result */
    cmp      $0, %esi         /* Compare x and 0 */
	setge    %r9b             /* Stores comparison result */
    and      %r8b, %r9b       /* If x in bound and >= 0 */  
	
	cmp      %edx, %ecx       /* Compare y and image length */
	setg     %r10b            /* Stores comparison result */
    cmp      $0, %edx         /* Compare y and 0 */
    setge    %r11b            /* Stores comparison result */
	and      %r10b, %r11b     /* If y in bound and >= 0 */
	
    and      %r9b, %r11b      /* If both x and y in bound */
	movb     %r11b, %al       /* Return result */
	addq     $8, %rbp         /* Restores stack pointer */
	ret


/*
 * Compute the index of a pixel in an image’s data
 * array given its x and y coordinates.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate (pixel column)
 *   %edx     - y coordinate (pixel row)
 *
 * Return: 
 *   %rax     - the index of a pixel in an image’s data array
 */	
	.globl compute_index
compute_index:
    subq     $8, %rbp 
    mov      $0, %rax                           /* Sets %rax %rbx to 0 */
    movl     IMAGE_WIDTH_OFFSET(%rdi), %ebx     /* Stores image width in %ebx */
	imull    %edx, %ebx                         /* y*(img->width) */
    addl     %esi, %ebx                         /* + x */
	movl     %ebx, %eax
	addq     $8, %rbp
    ret
	

/*
* Check whether a value is constrained to between min 
* and max. 
* 
* Parameters:
*   %rdi     - a 32-bit signed integer
*   %esi     - min, lower bound 
*   %edx     - max, upper bound
*
* Return:
*   %rax     - val is >= min and <= max
*            - val is < min or > max
*/	
	.globl clamp
clamp:  
    subq   $8, %rbp
    mov    $0, %rax          /* Sets %rax %rbx to 0 */
	mov    $0, %rbx
	cmp    %esi, %edi        /* If val >= min */
	setge  %al               /* Stores comparison result */
    cmp    %edx, %edi        /* If val <= max */
	setle  %bl               /* Stores comparison result */
	and    %bl, %al          /* If min <= val <= max */
	addq   $8, %rbp
	ret


/*
* Get the red component of a pixel color value.
*
* Parameters:
*   %rdi   - uint32_t color value
*
* Return:
*   %rax   - the red component of a pixel color value
*/
	.globl get_r
get_r:
    subq    $8, %rbp
	push 	%r8
    mov   	%edi, %eax          /* Copies color to %rax */
	shr   	$24, %rax           /* Rught shift 24 bits */
    movb   	%al, %r8b
	movq 	$0, %rax 
    movb  	%r8b, %al  
	pop		%r8     
	addq    $8, %rbp
	ret


/*
* Get the red component of a pixel color value.
*
* Parameters:
*   %rdi   - uint32_t color value
*
* Return:
*   %rax   - the green component of a pixel color value
*/	
	.globl get_g
get_g:
    subq    $8, %rbp
	push 	%r8
    mov   	%edi, %eax          /* Copies color to %rax */
	shr   	$16, %rax           /* Rught shift 16 bits */
    movb   	%al, %r8b
	movq 	$0, %rax 
    movb  	%r8b, %al     
	pop		%r8   
	addq    $8, %rbp
	ret


/*
* Get the red component of a pixel color value.
*
* Parameters:
*   %rdi   - uint32_t color value
*
* Return:
*   %rax   - the blue component of a pixel color value
*/	
	.globl get_b
get_b:
    subq    $8, %rbp
	push 	%r8
    mov   	%edi, %eax          /* Copies color to %rax */
	shr   	$8, %rax            /* Right shift 8 bits */ 
	movb   	%al, %r8b
	movq 	$0, %rax 
    movb   	%r8b, %al
	pop 	%r8  
	addq    $8, %rbp 
	ret


/*
* Get the red component of a pixel color value.
*
* Parameters:
*   %rdi   - uint32_t color value
*
* Return:
*   %rax   - the alpha component of a pixel color value
*/
	.globl get_a
get_a:
	subq    $8, %rbp
    movq    $0, %rax 
    mov     %dil, %al          /* Copies color to %rax */   
	addq    $8, %rbp    
	ret


/*
* Blend foreground and background color component values
* using a specified alpha (opacity) value.
* 
* Parameters:
*   %rdi      - the foreground color component value
*   %rsi      - the background color component value
*   %rdx      - the alpha value of the foreground color
*
* Return:
*   %rax      - the blended color component value
*/
	.globl blend_components
blend_components:
	subq   $8, %rbp
    mov    $255, %rbx
	mov    $255, %rax     /* Sets %rax to 255 */
    mov    %rdi, %r8      /* set %r8 to fg */

    imul   %rdx, %r8      /* fg*alpha */
	
	sub    %rdx, %rax     /* 255 - alpha */
    imul   %rsi, %rax     /* (255 - alpha)*bg */
	add    %r8, %rax      /* fg*alpha + (255 - alpha)*bg */
	mov    $0, %rdx       
	divq   %rbx           /* (fg*alpha + (255 - alpha)*bg)/255 */
	addq   $8, %rbp
 	ret 


/*
* Blend foreground and background colors using the foreground
* color’s alpha value to produce an opaque color.
* 
* Parameters:
*   %rdi      - the foreground color value
*   %rsi      - the background color value
*
* Return:
*   %rax      - the blended color value
*/
	.globl blend_colors
blend_colors:
	subq    $8, %rbp
/* ------Blends r------ */
	push	%rdi	          /* Stores fg to stack */
	push	%rsi	          /* Stores bg to stack */
    
	call    get_r             /* Gets red component of fg */
	mov     %rax, %r8         /* Stores red component of fg in %r8 */
	
	call    get_a             /* Gets alpha component of fg */
	mov     %rax, %r9         /* Stores alpha component of fg in %r9 */
    
	mov     %rsi, %rdi        /* Sets first arameter of get_r() as bg */
    call    get_r             /* Gets red component of bg */
	mov     %rax, %r10        /* Stores red component of bg in %r10 */

	mov     %r8, %rdi	      /* Sets 1st parameter of blend_components() */
	mov     %r10, %rsi        /* Sets 2nd parameter of blend_components() */
	mov     %r9, %rdx         /* Sets 3rd parameter of blend_components() */
	call    blend_components  
	mov     %rax, %rbx        /* Stores blended red component in %rbx */

    pop 	%rsi	          /* Restores bg to stack */
    pop  	%rdi	          /* Restores fg to stack */
/* ------Blends g------ */	
	push	%rdi	          /* Stores fg to stack */
	push	%rsi	          /* Stores bg to stack */
    push    %rbx              /* Stores blended red component in stack */
    
	call    get_g             /* Gets green component of fg */
	mov     %rax, %r8         /* Stores green component of fg in %r8 */
    
	call    get_a             /* Gets alpha component of fg */
	mov     %rax, %r9         /* Stores alpha component of fg in %r9 */

	mov     %rsi, %rdi        /* Sets first parameter of get_g() as bg */
    call    get_g             /* Gets green component of bg */
	mov     %rax, %r10        /* Stores green component of bg in %r10 */

	mov     %r8, %rdi	      /* Sets 1st parameter of blend_components() */
	mov     %r10, %rsi        /* Sets 2nd parameter of blend_components() */
	mov     %r9, %rdx         /* Sets 3rd parameter of blend_components() */
	call    blend_components  
	mov     %rax, %rcx        /* Stores blended green component in %rcx */
	
	pop     %rbx              /* Restores blended red component */
	pop 	%rsi	          /* Restores bg to stack */
    pop  	%rdi	          /* Restores fg to stack */
/* ------Blends b------ */	
	push	%rdi	          /* Stores fg to stack */
	push	%rsi	          /* Stores bg to stack */
	push    %rbx              /* Stores blended red component in stack */
	push    %rcx              /* Stores blended green component in stack */
    
	call    get_b             /* Gets blue component of fg */
	mov     %rax, %r8         /* Stores blue component of fg in %r8 */
    
	call    get_a             /* Gets alpha component of fg */
	mov     %rax, %r9         /* Stores alpha component of fg in %r9 */

	mov     %rsi, %rdi        /* Sets first parameter of get_g() as bg */
    call    get_b             /* Gets blue component of bg */
	mov     %rax, %r10        /* Stores blue component of bg in %r10 */

	mov     %r8, %rdi	      /* Sets 1st parameter of blend_components() */
	mov     %r10, %rsi        /* Sets 2nd parameter of blend_components() */
	mov     %r9, %rdx         /* Sets 3rd parameter of blend_components() */
	call    blend_components  
	mov     %rax, %r8         /* Stores blended b component in %8 */

    pop     %rcx              /* Restores blended green component */ 
	pop     %rbx              /* Restores blended red component */
	pop 	%rsi	          /* Restores bg to stack */
    pop  	%rdi	          /* Restores fg to stack */
/* ------Sets alpha------ */
    mov     $255, %r9         /* Sets alpha to 255 and stores it in %r9 */
/* ------Integrates-------*/
    shl     $24, %rbx         /* Right shift 24 bits for red component */
	shl     $16, %rcx         /* Right shift 16 bits for green component */
    shl     $8, %r8           /* Right shift 8 bits for blue component */

	add     %rbx, %rcx        /* red + green */
	add     %rcx, %r8         /* red + green + blue */
	add     %r8, %r9          /* red + green + blue + alpha */
    mov     %r9, %rax
    addq    $8, %rbp
	ret


/*
* Draws a single pixel to a destination image, blending the 
* specified foregroudn color with the existing background color,
* at a specified pixel index.
*
* Parameters:
*   %rdi     - pointer to struct Image
*   %rsi     - the index of the destination pixel
*   %rdx     - uint32_t color value
*/
	.globl set_pixel
set_pixel:
    subq    $8, %rbp

    movq    IMAGE_DATA_OFFSET(%rdi), %rcx         /* Stores pointer to data in %rcx */
	pushq   %rcx                                  /* Stores pointer to data in stack */
	pushq   %rsi                                  /* Stores index of the destination pixel in stack */
	movl    (%rcx, %rsi, 4), %ebx                 /* Store image bg in %ebx */
	
	movq    %rdx, %rdi                            /* Sets the first argument of blend_colors() */
	movl    %ebx, %esi                            /* Sets the second argument of blend_colors() */
	call    blend_colors
	
	popq    %rsi                                  /* Restores index of the destination pixel in stack */
	popq    %rcx                                  /* Restores pointer to data in %rcx */
	movl    %eax, (%rcx, %rsi, 4)                 /* Overrides new color at destination pixel */
	addq    $8, %rbp
	ret
	

/*
* Square an int64_t value.
*
* Parameters:
*   %rdi       - an int64_t value
*
* Return:
*   %rax       - the result of squaring an int64_t value
*/
	.globl square
square:
    subq   $8, %rbp
    movq   %rdi, %rax             /* Sets %rax %rbx to 0 */
	imulq  %rax, %rax             /* val*val */
	addq   $8, %rbp
	ret


/*
* Get the sum of the squares of the x and y distances
* between two points.
*
* Parameters:
*   %rdi      - x coordinate of one point
*   %rsi      - y coordinate of one point
*   %rdx      - x coordinate the other point
*   %rcx      - y coordinate the other point
*
* Return:
*   %rax      - the sum of the squares of the x and y distances 
*               between two points
*/
.globl square_dist
square_dist:
    subq    $8, %rbp
	movq	%rdi, %r8     	/* Init value of x1 */
	movq	%rsi, %r9		/* Init value of y1 */
	movq	%rdx, %r10		/* Init value of x2 */
	movq	%rcx, %r11		/* Init value of y2 */

	subq	%r8, %r10		/* x2-x1 */
	pushq	%rdi			/* Stores current %rdi to stack */
	movq	%r10, %rdi 		/* Sets the first parameter square() */
	call 	square			/* Calls function square() */
	popq	%rdi			/* Restores the first argument of current %rdi */
	movq 	%rax, %r10		/* Ges result of square (x2-x1) */

	subq	%r9, %r11		/* y2-y1 */
	pushq	%rdi			/* Stores current rdi to stack */
	movq	%r11, %rdi 		/* Sets the first parameter square() */
	call 	square			/* Calls function square() */
	popq	%rdi			/* Restores the first argument of current */
	movq 	%rax, %r11		/* Ges result of square (y2-y1) */
	
	addq 	%r10, %r11      /* (x2 - x1)^2 + (y2 - y1)^2 */
	movq 	%r11, %rax
    addq    $8, %rbp
	ret


/* TODO: implement your helper functions here */

/***********************************************************************
   Public API functions
 ***********************************************************************/

/*
 * Draw a pixel.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate (pixel column)
 *   %edx     - y coordinate (pixel row)
 *   %ecx     - uint32_t color value
 */
	.globl draw_pixel
draw_pixel:

	pushq 		%rbp 			# Use base pointer  
    movq  		%rsp, %rbp
    subq      	$8, %rbp       /* Stack pointer alignment */

	pushq     %rcx             /* Stores color value in stack */
	call      in_bounds        /* Checks if x, y in bound */
	popq      %rcx             /* Restores color value */
	cmpb      $1, %al          /* Checks result */
    jne       .LDone           /* If out of bound, done */
	call      compute_index    /* Computes index */
	
    movq      $0, %rsi         /* Init %rsi */
	movl      %eax, %esi       /* Sets the 2nd parameter of set_pixel() */
	movq      $0, %rdx         /* Init %rdx */
	movl      %ecx, %edx       /* Sets the 3rd parameter of set_pixel() */ 
    call      set_pixel        /* Sets pixel */

.LDone:
	addq      $8, %rbp
 	movq  	%rbp, %rsp  # Reset stack pointer and restore base  
    popq  	%rbp 
	ret


/*
 * Draw a rectangle.
 * The rectangle has rect->x,rect->y as its upper left corner,
 * is rect->width pixels wide, and rect->height pixels high.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - pointer to struct Rect
 *   %edx     - uint32_t color value
 */
	.globl draw_rect
draw_rect:

    subq    $8, %rbp
	pushq 	%rdi  		/*reserve Parameters*/
	pushq 	%rsi
	pushq	%rdx

	movl 	RECT_X_OFFSET(%rsi),%r8d 		/* store rect->x to px (r8)*/
	movl 	RECT_WIDTH_OFFSET(%rsi),%r9d  	/* store rect->width to r9*/
	addl	%r8d,%r9d			            /* store rect->width+rect->x to r9*/
	cmpl 	%r8d,%r9d			            /* compare */
	jle	 	.DR1			                /* if r9 <= r8, ret */
							                /* otherwise, continue*/
	
.DR2:
	movl 	RECT_Y_OFFSET(%rsi),%r10d 	        /* store rect->y to px (r10)*/
	movl 	RECT_HEIGHT_OFFSET(%rsi),%r11d  	/* store rect->length to r11 */
	addl	%r10d,%r11d			                /* store rect->length+rect->y to r11*/
	cmpl 	%r10d,%r11d			                /* compare */
	jle 		.DR3			                /* if r11 <= r10, break */		
							                    /* otherwise, continue*/

.DR4:
	pushq 	%rbp 				 # Use base pointer  
    movq  	%rsp, %rbp
	pushq	%r8
	pushq	%r9
	pushq	%r10
	pushq	%r11
	pushq 	%rdi
	pushq 	%rsi
	pushq	%rdx				 /*prepare to call function*/
	subq 	$72, %rsp

	movq 	%rdi, %rdi
	movl	%edx, %ecx				
	movl 	%r8d,%esi			
	movl 	%r10d,%edx

	call	draw_pixel

	addq 	$72, %rsp
	popq 	%rdx
	popq 	%rsi
	popq 	%rdi
	popq 	%r11
	popq 	%r10
	popq 	%r9
	popq 	%r8
	movq  	%rbp, %rsp
	popq  	%rbp

	addl	$1,%r10d

	cmpl 	%r10d,%r11d		/* compare */
							/* 	jle .DR3 if r11 <r10, break */		
	jg 		.DR4			/* r11> r10, continue*/
	
.DR3:
	addl	$1,%r8d
	cmpl 	%r8d,%r9d			        /* compare */
	jle	 	.DR1				        /* if r9 <= r8, ret */
	movl 	RECT_Y_OFFSET(%rsi),%r10d 	/* set r10 back to rect->y */
	jg		.DR4

.DR1:
	popq %rdx
	popq	%rsi
	popq 	%rdi
	addq    $8, %rbp
	ret


/*
 * Draw a circle.
 * The circle has x,y as its center and has r as its radius.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate of circle's center
 *   %edx     - y coordinate of circle's center
 *   %ecx     - radius of circle
 *   %r8d     - uint32_t color value
 */
	.globl draw_circle
draw_circle:
    
	subq    $8, %rbp
	movq 	%r8, %r15						/*store r8 to r15*/

	movl 	$0 ,%r8d 						/* store 0 to px (r8)*/
	movl 	IMAGE_WIDTH_OFFSET(%rdi),%r9d  	/* store image->width to r9*/
	cmpl 	%r8d,%r9d						/* compare */
	jle	 	.DC1							/* if r9 <= r8, ret */
											/* if r8 < r9 , continue*/
	
.DC2:
	movl 	$0,%r10d 						/* store 0 to r10*/
	movl 	IMAGE_HEIGHT_OFFSET(%rdi),%r11d /* store img->length to r11 */
	cmpl 	%r10d,%r11d						/* compare */
	jle 		.DC3						/* if r11 <= r10, break */		
											/* if r10 < r11, continue*/

.DC4:
    pushq    %rbp                              /*** DC4 start ***/
    movq     %rsp, %rbp
    pushq    %r8
    pushq    %r9
    pushq    %r10
    pushq    %r11
    pushq    %r15
    pushq    %rdi
    pushq    %rsi
    pushq    %rcx
    pushq    %rdx                              /*prepare to call function*/
    subq     $72, %rsp

    movl     %edx,%ecx                         /* y to 4th parameter*/
    movl     %esi,%edx                         /* x to 3rd parameter*/    
    movl     $0, %edi
    movl     %r8d, %edi                        /* px to 1st */
    movl     %r10d, %esi                       /* py to 2nd */    

    call     square_dist                       /******* SQUARE_DIST *******/    

    addq     $72, %rsp
    popq     %rdx
    popq     %rcx
    popq     %rsi
    popq     %rdi
    popq     %r15
    popq     %r11
    popq     %r10
    popq     %r9
    popq     %r8
    movq     %rbp, %rsp
    popq     %rbp

    pushq   %r8             
    movl    %ecx,%r8d                        /*move r to r8 */
    imul    %r8d,%r8d                        /* r times r*/
    cmpq    %r8,%rax            
    popq    %r8
    jg     .DC5                              /* if result > r^2, jump */
                                             /* if result <= r^2, continue */

/*-----------------------------------------------*/
    pushq    %rbp                            /***** result < r^2 *******/
    movq     %rsp, %rbp                      /***** PRINT here *******/
    pushq    %r8
    pushq    %r9
    pushq    %r10
    pushq    %r11
    pushq    %rdi
    pushq    %rsi
    pushq    %rcx
    pushq    %rdx                            /*prepare to call function*/
    subq     $72, %rsp

    movq     %rdi, %rdi
    movl     %r8d, %esi                
    movl     %r10d,%edx            
    movl     %r15d,%ecx

    call     draw_pixel                      /************ DRAW_PIXEL *************/

    addq     $72, %rsp
    popq     %rdx
    popq     %rcx
    popq     %rsi
    popq     %rdi
    popq     %r11
    popq     %r10
    popq     %r9
    popq     %r8
    movq     %rbp, %rsp
    popq     %rbp
/*---------------------------------------------------------*/
.DC5:
	addl	$1,%r10d				/***********DC5 strat***********/
	cmpl 	%r10d,%r11d				/* compare */
	jg 		.DC4					/* r11> r10 , loop */
									/* r10 >= r10 , go to DC3*/

.DC3:
	addl	$1,%r8d
	cmpl 	%r8d,%r9d				/* compare */
	jle	 	.DC1					/* if r9 <= r8, ret */
	movl 	$0,%r10d 				/* set r10 back to 0 */
	jg		.DC4					/* r8 < r9, and then jump back to DC4*/

.DC1:
    addq    $8, %rbp
	ret


/*
 * Draw a tile by copying all pixels in the region
 * enclosed by the tile parameter in the tilemap image
 * to the specified x/y coordinates of the destination image.
 * No blending of the tile pixel colors with the background
 * colors should be done.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where tile should be copied
 *   %edx - y coordinate of location where tile should be copied
 *   %rcx - pointer to Image (the tilemap)
 *   %r8  - pointer to Rect (the tile)
 */
	.globl draw_tile
draw_tile:
	/* TODO: implement */
	ret

/*
 * Draw a sprite by copying all pixels in the region
 * enclosed by the sprite parameter in the spritemap image
 * to the specified x/y coordinates of the destination image.
 * The alpha values of the sprite pixels should be used to
 * blend the sprite pixel colors with the background
 * pixel colors.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where sprite should be copied
 *   %edx - y coordinate of location where sprite should be copied
 *   %rcx - pointer to Image (the spritemap)
 *   %r8  - pointer to Rect (the sprite)
 */
	.globl draw_sprite
draw_sprite:
	/* TODO: implement */
	ret

/*
vim:ft=gas:
*/
