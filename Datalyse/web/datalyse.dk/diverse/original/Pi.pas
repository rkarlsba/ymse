program pi1; { Carl Hemmingsen 11/12-1984, februar 1993 }
{$N+}
{ Turbo Pascal version 6.0 }
{ Programmet m† ikke udnyttes ›konomisk uden forfatterens tilladelse!!}


uses	dos,crt;
const	maxantal = 30002;
	{ pi/4 = 4*arctan(1/5) - arctan(1/239)
	  arctan(x) = x - x'3/3 + x'5/5 - ...}
	escape= #27;

type	pi_ptrtype = array[-1..maxantal] of integer;
	invtg_ptrtype = array[0..maxantal] of integer;
	str2 = string[2];

var	nedregr,ovregr:word;
	restx: word;
	restled: word;
	i,j: integer;
	n, antal, hjalp: word;
	x: array[0..maxantal] of word;
	invtg: ^invtg_ptrtype; 	{ det skal v‘re integer }
	pi: ^pi_ptrtype; 	{ det skal v‘re integer !!! }
	divisor: word;
	reg: registers;
	tid, tid1, tid2: double;
	fortegn: integer;
	mintg, maxtg: integer;
	ch: char;
	enhed: text;
	printer: boolean;
	pi10: string[20];

function timer: double;
var	reg:registers;
begin with reg do begin
	ah:=44; intr($21,reg);
	timer:=hi(cx)*3600.0+lo(cx)*60.0+hi(dx) + 0.01*lo(dx);
end; end; (*timer*)

procedure ADD;
begin
	restx:=0; restled:=0;
  	asm
                mov si,offset x		{ si := adressen p† tabellen x }
                les di,invtg  		{ di:= adressen p† tabellen invtg }
                mov bx,nedregr
                shl bx,1        	{ x og invtg er i word }
                add di,bx       	{ opdatering af adresser }
                add si,bx 		{ - }
	        mov cx,ovregr
	        sub cx,nedregr  	{ antal genneml›b }
	        mov ax,restx            {rest x }
	        mov bx,restled
         @next: push cx         	{ cx skrives p† stakken }
	        cwd			{ convert word til dword: ax,dx }
	        mov cx,100 		{ restx*100 }
	        mul cx
	        add ax,[si] 		{ restx*100 + x[i] }
	        adc dx,0 		{ adder eventuel mente til dx }
	        div divisor
	        mov [si],ax     	{ x[i] skrives ud i lageret }
	        push dx 		{ restx skrives p† stakken }
	        push ax 		{ x[i]          p† stakken }
	        mov ax,bx 		{ restled flyttes til ax }
	        cwd
	        mul cx 			{ restled*100 }
	        pop bx 			{ x[i] l‘ses fra stakken }
	        add ax,bx 		{ restled*100 + x[i] }
	        adc dx,0
	        div n                   { n = 1,3,5,... }
	        mov bx,dx 		{ restled }
	        add word ptr [es:di],ax	{ invtg ›ges med ax }
                inc di                  { ny position i invtg tabel }
		inc di
	        inc si      		{ ny position i x tabel }
                inc si
	        pop ax			{ rest x }
	        pop cx			{ antal genneml›b }
          loop @next
	        mov restled,bx
          end;

	while (x[nedregr]=0) and (nedregr<ovregr) do inc(nedregr);
      	if 2*restled > n then invtg^[ovregr-1]:= invtg^[ovregr-1]+fortegn;
     	write(#13, n:5)

	{ i pascal:
       	for i:= nedregr to ovregr do begin
		dividendx:= x[i] + restx*100;
		x[i]:= dividendx div divisor;
		restx:= dividendx mod divisor;
                dummy:= restled;
                dummy:= dummy*100 + x[i];
                dividendl:= dummy div n;
                restled:= dummy - dividendl*n;
                invtg^[i]:= invtg^[i] + dividendl*fortegn; }

	{ bem‘rk, at x + restx*100  < 99 + 239*239*100 = 5722000.
          hvilket er st›rre end word-gr‘nsen, men i maskinkoden
          udvides word til dobbelt word!
       	  det samme g‘lder dummy < (n-1)*100 + 99 > word-gr‘nsen. }

    
end;	{ ADD }

procedure SUB;		{ se forklaring i add }
begin
	restx:=0; restled:=0;
  	asm
                mov si,offset x
                les di,invtg
                mov bx,nedregr
                shl bx,1
                add di,bx
                add si,bx
	        mov cx,ovregr
	        sub cx,nedregr
	        mov ax,restx
	        mov bx,restled
         @next: push cx
	        cwd
	        mov cx,100
	        mul cx
	        add ax,[si]
	        adc dx,0
	        div divisor
	        mov [si],ax
	        push dx
	        push ax
	        mov ax,bx
	        cwd
	        mul cx
	        pop bx
	        add ax,bx
	        adc dx,0
	        div n
	        mov bx,dx
	        sub word ptr[es:di],ax
	        add di,2
                add si,2
	        pop ax
	        pop cx
          loop @next
	        mov restled,bx
          end;

	while (x[nedregr]=0) and (nedregr<ovregr) do inc(nedregr);
       	if 2*restled > n then invtg^[ovregr-1]:= invtg^[ovregr-1]+fortegn;
     	write(#13, n:5)
end;	{ SUB }

function inttostr(k: integer): str2;
var	s: str2;
begin
	str(k:1, s); if length(s)=1 then s:= '0'+s;
        inttostr:=s;
end;

	{ ==================================================== }
procedure beregn_pi;
begin
        clrscr;
        write(' hvor mange decimaler: (100..30000): '); readln(antal);
	tid:= timer;
        if antal < 100 then antal:= 100;
        if antal > maxantal then antal:= maxantal;
        antal:= (antal div 10)*10;
	ovregr:= 2*(antal div 4);
        ovregr:= ovregr + 2; { der er afrundingsfejl p† de sidste cifre }
	writeln('Udregning af pi med ', antal, ' decimaler..');
        writeln;
        writeln('udregning af invtg(1/5), ›vre gr‘nse ca. ',(antal+4)*ln(10)/ln(5):1:0);
        getmem(pi, sizeof(pi_ptrtype));
        getmem(invtg, sizeof(invtg_ptrtype));
	fillchar(x, sizeof(x), #0);
	fillchar(invtg^, sizeof(invtg_ptrtype), #0);
	n:=1; x[0]:=100; nedregr:=0; fortegn:=1;
	divisor:= 5;
     	ADD;
	divisor:= 25;
	while nedregr < ovregr do begin
		inc(n,2); fortegn:=-1;
		SUB;
                inc(n,2); fortegn:= 1;
                if nedregr < ovregr then ADD
 	end;
{  	mintg:= 0; maxtg:= 0;
        for j:= 0 to ovregr do begin
            	if invtg^[j]<mintg then mintg:= invtg^[j];
                if invtg^[j]>maxtg then maxtg:= invtg^[j];
        end;
        writeln;
        writeln('mintg:',mintg:10, ' maxtg:',maxtg:10);
 }	for j:= ovregr downto 1 do
		if invtg^[j] < 0 then begin
			hjalp:= (-invtg^[j] div 100) + 1;
			dec(invtg^[j-1], hjalp);
			inc(invtg^[j], hjalp*100)
		end;
  	for j:= ovregr downto 1 do begin
                inc(invtg^[j-1], invtg^[j] div 100);
                invtg^[j]:= invtg^[j] mod 100;
	end;
 {   	mintg:= 0; maxtg:= 0;
        for j:= 0 to ovregr do begin
            	if invtg^[j]<mintg then mintg:= invtg^[j];
                if invtg^[j]>maxtg then maxtg:= invtg^[j];
        end;
        writeln;
        writeln('mintg:',mintg:10, ' maxtg:',maxtg:10);
 }	for j:= 0 to ovregr do pi^[j]:= invtg^[j]*16;
        tid1:= timer -tid;
        writeln;
       	writeln('tid:  ', tid1:5:2, ' s');
	pi^[-1]:= 0;
        writeln;
        writeln('udregning af invtg(1/239), ›vre gr‘nse ca. ',(antal+4)*ln(10)/ln(239):1:0);

       	fillchar(x, sizeof(x), #0);
	fillchar(invtg^, sizeof(invtg_ptrtype), #0);
  	n:=1; x[0]:=100; nedregr:= 0; fortegn:=1;
	divisor:= 239;
     	ADD;
        divisor:= 239*239;
	while nedregr < ovregr do begin
		inc(n,2); fortegn:= -1;
		SUB;
                inc(n,2); fortegn:= 1;
                if nedregr < ovregr then ADD;
	end;
 {    	mintg:= 0; maxtg:= 0;
        for j:= 0 to ovregr do begin
            	if invtg^[j]<mintg then mintg:= invtg^[j];
                if invtg^[j]>maxtg then maxtg:= invtg^[j];
        end;
        writeln;
        writeln('mintg:',mintg:10, ' maxtg:',maxtg:10);
 }	for j:= ovregr downto 1 do
		if invtg^[j] < 0 then begin
			hjalp:= (-invtg^[j] div 100) + 1;
			dec(invtg^[j-1], hjalp);
			inc(invtg^[j], hjalp*100)
		end;
	for j:= ovregr downto 1 do begin
                inc(invtg^[j-1], invtg^[j] div 100);
                invtg^[j]:= invtg^[j] mod 100;
 	end;
 {	mintg:= 0; maxtg:= 0;
        for j:= 0 to ovregr do begin
            	if invtg^[j]<mintg then mintg:= invtg^[j];
                if invtg^[j]>maxtg then maxtg:= invtg^[j];
        end;
        writeln;
        writeln('mintg:',mintg:10, ' maxtg:',maxtg:10);
 }	for j:= 0 to ovregr do dec(pi^[j], 4*invtg^[j]);

	for j:= ovregr downto 0 do begin
		if pi^[j] < 0 then begin
			hjalp:= (-pi^[j] div 100) + 1;
			dec(pi^[j-1], hjalp);
			inc(pi^[j], hjalp*100)
		end;
	end;
	for j:= ovregr downto 0 do begin
                inc(pi^[j-1], pi^[j] div 100);
                pi^[j]:= pi^[j] mod 100;
	end;
	writeln;
        tid2:=timer-tid;
	writeln('tid:  ', tid2-tid1:5:2, ' s');
        writeln('ialt: ', tid2:5:2, ' s');
        writeln;
	write('udskrift p† sk‘rm/printer/fil (s/p/f) ? '); ch:= readkey;
	case ch of
	'p','P': begin
		printer:= true;
		assign(enhed, 'lpt1'); rewrite(enhed);
	end;
	'f','F': begin
	    printer:= true;
	    assign(enhed,'pifil'); rewrite(enhed);
	end;
	else begin
		printer:= false; assign(enhed, 'CON'); rewrite(enhed);
	end;
	end;
        writeln(enhed); writeln(enhed);
	write(enhed, 'pi = ',pi^[-1],'.');
        j:= 0;
        ovregr:= ovregr-2;
        while j < ovregr-1 do begin
           pi10:= '';
           for i:= 0 to 4 do pi10:= pi10 + inttostr(pi^[j+i]);
           insert(' ',pi10,6);
           write(enhed, pi10,' ');
           inc(j,5);
           if j mod 25 = 0 then begin writeln(enhed); write(enhed,'       ') end;
           if j mod 500 = 0 then begin writeln(enhed); write(enhed, '       ') end;
        end;
        writeln(enhed);
        freemem(pi, sizeof(pi_ptrtype));
        freemem(invtg, sizeof(invtg_ptrtype));
	writeln(enhed, 'tid: ', tid2:5:2);
        if printer then begin write(enhed,#12); close(enhed) end;
	write('tast retur '); readln;
end;
begin
        ch:=' ';
        repeat
		beregn_pi;
                write('tast retur eller esc: '); ch:= readkey;
        until ch=escape;
end.

{ Bem‘rk, at der er 3 fordele ved lidt maskinkode:
	1: div udregner b†de rest og kvotient i dx:ax
        2: udvidelse fra 16 bit til 32 bit internt
        3: mindre trafik p† bussen (AT-bus: 8MHz mod CPU'ens f.eks. 33MHz)
}
