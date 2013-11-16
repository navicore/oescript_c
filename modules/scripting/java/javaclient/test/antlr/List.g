grammar List;
options {output=AST;}
r : (ID|INT)+ ;
ID : 'a'..'z'+ ;
INT : '0'..'9'+;
WS : (' '|'\n'|'\r') {$channel=HIDDEN;} ;

