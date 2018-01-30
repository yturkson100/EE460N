		.ORIG x3000


Pseudo:
R0 <- content of x4000

R1 <- content of x4001

R2 <- content of x4002

R3 <- content of x4003

R4 <- content of x4004 (control)

Check bits [1:0]
if (0:0){ 
	nop
}
if (0:1){
	R5 <- R0
	R0 <R1
}
