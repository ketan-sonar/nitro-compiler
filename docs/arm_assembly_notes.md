# Assembly Language for Apple Silicon

## Define the starting point

```asm
.global _start

_start:
    // code goes here
```

## Set .align 2

Don't know why, but we have to do this.

```asm
.global _start
.align 2
// ...
```

## Put value in registers

```asm
mov X0, #10
```

This instruction puts the value `10` in the `X0` register.  
You can put any value in place of `10`.  
You can also put hexadecimal values like `0x3a`.  

## Copy value from one register to another

```asm
mov X0, X1
```

This instruction copies the value from the `X1` register to the `X0` register.  

## Define procedure and call it

```asm
.global _start
.align 2

_start:
    b _procedure

_procedure:
    mov X0, #10
```

Here, a new procedure called `_procedure` is defined.  
It is then called in the `_start` procedure using the `b` (meaning `branch`) instruction.  

## Exit the program

```asm
.global _start
.align 2

_start:
    mov X0, #0 // this is the exit code
    mov X16, #1
    svc #0
```

## Push a value onto the stack

```asm
.global _start
.align 2

_start:
    mov X0, #10
    str X0, [sp, #-0x10]!

    mov X0, #0
    mov X16, #1
    svc
```

## Pop value from stack

```asm
.global _start
.align 2

_start:
    ldr X0, [sp]

    mov X0, #0
    mov X16, #1
    svc
```
