# Мои команды
alloca reg imm - выделить на куче imm * 4 байт и пололжить начало отрезка в reg\br

seti reg1 reg2 imm - положить по адресу reg1 + reg2 imm\br

get reg1 reg2 reg3 - пложить в reg1 значение по адресу reg2 + reg2\br

addrmi reg1 reg2 imm - reg1 = (reg2 + imm) % imm\br

binci reg1 imm1 imm2 - reg1 += 1 и b на imm2 если reg1 меньше imm1\br
