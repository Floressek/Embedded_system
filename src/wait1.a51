;Autor: dr inz. Krzysztof Murawski - kod wykorzystany do projektu przez SZYMON FLOREK WCY22IY2S1

NAME 	wait
EXTRN   DATA (Var1, Var2, Var3)
PUBLIC	w1ms, w5mS, w50mS
Wait_ROUTINES  SEGMENT  CODE

		RSEG  Wait_ROUTINES
//przypisz wartosci
w1ms:                 	; 
		MOV	 Var2,#4    ; 
		MOV	 Var1,#224  ; 
//wykonuj skoki na siebie Var2 razy till Var2 == 0
		
TT2:	DJNZ Var1,TT2   ; 
	    DJNZ Var2,TT2   ; 
	    RET             ;

w5mS:                	; 
		MOV	 Var2,#20   ; 
		MOV	 Var1,#112  ;  
TT3:	DJNZ Var1,TT3   ; 
	    DJNZ Var2,TT3   ; 
	    RET  	        ;

w50mS:              	;
		MOV	 Var2,# 195 ;
		MOV	 Var1,# 137 ;
		
TT5:	DJNZ Var1,TT5   ;
	    DJNZ Var2,TT5   ;	
	    RET  	
END
	


