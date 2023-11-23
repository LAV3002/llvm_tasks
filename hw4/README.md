# Мои команды
alloca reg imm - выделить на куче imm * 4 байт и пололжить начало отрезка в reg

seti reg1 reg2 imm - положить по адресу reg1 + reg2 imm

get reg1 reg2 reg3 - пложить в reg1 значение по адресу reg2 + reg2

addrmi reg1 reg2 imm - reg1 = (reg2 + imm) % imm

binci reg1 imm1 imm2 - reg1 += 1 и b на imm2 если reg1 меньше imm1
